#include <sstream>

#include "with_predict.h"

ProcessorWithPredict::ProcessorWithPredict(const std::vector<unsigned> &inst,
                                           const std::vector<unsigned> &data,
                                           unsigned entry)
    : frontend(inst),
      backend(data),
      regFile() {
    frontend.jump(entry);
    backend.attachRegFile(std::shared_ptr<RegisterFile>(&regFile));
}

/**
 * @brief Processor 步进函数
 *
 * @return true 提交了EXTRA::EXIT
 * @return false 其他情况
 */
bool ProcessorWithPredict::step() {
    bool finish = backend.step(frontend);
    auto newInst = frontend.step();
    if (newInst.has_value()) {
        if (!backend.dispatchInstruction(newInst.value()))
            frontend.haltDispatch();
        else {
            std::stringstream ss;
            ss << newInst.value();
            Logger::Info("Dispatching %s with pc = %08x\n",
                         ss.str().c_str(),
                         newInst.value().pc);
        }
    }
    return finish;
}

/**
 * @brief 用于读取数据内存中的内容
 *
 * @param addr
 * @return unsigned
 */
unsigned ProcessorWithPredict::readMem(unsigned addr) const {
    return backend.read(addr);
}

/**
 * @brief 用于读取寄存器当中的内容
 *
 * @param addr
 * @return unsigned
 */
unsigned ProcessorWithPredict::readReg(unsigned addr) const {
    return regFile.read(addr);
}
