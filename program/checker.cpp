#include <stdio.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "cxxopts.hpp"
#include "logger.h"
#include "processor.h"
#include "with_predict.h"

using namespace std;

[[maybe_unused]] Processor *processor = nullptr;
[[maybe_unused]] ProcessorWithPredict *processorWP = nullptr;

int main(int argc, char **argv) {
    cxxopts::Options options("tomasulo-tester", "Tomasulo Simulator Tester");
    auto adder = options.add_options();
    adder("o,output",
          "Output Log file",
          cxxopts::value<std::string>()->default_value("output.log"));
    adder("h,help", "Print Usage");
    adder("f,file", "Input elf file", cxxopts::value<std::string>());
    adder("c,chk-file", "Check file", cxxopts::value<std::string>());
    adder("d,debug", "Print debug infos");
    adder("p,predict", "Use frontend with predictor");
    auto result = options.parse(argc, argv);
    if (result.count("help") != 0 || result.unmatched().size() > 0 ||
        argc == 1) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    if (result.count("debug") != 0) {
        Logger::setInfoOutput(true);
    } else {
        Logger::setInfoOutput(false);
    }

    bool withPredict = false;
    if (result.count("predict") != 0) {
        withPredict = true;
        Logger::Warn("Running branch prediction testcase");
    }

    auto elfFile = result["file"].as<std::string>();

    std::vector<unsigned> inst, data;
    unsigned entry = readElf(elfFile, inst, data);

    if (!withPredict)
        processor = new Processor(inst, data, entry);
    else {
        processor = new Processor(inst, data, entry);
        processorWP = new ProcessorWithPredict(inst, data, entry);
    }

    unsigned counter = 0;

    bool finish = false;
    do {
        if (!withPredict)
            finish = processor->step();
        else
            finish = processorWP->step();
        counter++;
        if (counter % 50000 == 0) {
            Logger::Warn("Running %u cycles.", counter);
        }
    } while (!finish);

    Logger::Warn("Finished in %u cycles.", counter);

    if (withPredict) {
        Logger::Warn("Running normal testcase");
        finish = false;
        unsigned counterWithoutPredict = 0;
        do {
            finish = processor->step();
            counterWithoutPredict++;
            if (counterWithoutPredict % 50000 == 0) {
                Logger::Warn("Running %u cycles.", counterWithoutPredict);
            }
        } while (!finish);

        Logger::Warn("Finished in %u cycles.", counterWithoutPredict);
        Logger::Warn(
            "Without prediction: %u cycles. With "
            "prediction: %u cycles.",
            counterWithoutPredict,
            counter);
        if (counterWithoutPredict <= counter) {
            fprintf(stderr, "[ FAILED  ] Branch prediction failed.\n");
            return -1;
        }
        fprintf(stderr,
                "[   OK    ] Branch prediction running time check passed\n");
    }

    auto chkFile = result["chk-file"].as<std::string>();
    std::ifstream chkIn(chkFile);

    int cases;
    chkIn >> cases;
    std::string tmp;
    std::getline(chkIn, tmp);
    int passedCases = 0;
    for (int i = 1; i <= cases; i++) {
        std::string s;
        std::getline(chkIn, s);
        std::stringstream ss;
        ss << s;
        ss >> s;
        if (s == "RAM") {
            uint32_t addr;
            ss >> std::hex >> addr >> std::dec;
            if (addr & 0x3) {
                fprintf(stderr,
                        "[ FAILED  ] On testcase %d, unaligned address %08x "
                        "for WORD.\n",
                        i,
                        addr);
                continue;
            }
            uint32_t answer;
            ss >> answer;
            auto result = withPredict ? processorWP->readMem(addr)
                                      : processor->readMem(addr);
            if (result != answer) {
                fprintf(stderr,
                        "[ FAILED  ] On testcase %d, answer is %d, but %d "
                        "is found in DRAM\n",
                        i,
                        answer,
                        result);
                continue;
            }
        } else if (s == "REG") {
            uint32_t addr;
            ss >> addr;
            uint32_t answer;
            ss >> answer;
            auto result = withPredict ? processorWP->readReg(addr)
                                      : processor->readReg(addr);
            if (result != answer) {
                fprintf(stderr,
                        "[ FAILED  ] On testcase %d, answer is %d, but %d "
                        "is found in Register\n",
                        i,
                        answer,
                        result);
                continue;
            }
        }
        passedCases++;
    }

    if (passedCases == cases) {
        fprintf(stderr, "[   OK    ] %d testcase(s) passed\n", cases);
        return 0;
    } else {
        fprintf(stderr,
                "[ FAILED  ] %d of %d testcase(s) passed\n",
                passedCases,
                cases);
        return -1;
    }
}