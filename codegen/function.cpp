//
// Created by Tomas Tran on 04/06/2020.
//

#include "codegen.cpp"
#include "../ast/function/ASTFunctionCall.hpp"
#include "../ast/references/ASTSingleVarReference.hpp"
/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
static AllocaInst * CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, Type * type)
{
    IRBuilder<> TmpB(&TheFunction -> getEntryBlock(), TheFunction -> getEntryBlock().begin());

    auto init_value = ConstantInt::get(Type::getInt32Ty(TheContext), (type->isArrayTy() ? type->getArrayNumElements() : 0), false);
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
        // TODO maybe use m_Name and type from m_Prototype
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
        // Builder.CreateStore(ConstantInt::get(TheContext, APInt(32, 0, true)), alloca);  // TODO not for arrays
        named_values[var->m_Name] = std::make_pair(alloca, var->m_Type);
    }

    // Return variable for functions
    if (m_Prototype->m_ReturnType) {
        AllocaInst *alloca = CreateEntryBlockAlloca(function, m_Prototype->getName(), m_Prototype->m_ReturnType->codegen());
        // Builder.CreateStore(ConstantInt::get(TheContext, APInt(32, 0, true)), alloca);  // TODO not for arrays
        named_values[m_Prototype->getName()] = std::make_pair(alloca, m_Prototype->m_ReturnType);;
    }


    //Builder.SetInsertPoint(function_BB);

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
    //TheFPM -> run(*function);

    // Unrecurse
    named_values = old_named_values;

    return function;
}
