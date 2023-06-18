#include "with_predict.h"

FrontendWithPredict::FrontendWithPredict(const std::vector<unsigned> &inst)
    : Frontend(inst) {
    // Optional TODO: initialize your prediction structures here.
    for (int i = 0; i < 1024; ++i) {
        btb_table[i].valid = false;
    }
}

/**
 * @brief 获取指令的分支预测结果，分支预测时需要
 * 
 * @param pc 指令的pc
 * @return BranchPredictBundle 分支预测的结构
 */
BranchPredictBundle FrontendWithPredict::bpuFrontendUpdate(unsigned int pc) {
    // Optional TODO: branch predictions
    BranchPredictBundle result;
    unsigned i = (pc >> 2) % 1024;
    if (btb_table[i].valid && btb_table[i].pc == pc) {
        if (btb_table[i].bht == 0 || btb_table[i].bht == 1) {
            // 预测分支不成功
            result.predictJump = false;
        } else { // 2 or 3
            result.predictJump = true;
            result.predictTarget = btb_table[i].target;
        }
    } else {
        result.predictJump = false;
    }
    return result;
}

/**
 * @brief 用于计算NextPC，分支预测时需要
 * 
 * @param pc 
 * @return unsigned 
 */
unsigned FrontendWithPredict::calculateNextPC(unsigned pc) const {
    // Optional TODO: branch predictions
    unsigned i = (pc >> 2) % 1024;
    unsigned next_pc = 0;
    if (btb_table[i].valid && btb_table[i].pc == pc) {
        if (btb_table[i].bht == 0 || btb_table[i].bht == 1) {
            // 预测分支不成功
            next_pc = pc + 4;
        } else { // 2 or 3
            next_pc = btb_table[i].target;
        }
    } else {
        next_pc = pc + 4;
    }
    return next_pc;
}

/**
 * @brief 用于后端提交指令时更新分支预测器状态，分支预测时需要
 * 
 * @param x 
 */
void FrontendWithPredict::bpuBackendUpdate(const BpuUpdateData &x) {
    // Optional TODO: branch predictions
    unsigned i = (x.pc >> 2) % 1024;
    if (btb_table[i].valid && btb_table[i].pc == x.pc) {
        btb_table[i].target = x.jumpTarget;
        if (x.branchTaken) { // 分支成功
            if (btb_table[i].bht == 0) { // 00 -> 01
                btb_table[i].bht = 1;
            } else { // 01 10 11 -> 11
                btb_table[i].bht = 3;
            }
        } else { // 分支失败
            if (btb_table[i].bht == 3) { // 11 -> 10
                btb_table[i].bht = 2;
            } else { // 00 01 10 -> 00
                btb_table[i].bht = 0;
            }
        }
    } else {
        btb_table[i].pc = x.pc;
        btb_table[i].valid = true;
        btb_table[i].target = x.jumpTarget;
        if (x.branchTaken) {
            btb_table[i].bht = 1;
        } else {
            btb_table[i].bht = 0;
        }
    }
}
