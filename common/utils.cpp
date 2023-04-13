#include <string.h>

#include <string>
#include <vector>

#include "elf.h"
#include "logger.h"

std::string xreg_name[32] = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
std::string freg_name[32] = {
    "f0",  "f1",  "f2",  "f3",  "f4",  "f5",  "f6",  "f7",  "f8",  "f9",  "f10",
    "f11", "f12", "f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21",
    "f22", "f23", "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31"};

/**
 * @brief
 * 读取ELF中的内容，分离为0x80000000开始的指令区，和0x80400000开始的数据区
 *
 * @param elfFileName
 * @param instruction
 * @param data
 * @return 程序入口地址
 */
unsigned readElf(const std::string &elfFileName,
                 std::vector<unsigned> &instruction,
                 std::vector<unsigned> &data) {
    fprintf(stderr, "[  INFO   ] Reading ELF file %s\n", elfFileName.c_str());

    FILE *elfFile = fopen(elfFileName.c_str(), "rb");
    elf32_ehdr ehdr;
    [[maybe_unused]] size_t __result =
        fread(&ehdr, 1, sizeof(elf32_ehdr), elfFile);

    if (ehdr.e_ident[0] != ELF_MAGIC) {
        std::__throw_invalid_argument(
            "No valid elf magic found, please check input file's "
            "type.");
    }
    if (ehdr.e_machine != EM_RISCV) {
        std::__throw_invalid_argument(
            "Elf is not for RISCV, please check input file and "
            "compiler.");
    }
    fseek(elfFile, ehdr.e_shoff, SEEK_SET);
    elf32_shdr *sectionHeaders = new elf32_shdr[ehdr.e_shnum];
    __result = fread(sectionHeaders, sizeof(elf32_shdr), ehdr.e_shnum, elfFile);

    fseek(elfFile, sectionHeaders[ehdr.e_shstrndx].sh_offset, SEEK_SET);
    char *stringTable = new char[sectionHeaders[ehdr.e_shstrndx].sh_size];
    __result =
        fread(stringTable, 1, sectionHeaders[ehdr.e_shstrndx].sh_size, elfFile);

    instruction.clear();
    data.clear();

    instruction.resize(0x400000 / 4, 0u);
    data.resize(0x400000 / 4, 0u);

    unsigned instMax = 0u, dataMax = 0u;

    for (int i = 0; i < ehdr.e_shnum; i++) {
        Logger::Info("Segment [%2d]:\t", i);
        Logger::Info("%-24s\t", &stringTable[sectionHeaders[i].sh_name]);
        Logger::Info("ADDR: 0x%08x\t", sectionHeaders[i].sh_addr);
        Logger::Info("OFFSET: %u\t", sectionHeaders[i].sh_offset);
        Logger::Info("SIZE: %u\n", sectionHeaders[i].sh_size);

        unsigned addr = sectionHeaders[i].sh_addr;

        if (addr >= 0x80000000u && addr < 0x80400000u) {
            uint8_t *sectionData = new uint8_t[sectionHeaders[i].sh_size];
            fseek(elfFile, sectionHeaders[i].sh_offset, SEEK_SET);
            fread(sectionData, 1, sectionHeaders[i].sh_size, elfFile);
            uint32_t len = sectionHeaders[i].sh_size;
            uint32_t *p = (uint32_t *) sectionData;
            for (uint32_t j = addr; j < addr + len; j += 4, p++) {
                instruction[(j - 0x80000000u) >> 2] = (*p);
                instMax = std::max(instMax, (j - 0x80000000u) >> 2);
            }
            delete[] sectionData;
        } else if (addr >= 0x80400000u && addr < 0x80800000u) {
            uint8_t *sectionData = new uint8_t[sectionHeaders[i].sh_size];
            fseek(elfFile, sectionHeaders[i].sh_offset, SEEK_SET);
            fread(sectionData, 1, sectionHeaders[i].sh_size, elfFile);
            uint32_t len = sectionHeaders[i].sh_size;
            uint32_t *p = (uint32_t *) sectionData;
            bool bssFlag = false;
            if (strcmp(&stringTable[sectionHeaders[i].sh_name], ".bss") == 0)
                bssFlag = true;
            for (uint32_t j = addr; j < addr + len; j += 4, p++) {
                data[(j - 0x80400000u) >> 2] = (bssFlag ? 0 : (*p));
                dataMax = std::max(dataMax, (j - 0x80400000u) >> 2);
            }
            delete[] sectionData;
        }
    }
    Logger::Info("instMax = %d, dataMax = %d", instMax, dataMax);
    instruction.resize(instMax + 1);
    data.resize(dataMax + 1);

    return ehdr.e_entry;
}
