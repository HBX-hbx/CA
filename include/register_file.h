#pragma once

class RegisterFile {
    unsigned reg[32];
    bool busy[32] = {false};
    unsigned robIndices[32] = {0u};

public:
    RegisterFile();
    unsigned read(unsigned addr) const;
    void write(unsigned addr, unsigned val, unsigned robIdx);
    bool isBusy(unsigned addr) const;
    void markBusy(unsigned addr, unsigned robIdx);
    unsigned getBusyIndex(unsigned addr) const;
    void flush();
};