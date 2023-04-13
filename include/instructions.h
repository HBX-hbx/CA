#pragma once

#include <utility>

#include "masked_literal.h"
#include "branch_predict_model.h"

struct ExecuteResultBundle {
    bool mispredict;
    bool actualTaken;
    unsigned result;
    unsigned jumpTarget;
};

struct Instruction {
    unsigned instruction;
    InstructionType type;
    unsigned pc;
    BranchPredictBundle predictBundle;

    unsigned getImmediate() const;
    unsigned getRd() const;
    unsigned getRs1() const;
    unsigned getRs2() const;
    unsigned get(int index) const;
    unsigned get(int index, int length) const;

    Instruction(unsigned inst = 0x13);
    bool operator==(const MaskedLiteral &rhs) const;
    bool operator!=(const MaskedLiteral &rhs) const;
    friend std::ostream &operator<<(std::ostream &os, const Instruction &inst);

    ExecuteResultBundle execute(const std::string &name,
                                unsigned operand1,
                                unsigned operand2) const;

    static Instruction NOP();
};

FUType getFUType(const Instruction &inst);
