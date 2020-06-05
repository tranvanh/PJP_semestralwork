//
// Created by Tomas Tran on 05/06/2020.
//

#ifndef SIMPLEFRONTEND_CODEGEN_HPP
#define SIMPLEFRONTEND_CODEGEN_HPP

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

#include "codegen.hpp"
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



#endif //SIMPLEFRONTEND_CODEGEN_HPP
