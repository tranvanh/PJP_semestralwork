cmake_minimum_required(VERSION 3.4.3)
project(SimpleFrontend)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Now build our tools
add_executable(mila main.cpp Lexer.hpp Lexer.cpp Parser.hpp Parser.cpp ast/ASTExpression.cpp ast/ASTExpression.hpp ast/constants/ASTString.cpp ast/constants/ASTString.hpp ast/constants/ASTNumber.cpp ast/constants/ASTNumber.hpp ast/data_types/ASTVariableType.cpp ast/data_types/ASTVariableType.hpp ast/data_types/ASTInteger.cpp ast/data_types/ASTInteger.hpp ast/data_types/ASTArray.cpp ast/data_types/ASTArray.hpp ast/ASTBody.cpp ast/ASTBody.hpp ast/variable/ASTVariableDef.cpp ast/variable/ASTVariableDef.hpp ast/variable/ASTVariable.cpp ast/variable/ASTVariable.hpp ast/variable/ASTConstVariable.cpp ast/variable/ASTConstVariable.hpp ast/function/ASTFunctionCall.cpp ast/function/ASTFunctionCall.hpp ast/function/ASTFunctionPrototype.cpp ast/function/ASTFunctionPrototype.hpp ast/function/ASTFunction.cpp ast/function/ASTFunction.hpp ast/statements/ASTIf.cpp ast/statements/ASTIf.hpp ast/statements/ASTFor.cpp ast/statements/ASTFor.hpp ast/statements/ASTWhile.cpp ast/statements/ASTWhile.hpp ast/statements/ASTBreak.cpp ast/statements/ASTBreak.hpp ast/statements/ASTExit.cpp ast/statements/ASTExit.hpp ast/references/ASTReference.cpp ast/references/ASTReference.hpp ast/references/ASTSingleVarReference.cpp ast/references/ASTSingleVarReference.hpp ast/references/ASTArrayReference.cpp ast/references/ASTArrayReference.hpp ast/operators/ASTAssignOperator.cpp ast/operators/ASTAssignOperator.hpp ast/operators/ASTBinaryOperator.cpp ast/operators/ASTBinaryOperator.hpp ast/ASTProgram.cpp ast/ASTProgram.hpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(mila ${llvm_libs})
