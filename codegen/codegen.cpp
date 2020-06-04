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
#include "../ast/data_types/ASTVariableType.hpp"
#include "../ast/ASTProgram.hpp"

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

/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
static AllocaInst * CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, Type * type)
{
    IRBuilder<> TmpB(&TheFunction -> getEntryBlock(), TheFunction -> getEntryBlock().begin());

    auto init_value = ConstantInt::get(Type::getInt32Ty(TheContext), (type->isArrayTy() ? type->getArrayNumElements() : 0), false);
    return TmpB.CreateAlloca(type, init_value, VarName.c_str());
}

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
    //	TheFPM = llvm::make_unique<legacy::FunctionPassManager>(TheModule.get());
    /*
    // Promote allocas to registers.
    TheFPM->add(createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    TheFPM->add(createInstructionCombiningPass());
    // Reassociate expressions.
    TheFPM->add(createReassociatePass());
    // Eliminate Common SubExpressions.
    TheFPM->add(createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    TheFPM->add(createCFGSimplificationPass());

    TheFPM->doInitialization();*/

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
//    auto file_type = TargetMachine::CGFT_ObjectFile;
    auto file_type = CGFT_ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, (raw_pwrite_stream &) outs(),
                                              (raw_pwrite_stream *) (&outs()), file_type)) {
        errs() << "TheTargetMachine can't emit a file of this type";
        return nullptr;
    }

    assert(!verifyModule(*TheModule, &errs()));

    pass.run(*TheModule);

    dest.flush();

    outs() << "Wrote " << output_file << "\n";

    return nullptr;
}
