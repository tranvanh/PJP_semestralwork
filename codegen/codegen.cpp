//
// Created by Tomas Tran on 04/06/2020.
//
#include "codegen.hpp"

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
//    auto file_type = CGFT_ObjectFile;
    auto file_type = TargetMachine::CGFT_ObjectFile;

    if (TheTargetMachine -> addPassesToEmitFile(pass, dest, file_type)) {
//    if (TheTargetMachine->addPassesToEmitFile(pass, (raw_pwrite_stream &) outs(),
//                                              (raw_pwrite_stream * )(&outs()), file_type)) {
        errs() << "TheTargetMachine can't emit a file of this type";
        return nullptr;
    }

    assert(!verifyModule(*TheModule, &errs()));

    pass.run(*TheModule);

    dest.flush();

    outs() << "Wrote " << output_file << "\n";

    return nullptr;
}
