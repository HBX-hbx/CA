#pragma once

#include <optional>

#include "defines.h"

struct StoreBufferSlot {
    unsigned storeAddress;
    unsigned storeData;
    bool valid;
};

class StoreBuffer {
    StoreBufferSlot buffer[ROB_SIZE];
    unsigned pushPtr, popPtr;

public:
    StoreBuffer();
    void push(unsigned addr, unsigned value);
    StoreBufferSlot pop();
    void flush();
    std::optional<unsigned> query(unsigned addr);
};
