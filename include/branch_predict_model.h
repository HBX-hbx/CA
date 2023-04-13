// Optional TODO: Change this file to whatever you like!
// NOTE: Please keep these signals that are already in used.

struct BpuUpdateData {
    unsigned pc;
    bool isCall, isReturn, isBranch, branchTaken;
    unsigned jumpTarget;
};

struct BranchPredictBundle {
    bool predictJump = false;
    unsigned predictTarget = 0;
};
