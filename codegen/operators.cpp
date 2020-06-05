//
// Created by Tomas Tran on 04/06/2020.
//

#include "codegen.hpp"

Value * ASTAssignOperator::codegen ()
{
    // Find alloca address of left side
    Value * alloca = m_Variable -> getAlloc();
    if ( !alloca )
        return nullptr;

    Value * new_value = m_Value -> codegen();
    if ( !new_value )
        return nullptr;

    Builder.CreateStore(new_value, alloca);

    return new_value;
}


Value * ASTBinaryOperator::codegen ()
{
    Value * left = m_LHS -> codegen();
    Value * right = m_RHS -> codegen();

    if ( !left || !right )
        return nullptr;

    Value * bit_result;
    switch ( m_Op ) {
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

