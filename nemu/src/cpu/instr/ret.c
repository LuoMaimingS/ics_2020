#include "cpu/instr.h"

make_instr_func(ret_near)
{
    print_asm_0("ret", "", 1);
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.sreg = SREG_DS;
    mem.data_size = data_size;
    mem.addr = cpu.esp;
    operand_read(&mem);
    cpu.eip = mem.val;

    cpu.esp = cpu.esp + data_size / 8;
    
    return 0;
}

make_instr_func(ret_near_imm_w)
{
    print_asm_0("ret", "", 3);
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.sreg = SREG_DS;
    mem.data_size = data_size;
    mem.addr = cpu.esp;
    operand_read(&mem);
    cpu.eip = mem.val;
    
    int offset = sign_ext(instr_fetch(eip + 1, 2), 16);
    cpu.esp = cpu.esp + data_size / 8 + offset;
    
    return 0;
}

make_instr_func(iret)
{
    print_asm_0("iret", "", 1);
    
    cpu.eip = vaddr_read(cpu.esp, SREG_SS, 4);    
    cpu.esp += 4;
    
    cpu.cs.val = vaddr_read(cpu.esp, SREG_SS, 2);
    cpu.esp += 2;

    cpu.eflags.val = vaddr_read(cpu.esp, SREG_SS, 4);
    cpu.esp += 4;

    return 0;
}



