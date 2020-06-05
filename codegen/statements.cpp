//
// Created by Tomas Tran on 04/06/2020.
//

#include "codegen.hpp"

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


/*	PHINode * phi_node = Builder.CreatePHI(Type::getInt32Ty(TheContext), 2, "iftmp");

	phi_node -> addIncoming(then_value, then_BB);
	phi_node -> addIncoming(else_value, else_BB);
	return phi_node; */
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

