#include "cpu/instr.h"


make_instr_func(lgdt)
{
    int len = 1;
    OPERAND rel;
    
    // printf("%x %x %x %x %x %x %x %x.\n", instr_fetch(eip, 1), instr_fetch(eip+1, 1),instr_fetch(eip+2, 1), instr_fetch(eip+3, 1), instr_fetch(eip+4, 1),instr_fetch(eip+5, 1), instr_fetch(eip+6, 1), instr_fetch(eip+7, 1));
    // .word
    len += modrm_rm(eip + 1, &rel);
    rel.data_size = 16;
    operand_read(&rel);
    cpu.gdtr.limit = rel.val;
    // printf("limit addr: %x.\n", rel.addr);
    
    // .long
    rel.addr += 2;
    rel.data_size = 32;
    operand_read(&rel);
    cpu.gdtr.base = rel.val;
    // printf("base addr: %x.\n", rel.addr);

    // printf("GDT Loaded. Base: %x, limit: %x.\n", cpu.gdtr.base, cpu.gdtr.limit);
    print_asm_1("lgdt", "", len + 1, &rel);
    
    return len;
}

