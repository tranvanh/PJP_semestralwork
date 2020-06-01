//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTARRAYREFERENCE_HPP
#define SIMPLEFRONTEND_ASTARRAYREFERENCE_HPP


#include "ASTReference.hpp"

class ASTArrayReference : public ASTReference {
public:
    ASTArrayReference(const std::string &name,
                      std::unique_ptr<ASTExpression> index
    );

    llvm::Value *codegen() override;

    llvm::Value *getAlloc() override;

    const std::unique_ptr<ASTExpression> m_Index;
};


#endif //SIMPLEFRONTEND_ASTARRAYREFERENCE_HPP
