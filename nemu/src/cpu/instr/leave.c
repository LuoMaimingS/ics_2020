#include "cpu/instr.h"

make_instr_func(leave)
{
    cpu.esp = cpu.ebp;    
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.sreg = SREG_DS;
    mem.data_size = data_size;
    mem.addr = cpu.esp;
    operand_read(&mem);
    cpu.ebp = mem.val;    
    cpu.esp += data_size / 8;
    
    return 1;
}

