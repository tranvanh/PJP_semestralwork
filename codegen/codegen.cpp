//
// Created by Tomas Tran on 04/06/2020.
//
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm-c/Target.h"

#include "../ast/data_types/ASTVariableType.hpp"
#include "../ast/ASTProgram.hpp"
#include "../ast/variable/ASTConstVariable.hpp"
#include "../ast/statements/ASTExit.hpp"
#include "../ast/statements/ASTBreak.hpp"
#include "../ast/statements/ASTWhile.hpp"
#include "../ast/statements/ASTFor.hpp"
#include "../ast/statements/ASTIf.hpp"
#include "../ast/references/ASTArrayReference.hpp"
#include "../ast/references/ASTSingleVarReference.hpp"
#include "../ast/operators/ASTBinaryOperator.hpp"
#include "../ast/operators/ASTAssignOperator.hpp"
#include "../ast/function/ASTFunctionCall.hpp"
#include "../ast/data_types/ASTArray.hpp"
#include "../ast/data_types/ASTInteger.hpp"
#include "../ast/constants/ASTString.hpp"
#include "../ast/constants/ASTNumber.hpp"

using namespace llvm;

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;

typedef std::pair<Value *, std::shared_ptr<ASTVariableType>> TVarInfo;

static std::map<std::string, std::pair<AllocaInst *, std::shared_ptr<ASTVariableType>>> named_values;
static std::map<std::string, std::pair<GlobalVariable *, std::shared_ptr<ASTVariableType>>> global_vars;
static std::map<std::string, llvm::Constant *> const_vars;

static Value *decimal_specifier_character;
static Value *string_specifier_character;
static Value *new_line_specifier;


Value *ASTProgram::codegen() {
    // Printf and scanf declarations
    PointerType *ptr = PointerType::get(IntegerType::get(TheContext, 8), 0);
    FunctionType *function_type = FunctionType::get(IntegerType::get(TheContext, 32), ptr, true);
    Function *llvm_printf = Function::Create(function_type, Function::ExternalLinkage, "printf", TheModule.get());
    llvm_printf->setCallingConv(CallingConv::C);

    Function *llvm_scanf = Function::Create(function_type, Function::ExternalLinkage, "scanf", TheModule.get());
    llvm_scanf->setCallingConv(CallingConv::C);

    function_type = FunctionType::get(IntegerType::getInt32Ty(TheContext), {}, false);

    Function *program_func = Function::Create(function_type, Function::ExternalLinkage, "main", TheModule.get());
    program_func->setCallingConv(CallingConv::C);

    BasicBlock *program_BB = BasicBlock::Create(TheContext, "main_BB", program_func);
    //Builder.CreateBr(program_BB);
    Builder.SetInsertPoint(program_BB);


    decimal_specifier_character = Builder.CreateGlobalStringPtr("%d");
    string_specifier_character = Builder.CreateGlobalStringPtr("%s");
    new_line_specifier = Builder.CreateGlobalStringPtr("\n");


    for (auto &var : m_Global)
        var->codegen();

    for (auto &f : m_Functions)
        f->codegen();

    Builder.SetInsertPoint(program_BB);
    auto v = m_Main->codegen();
    if (v)
        return Builder.CreateRet(ConstantInt::get(TheContext, APInt(32, 0, false)));
    else
        return Builder.CreateRet(ConstantInt::get(TheContext, APInt(32, 1, false)));
}


// Does the magic
std::unique_ptr<Module> ASTProgram::runCodegen(const std::string &output_file) {
    // Create a new pass manager attached to it.

    TheModule = std::make_unique<Module>("main_module", TheContext);

    auto res = codegen();


    if (!res)
        return nullptr;

    // llvm::outs() raw_ostream for std::cout
    TheModule->print(outs(), nullptr);



    // GENERATE OBJECT FILE
    // Initialize the target registry etc.
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto TargetTriple = sys::getDefaultTargetTriple();
    TheModule->setTargetTriple(TargetTriple);

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
        errs() << Error;
        return nullptr;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TheTargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    TheModule->setDataLayout(TheTargetMachine->createDataLayout());

    std::error_code EC;
    raw_fd_ostream dest(output_file, EC, sys::fs::F_None);

    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return nullptr;
    }

    legacy::PassManager pass;
    auto file_type = TargetMachine::CGFT_ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, file_type)) {
        errs() << "TheTargetMachine can't emit a file of this type";
        return nullptr;
    }

    assert(!verifyModule(*TheModule, &errs()));

    pass.run(*TheModule);

    dest.flush();

    outs() << "Wrote " << output_file << "\n";

    return nullptr;
}

Value *ASTBody::codegen() {
    for (auto &statement : m_Content)
        statement->codegen();

    return Constant::getNullValue(Type::getInt32Ty(TheContext));
}

Value *ASTNumber::codegen() {
    return ConstantInt::get(TheContext, APInt(32, m_Value, true));
}

Value *ASTString::codegen() {
    return Builder.CreateGlobalString(StringRef(m_Str));
}

Type *ASTInteger::codegen() {
    return Type::getInt32Ty(TheContext);
}

Type *ASTArray::codegen() {
    Type *elem_type = m_Type->codegen();
    int size = (m_UpperIdx->m_Value) - (m_LowerIdx->m_Value) + 1;
    return ArrayType::get(elem_type, size);
}

static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, Type *type) {
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());

    auto init_value = ConstantInt::get(Type::getInt32Ty(TheContext),
                                       (type->isArrayTy() ? type->getArrayNumElements() : 0), false);
    return TmpB.CreateAlloca(type, init_value, VarName.c_str());
}

Value *ASTFunctionCall::codegen() {
    if (m_Name == "writeln" || m_Name == "write") {
        if (m_Arguments.size() == 0) {
            if (m_Name == "writeln")
                Builder.CreateCall(TheModule->getFunction("printf"), {new_line_specifier}, "call_printf");
            return nullptr;
        }
        auto value = m_Arguments[0]->codegen();

        Value *res;
        if (value->getType() == Type::getInt32Ty(TheContext))
            res = Builder.CreateCall(TheModule->getFunction("printf"), {decimal_specifier_character, value},
                                     "call_printf");
        else
            res = Builder.CreateCall(TheModule->getFunction("printf"), {string_specifier_character, value},
                                     "call_printf");

        if (m_Name == "writeln")
            Builder.CreateCall(TheModule->getFunction("printf"), {new_line_specifier, value}, "call_printf");
        return res;
    } else if (m_Name == "readln") {
        if (m_Arguments.size() == 0)
            return nullptr;

        ASTSingleVarReference *var = (ASTSingleVarReference *) m_Arguments[0].get();
        Value *alloca = var->getAlloc();
        if (!alloca)
            return nullptr;

        return Builder.CreateCall(TheModule->getFunction("scanf"), {decimal_specifier_character, alloca}, "call_scanf");
    } else if (m_Name == "dec") {
        if (m_Arguments.size() == 0)
            return nullptr;

        ASTSingleVarReference *var = (ASTSingleVarReference *) m_Arguments[0].get();
        Value *alloca = var->getAlloc();
        if (!alloca)
            return nullptr;
        Value *current_value = var->codegen();
        Value *new_value = Builder.CreateSub(current_value, ConstantInt::get(TheContext, APInt(32, 1, true)));

        return Builder.CreateStore(new_value, alloca);
    } else {
        Function *f = TheModule->getFunction(m_Name);
        if (!f) {
            printf("Error: Unknown function referenced\n");
            return nullptr;
        }

        if (f->arg_size() != m_Arguments.size()) {
            printf("Error: Incorrect number of m_Arguments passed\n");
            return nullptr;
        }

        // Generate argument expr
        std::vector<Value *> arg_values;
        for (auto &arg : m_Arguments)
            arg_values.push_back(arg->codegen());

        return Builder.CreateCall(f, arg_values);
    }
}

Function *ASTFunctionPrototype::codegen() {
    std::vector<Type *> param_types;
    for (auto &param : m_Parameters)
        param_types.push_back(param->m_Type->codegen());

    FunctionType *function_type;
    if (m_ReturnType) // Function
        function_type = FunctionType::get(m_ReturnType->codegen(), param_types, false);
    else  // Procedure
        function_type = FunctionType::get(Type::getVoidTy(TheContext), param_types, false);

    Function *function = Function::Create(function_type, Function::ExternalLinkage, m_Name, TheModule.get());

    // Set names for m_Arguments to match m_Prototype parameters
    unsigned i = 0;
    for (auto &param : function->args())
        param.setName(m_Parameters[i++]->m_Name);

    return function;
}

Function *ASTFunction::codegen() {
    // Lookup function declaration
    Function *function = TheModule->getFunction(m_Prototype->getName());
    if (!function) // Not yet generated
        function = m_Prototype->codegen();
    if (!function || !m_Body)
        return nullptr;

    // Create a new basic block to start insertion into.
    BasicBlock *function_BB = BasicBlock::Create(TheContext, "function_block: " + m_Prototype->getName(), function);
    BasicBlock *return_BB = BasicBlock::Create(TheContext, "return_block: " + m_Prototype->getName(), function);

    Builder.SetInsertPoint(function_BB);

    // Remember the old variable binding so that we can restore the binding when
    // we unrecurse.
    std::map<std::string, std::pair<AllocaInst *, std::shared_ptr<ASTVariableType>>> old_named_values(named_values);

    // Save function m_Arguments so they can be used as local variables
    int idx = 0;
    for (auto &arg : function->args()) {
        // Create an alloca for arg
        AllocaInst *alloca = CreateEntryBlockAlloca(function, arg.getName(), arg.getType());
        // Store arg into the alloca.
        Builder.CreateStore(&arg, alloca);
        // Add m_Arguments to variable symbol table.
        named_values[arg.getName()] = std::make_pair(alloca, m_Prototype->m_Parameters[idx++]->m_Type);
    }

    // Local variables
    for (auto &var : m_LocalVariables) {
        auto type_value = var->m_Type->codegen();
        AllocaInst *alloca = CreateEntryBlockAlloca(function, var->m_Name, type_value);
        named_values[var->m_Name] = std::make_pair(alloca, var->m_Type);
    }

    // Return variable for functions
    if (m_Prototype->m_ReturnType) {
        AllocaInst *alloca = CreateEntryBlockAlloca(function, m_Prototype->getName(),
                                                    m_Prototype->m_ReturnType->codegen());
        named_values[m_Prototype->getName()] = std::make_pair(alloca, m_Prototype->m_ReturnType);;
    }



    auto body_value = m_Body->codegen();
    if (!body_value)
        return nullptr;


    Builder.CreateBr(return_BB);
    Builder.SetInsertPoint(return_BB);

    if (m_Prototype->m_ReturnType) {
        auto return_value = Builder.CreateLoad(named_values[m_Prototype->getName()].first);
        Builder.CreateRet(return_value);
    } else {
        Builder.CreateRetVoid();
    }


    assert(!verifyFunction(*function, &errs()));

    named_values = old_named_values;

    return function;
}

Value *ASTAssignOperator::codegen() {
    // Find alloca address of left side
    Value *alloca = m_Variable->getAlloc();
    if (!alloca)
        return nullptr;

    Value *new_value = m_Value->codegen();
    if (!new_value)
        return nullptr;

    Builder.CreateStore(new_value, alloca);

    return new_value;
}


Value *ASTBinaryOperator::codegen() {
    Value *left = m_LHS->codegen();
    Value *right = m_RHS->codegen();

    if (!left || !right)
        return nullptr;

    Value *bit_result;
    switch (m_Op) {
        case tok_plus:
            bit_result = Builder.CreateAdd(left, right, "add");
            break;
        case tok_minus:
            bit_result = Builder.CreateSub(left, right, "sub");
            break;
        case tok_multiply:
            bit_result = Builder.CreateMul(left, right, "mul");
            break;
        case tok_div:
            bit_result = Builder.CreateSDiv(left, right, "div");
            break;
        case tok_mod:
            bit_result = Builder.CreateSRem(left, right, "mod");
            break;
        case tok_equal:
            bit_result = Builder.CreateICmpEQ(left, right, "eq");
            break;
        case tok_notEqual:
            bit_result = Builder.CreateICmpNE(left, right, "neq");
            break;
        case tok_less:
            bit_result = Builder.CreateICmpSLT(left, right, "less");
            break;
        case tok_lessEqual:
            bit_result = Builder.CreateICmpSLE(left, right, "lessEq");
            break;
        case tok_greater:
            bit_result = Builder.CreateICmpSGT(left, right, "greater");
            break;
        case tok_greaterEqual:
            bit_result = Builder.CreateICmpSGE(left, right, "greaterEq");
            break;
        case tok_and:
            bit_result = Builder.CreateAnd(left, right, "and");
            break;
        case tok_or:
            bit_result = Builder.CreateOr(left, right, "or");
            break;
        default:
            bit_result = nullptr;
            break;
    }
    return Builder.CreateIntCast(bit_result, Type::getInt32Ty(TheContext), true);
}

Value *ASTSingleVarReference::codegen() {
    if (const_vars.find(m_Name) != const_vars.cend())
        return const_vars[m_Name];

    // Search for var
    TVarInfo info;

    if (named_values.find(m_Name) != named_values.end())
        info = named_values[m_Name];
    else if (global_vars.find(m_Name) != global_vars.end())
        info = global_vars[m_Name];
    else
        throw "Using an undeclared variable";

    return Builder.CreateLoad(info.first, m_Name);
}

// Find variable address
Value *ASTSingleVarReference::getAlloc() {
    TVarInfo info;
    if (named_values.find(m_Name) != named_values.cend())
        info = named_values[m_Name];
    else if (global_vars.find(m_Name) != global_vars.cend())
        info = global_vars[m_Name];

    return info.first;
}

// Get array elem value from stack
Value *ASTArrayReference::codegen() {
    return Builder.CreateLoad(getAlloc());
}

// Find array elem address
Value *ASTArrayReference::getAlloc() {
    // Search for var
    TVarInfo info;
    if (named_values.find(m_Name) != named_values.cend())
        info = named_values[m_Name];
    else if (global_vars.find(m_Name) != global_vars.cend())
        info = global_vars[m_Name];
    else
        throw "Undeclared array variable";

    // Calculating elem address
    std::vector<Value *> idx_list;
    auto start_idx = std::dynamic_pointer_cast<ASTArray>(info.second)->m_LowerIdx->codegen();
    auto idx = Builder.CreateSub(m_Index->codegen(), start_idx);

    idx_list.push_back(ConstantInt::get(Type::getInt32Ty(TheContext), 0));
    idx_list.push_back(idx);

    return Builder.CreateGEP(info.first, idx_list);
}

Value *ASTIf::codegen() {
    Value *condition_value = m_Condition->codegen();
    if (!condition_value)
        return nullptr;

    condition_value = Builder.CreateICmpNE(condition_value, ConstantInt::get(TheContext, APInt(32, 0, true)),
                                           "if_condition");

    Function *parent = Builder.GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    BasicBlock *then_BB = BasicBlock::Create(TheContext, "then", parent);
    BasicBlock *else_BB = BasicBlock::Create(TheContext, "else", parent);
    BasicBlock *after_BB = BasicBlock::Create(TheContext, "after_block", parent);

    Builder.CreateCondBr(condition_value, then_BB, else_BB);


    // Then body
    Builder.SetInsertPoint(then_BB);
    Value *then_value = m_ThenBody->codegen();
    if (!then_value)
        return nullptr;
    Builder.CreateBr(after_BB);


    // Else body
    Builder.SetInsertPoint(else_BB);
    Value *else_value = nullptr;
    if (m_ElseBody) {
        else_value = m_ElseBody->codegen();
        if (!else_value)
            return nullptr;
    }
    Builder.CreateBr(after_BB);


    Builder.SetInsertPoint(after_BB);

    return Constant::getNullValue(Type::getInt32Ty(TheContext));

}

Value *ASTFor::codegen() {
    // Search for control_variable
    TVarInfo info;
    if (named_values.find(m_VarName) != named_values.cend())
        info = named_values[m_VarName];
    else if (global_vars.find(m_VarName) != global_vars.cend())
        info = global_vars[m_VarName];
    else
        throw "Using an undeclared variable in for statement";

    // Create an alloca for the variable in the entry block.
    //AllocaInst * Alloca = CreateEntryBlockAlloca(parent, variable_name, Type::getInt32Ty(TheContext));

    // Codegen start value
    Value *start_value = m_Start->codegen();
    if (!start_value)
        return nullptr;
    // Codegen end value
    Value *end_value = m_End->codegen();
    if (!end_value)
        return nullptr;

    // Store the value into the alloca.
    Builder.CreateStore(start_value, info.first);

    Function *parent = Builder.GetInsertBlock()->getParent();
    BasicBlock *condition_BB = BasicBlock::Create(TheContext, "for_condition", parent);
    BasicBlock *body_BB = BasicBlock::Create(TheContext, "for_lopp", parent);
    BasicBlock *after_BB = BasicBlock::Create(TheContext, "after_block", parent);

    // Condition
    Builder.CreateBr(condition_BB);
    Builder.SetInsertPoint(condition_BB);
    Value *for_condition;
    if (m_DownTo)
        for_condition = Builder.CreateICmpSGE(Builder.CreateLoad(info.first, m_VarName), end_value);
    else
        for_condition = Builder.CreateICmpSLE(Builder.CreateLoad(info.first, m_VarName), end_value);

    Builder.CreateCondBr(for_condition, body_BB, after_BB);

    // Loop branch
    Builder.SetInsertPoint(body_BB);

    if (!m_Body->codegen())
        return nullptr;

    // Calculate Next Value
    Value *current_value = Builder.CreateLoad(info.first, m_VarName);
    Value *next_value = nullptr;
    Value *step_value = ConstantInt::get(TheContext, APInt(32, 1, true));
    if (m_DownTo)
        next_value = Builder.CreateSub(current_value, step_value, "next_value");
    else
        next_value = Builder.CreateAdd(current_value, step_value, "next_value");

    // Save to alloca
    Builder.CreateStore(next_value, info.first);

    Builder.CreateBr(condition_BB);



    // After for cycle
    Builder.SetInsertPoint(after_BB);

    // for expr always returns 0
    return Constant::getNullValue(Type::getInt32Ty(TheContext));
}

Value *ASTWhile::codegen() {
    Function *parent = Builder.GetInsertBlock()->getParent();
    BasicBlock *condition_BB = BasicBlock::Create(TheContext, "while_condition", parent);
    BasicBlock *body_BB = BasicBlock::Create(TheContext, "while_loop", parent);
    BasicBlock *after_BB = BasicBlock::Create(TheContext, "after_block", parent);

    // Condition
    Builder.CreateBr(condition_BB);
    Builder.SetInsertPoint(condition_BB);

    Value *while_condition = m_Condition->codegen();
    if (!while_condition)
        return nullptr;

    auto res = Builder.CreateICmpNE(while_condition, ConstantInt::get(TheContext, APInt(32, 0, true)));

    Builder.CreateCondBr(res, body_BB, after_BB);


    // Body
    Builder.SetInsertPoint(body_BB);
    if (!(m_Body->codegen()))
        return nullptr;
    Builder.CreateBr(condition_BB);



    // After while loop
    Builder.SetInsertPoint(after_BB);

    // while expression always returns 0.
    return Constant::getNullValue(Type::getInt32Ty(TheContext));
}


Value *ASTBreak::codegen() {
    auto parent = Builder.GetInsertBlock()->getParent();
    BasicBlock *after_BB = nullptr;

    for (auto &b : parent->getBasicBlockList()) {
        if (b.getName().startswith("after_block")) {
            after_BB = &b;
            break;
        }
    }
    if (!after_BB)
        return nullptr;

    Builder.CreateBr(after_BB);

    auto break_BB = BasicBlock::Create(TheContext, "after_break", parent);
    Builder.SetInsertPoint(break_BB);

    return Constant::getNullValue(Type::getInt32Ty(TheContext));
}

Value *ASTExit::codegen() {
    Function *parent = Builder.GetInsertBlock()->getParent();

    BasicBlock *function_return_BB = nullptr;

    for (BasicBlock &b : parent->getBasicBlockList()) {
        if (b.getName().startswith("return_block")) {
            function_return_BB = &b;
            break;
        }
    }
    if (!function_return_BB)
        return nullptr;


    Builder.CreateBr(function_return_BB);

    auto exit_BB = BasicBlock::Create(TheContext, "after_exit", parent);
    Builder.SetInsertPoint(exit_BB);

    return Constant::getNullValue(Type::getInt32Ty(TheContext));
}

Value *ASTVariable::codegen() {
    auto type_value = m_Type->codegen();
    if (!type_value)
        return nullptr;

    TheModule->getOrInsertGlobal(m_Name, type_value);
    GlobalVariable *global_var = TheModule->getNamedGlobal(m_Name);
    global_var->setLinkage(GlobalValue::InternalLinkage);

    if (type_value == Type::getInt32Ty(TheContext))
        global_var->setInitializer(ConstantInt::get(TheContext, APInt(32, 0, true)));
    else
        global_var->setInitializer(ConstantAggregateZero::get(type_value));


    global_vars[m_Name] = std::make_pair(global_var, m_Type);

    return global_vars[m_Name].first;
}

// Const declaration
Value *ASTConstVariable::codegen() {
    const_vars[m_Name] = ConstantInt::get(TheContext, APInt(32, m_Value, true));

    return const_vars[m_Name];
}