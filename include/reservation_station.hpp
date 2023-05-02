#pragma once
#include <memory>
#include <sstream>

#include "instructions.h"
#include "issue_slot.h"
#include "logger.h"
#include "register_file.h"
#include "rob.h"

template <unsigned size>
class ReservationStation {
    IssueSlot buffer[size];

public:
    ReservationStation();
    bool hasEmptySlot() const;
    void insertInstruction(const Instruction &inst,
                           unsigned robIdx,
                           std::shared_ptr<RegisterFile> regFile,
                           const ReorderBuffer &reorderBuffer);
    void wakeup(const ROBStatusWritePort &x);
    bool canIssue() const;
    IssueSlot issue();
    void flush();
};

template <unsigned size>
ReservationStation<size>::ReservationStation() {
    for (auto &slot : buffer) {
        slot.busy = false;
    }
}

template <unsigned size>
bool ReservationStation<size>::hasEmptySlot() const {
    for (auto const &slot : buffer) {
        if (!slot.busy) return true;
    }
    return false;
}

template <unsigned size>
void ReservationStation<size>::insertInstruction(
    const Instruction &inst,
    unsigned robIdx,
    std::shared_ptr<RegisterFile> regFile,
    const ReorderBuffer &reorderBuffer) {
    for (auto &slot : buffer) {
        if (slot.busy) continue;
        // TODO: Dispatch instruction to this slot
        // 1. 若有 slot 空闲，执行插入
        // 2. 插入时，设置两个寄存器读取端口是否已经唤醒，以及对应值，必要时从 ROB 中读取。
        // 3. 寄存器设置 busy
        // TODO: 寄存器 busy?
        // 4. slot busy，返回
        slot.inst = inst;
        slot.robIdx = robIdx;
        unsigned rs1 = inst.getRs1(), rs2 = inst.getRs2();

        if (regFile->isBusy(rs1)) {
            unsigned busyRobIdx = regFile->getBusyIndex(rs1);
            // 寄存器未就绪
            if (reorderBuffer.checkReady(busyRobIdx)) {
                // 值就绪
                slot.readPort1.waitForWakeup = false;
                slot.readPort1.value = reorderBuffer.read(busyRobIdx);
            } else {
                // 值未就绪
                slot.readPort1.waitForWakeup = true;
                slot.readPort1.robIdx = busyRobIdx;
            }
        } else {
            // 值就绪
            slot.readPort1.waitForWakeup = false;
            slot.readPort1.value = regFile->read(rs1);
        }

        if (regFile->isBusy(rs2)) {
            unsigned busyRobIdx = regFile->getBusyIndex(rs2);
            // 寄存器未就绪
            if (reorderBuffer.checkReady(busyRobIdx)) {
                // 值就绪
                slot.readPort2.waitForWakeup = false;
                slot.readPort2.value = reorderBuffer.read(busyRobIdx);
            } else {
                // 值未就绪
                slot.readPort2.waitForWakeup = true;
                slot.readPort2.robIdx = busyRobIdx;
            }
        } else {
            // 值就绪
            slot.readPort2.waitForWakeup = false;
            slot.readPort2.value = regFile->read(rs2);
        }
        slot.busy = true;
        return;
    }
    Logger::Error("ReservationStation::insertInstruction no empty slots!");
    std::__throw_runtime_error(
        "No empty slots when inserting instruction into reservation "
        "station.");
}

template <unsigned size>
void ReservationStation<size>::wakeup(
    [[maybe_unused]] const ROBStatusWritePort &x) {
    // TODO: Wakeup instructions according to ROB Write
    for (IssueSlot &slot: buffer) {
        // rs1
        if (slot.readPort1.waitForWakeup) {
            if (slot.readPort1.robIdx == x.robIdx) {
                slot.readPort1.waitForWakeup = false;
                slot.readPort1.value = x.result;
            }
        }
        // rs2
        if (slot.readPort2.waitForWakeup) {
            if (slot.readPort2.robIdx == x.robIdx) {
                slot.readPort2.waitForWakeup = false;
                slot.readPort2.value = x.result;
            }
        }
    }
}

template <unsigned size>
bool ReservationStation<size>::canIssue() const {
    // TODO: Decide whether an issueSlot is ready to issue.
    for (const IssueSlot &slot: buffer) {
        if (!slot.busy) continue;
        if (!slot.readPort1.waitForWakeup && !slot.readPort2.waitForWakeup) {
            return true;
        }
    }
    return false;
}

template <unsigned size>
IssueSlot ReservationStation<size>::issue() {
    // TODO: Issue a ready issue slot and remove it from reservation station.
    for (IssueSlot &slot: buffer) {
        if (!slot.busy) continue;
        if (!slot.readPort1.waitForWakeup && !slot.readPort2.waitForWakeup) {
            IssueSlot result_slot;
            result_slot.readPort1 = slot.readPort1;
            result_slot.readPort2 = slot.readPort2;
            result_slot.inst      = slot.inst;
            result_slot.robIdx    = slot.robIdx;
            result_slot.busy      = true;
            slot.busy             = false;
            return result_slot;
        }
    }
    Logger::Error("No available slots for issuing");
    std::__throw_runtime_error("No available slots for issuing");
}

template <unsigned size>
void ReservationStation<size>::flush() {
    for (auto &slot : buffer) {
        slot.busy = false;
    }
}