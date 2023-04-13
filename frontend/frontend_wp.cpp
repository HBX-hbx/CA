#include "with_predict.h"

FrontendWithPredict::FrontendWithPredict(const std::vector<unsigned> &inst)
    : Frontend(inst) {
    // Optional TODO: initialize your prediction structures here.
}

/**
 * @brief 获取指令的分支预测结果，分支预测时需要
 * 
 * @param pc 指令的pc
 * @return BranchPredictBundle 分支预测的结构
 */
BranchPredictBundle FrontendWithPredict::bpuFrontendUpdate(unsigned int pc) {
    // Optional TODO: branch predictions
    return Frontend::bpuFrontendUpdate(pc);
}

/**
 * @brief 用于计算NextPC，分支预测时需要
 * 
 * @param pc 
 * @return unsigned 
 */
unsigned FrontendWithPredict::calculateNextPC(unsigned pc) const {
    // Optional TODO: branch predictions
    return Frontend::calculateNextPC(pc);
}

/**
 * @brief 用于后端提交指令时更新分支预测器状态，分支预测时需要
 * 
 * @param x 
 */
void FrontendWithPredict::bpuBackendUpdate(const BpuUpdateData &x) {
    // Optional TODO: branch predictions
    Frontend::bpuBackendUpdate(x);
}
