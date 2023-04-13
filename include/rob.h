#pragma once
#include <optional>

#include "instructions.h"

struct ROBStatusBundle {
    // ALU section
    unsigned result;
    // BRU section
    bool mispredict, actualTaken;
    unsigned jumpTarget;
    bool ready;
};

struct ROBStatusWritePort {
    unsigned result;
    bool mispredict, actualTaken;
    unsigned jumpTarget;
    unsigned robIdx;
};

struct ROBEntry {
    Instruction inst;
    ROBStatusBundle state;
    bool valid;
};

class ReorderBuffer {
    ROBEntry buffer[ROB_SIZE];
    unsigned pushPtr, popPtr; // [popPtr, pushPtr)

public:
    ReorderBuffer();
    bool canPush() const;
    bool canPop() const;
    unsigned push(const Instruction &x, bool ready);
    void pop();
    void flush();
    std::optional<ROBEntry> getFront() const;
    void writeState(const ROBStatusWritePort &x);
    unsigned getPopPtr() const;
    unsigned read(unsigned addr) const;
    bool checkReady(unsigned addr) const;
};
