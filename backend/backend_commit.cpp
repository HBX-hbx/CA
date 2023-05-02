#include <optional>
#include "defines.h"
#include "instructions.h"
#include "logger.h"
#include "processor.h"

/**
 * @brief 处理前端流出的指令
 *
 * @param inst 前端将要流出的指令（在流出buffer里面）
 * @return true 后端接受该指令
 * @return false 后端拒绝该指令
 */
bool Backend::dispatchInstruction([[maybe_unused]] const Instruction &inst) {
    if (!rob.canPush()) return false;

    // TODO: Check rob and reservation station is available for push.
    // NOTE: use getFUType to get instruction's target FU
    // NOTE: FUType::NONE only goes into ROB but not Reservation Stations
    // 1. 检查 ROB 是否已满
    // 2. 检查对应的保留站是否已满
    // 3. 插入 ROB
    // 4. 插入保留站
    // 5. 更新寄存器占用情况
    // 6. 插入成功返回 true, 失败返回 false, 请保证返回 false 时没有副作用
    FUType fuType = getFUType(inst);
    switch (fuType) {
        case FUType::ALU : {
            // 检查 ALU 保留站是否已满
            if(!rsALU.hasEmptySlot()) return false;
            // 插入 ROB，获取 robIdx
            unsigned robIdx = rob.push(inst, false);
            // 插入保留站
            rsALU.insertInstruction(inst, robIdx, regFile, rob);
            // 更新 rd 寄存器占用情况
            regFile->markBusy(inst.getRd(), robIdx);
            break;
        }
        case FUType::BRU : {
            // 检查 BRU 保留站是否已满
            if(!rsBRU.hasEmptySlot()) return false;
            // 插入 ROB，获取 robIdx
            unsigned robIdx = rob.push(inst, false);
            // 插入保留站
            rsBRU.insertInstruction(inst, robIdx, regFile, rob);
            // 更新 rd 寄存器占用情况
            regFile->markBusy(inst.getRd(), robIdx);
            break;
        }
        case FUType::DIV : {
            // 检查 DIV 保留站是否已满
            if(!rsDIV.hasEmptySlot()) return false;
            // 插入 ROB，获取 robIdx
            unsigned robIdx = rob.push(inst, false);
            // 插入保留站
            rsDIV.insertInstruction(inst, robIdx, regFile, rob);
            // 更新 rd 寄存器占用情况
            regFile->markBusy(inst.getRd(), robIdx);
            break;
        }
        case FUType::LSU : {
            // 检查 LSU 保留站是否已满
            if(!rsLSU.hasEmptySlot()) return false;
            // 插入 ROB，获取 robIdx
            unsigned robIdx = rob.push(inst, false);
            // 插入保留站
            rsLSU.insertInstruction(inst, robIdx, regFile, rob);
            // 更新 rd 寄存器占用情况
            regFile->markBusy(inst.getRd(), robIdx);
            break;
        }
        case FUType::MUL : {
            // 检查 MUL 保留站是否已满
            if(!rsMUL.hasEmptySlot()) return false;
            // 插入 ROB，获取 robIdx
            unsigned robIdx = rob.push(inst, false);
            // 插入保留站
            rsMUL.insertInstruction(inst, robIdx, regFile, rob);
            // 更新 rd 寄存器占用情况
            regFile->markBusy(inst.getRd(), robIdx);
            break;
        }
        case FUType::NONE : {
            // 插入 ROB，不插入保留站
            rob.push(inst, true);
            break;
        }
        default: {
            Logger::Error("Not supported inst.type: %d", inst.type);
            std::__throw_runtime_error("Instruction dispatch not implemented!");
        }
    }
    return true;
}

/**
 * @brief 后端完成指令提交
 *
 * @param entry 被提交的 ROBEntry
 * @param frontend 前端，用于调用前端接口
 * @return true 提交了 EXTRA::EXIT
 * @return false 其他情况
 */
bool Backend::commitInstruction([[maybe_unused]] const ROBEntry &entry,
                                [[maybe_unused]] Frontend &frontend) {
    // TODO: Commit instructions here. Set executeExit when committing
    // EXTRA::EXIT NOTE: Be careful about Store Buffer! NOTE: Be careful about
    // flush!
    // 1. 完成指令提交
    // 2. (Optional TODO) 更新 bpu
    // 3. ROB 弹出
    // 4. 若 mispredict，跳转，清空
    // 5. 若提交 Exit，返回 true，否则返回 false
    unsigned robIdx = rob.getPopPtr();
    rob.pop();
    FUType fuType = getFUType(entry.inst);
    regFile->write(entry.inst.getRd(), entry.state.result, robIdx);
    switch (fuType) {
        case FUType::MUL :
        case FUType::DIV :
        case FUType::ALU : {
            break;
        }
        case FUType::BRU : {
            // mispredict: 实际上分支但预测不分支，或者实际上不分支但预测分支
            // jumpTarget: 分支跳转的地址
            if (entry.state.mispredict) {
                if (entry.state.actualTaken) {
                    // 预测不分支，但是实际上分支了，进行跳转
                    frontend.jump(entry.state.jumpTarget);
                } else {
                    // 预测分支，但是实际上不分支，进行跳转
                    frontend.jump(entry.inst.pc + 4);
                }
                flush();

            }
            if (entry.inst == RV32I::BEQ || entry.inst == RV32I::BNE ||
                entry.inst == RV32I::BLT || entry.inst == RV32I::BGE || 
                entry.inst == RV32I::BLTU || entry.inst == RV32I::BGEU
            ) {
                BpuUpdateData x;
                x.branchTaken = entry.state.actualTaken; // 实际上分支成功
                x.jumpTarget = entry.state.jumpTarget;
                x.pc = entry.inst.pc;
                frontend.bpuBackendUpdate(x);
            }
            break;
        }
        case FUType::LSU : {
            if (entry.inst == RV32I::SB || entry.inst == RV32I::SH || entry.inst == RV32I::SW) {
                StoreBufferSlot slot = storeBuffer.pop();
                data[(slot.storeAddress - 0x80400000) >> 2] = slot.storeData;
            }
            break;
        }
        case FUType::NONE : {
            // 提交 Exit，返回 true
            return true;
            break;
        }
        default: {
            Logger::Error("Not supported inst.type: %d", entry.inst.type);
            std::__throw_runtime_error("Instruction dispatch not implemented!");
        }
    }

    return false;
}