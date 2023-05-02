#include "processor.h"

// Optional TODO: Change here to implement predictions

struct BTBEntry {
    unsigned pc;
    unsigned bht;  // 取值 0, 1, 2, 3
    unsigned target;
    bool valid;
};

class FrontendWithPredict : public Frontend {
    BTBEntry btb_table[1024];

protected:
    BranchPredictBundle bpuFrontendUpdate(unsigned int pc) override;

public:
    FrontendWithPredict(const std::vector<unsigned> &inst);
    unsigned calculateNextPC(unsigned pc) const override;
    void bpuBackendUpdate(const BpuUpdateData &x) override;
};

class ProcessorWithPredict {
    FrontendWithPredict frontend;
    Backend backend;
    RegisterFile regFile;

public:
    ProcessorWithPredict(const std::vector<unsigned> &inst,
                         const std::vector<unsigned> &data,
                         unsigned entry);
    bool step();
    unsigned readMem(unsigned addr) const;
    unsigned readReg(unsigned addr) const;
};
