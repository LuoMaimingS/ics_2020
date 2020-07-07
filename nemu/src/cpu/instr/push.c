#include "cpu/instr.h"

static void instr_execute_1op()
{
    operand_read(&opr_src);
	cpu.esp -= data_size / 8;
	opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_DS;
	opr_dest.addr = cpu.esp;
	opr_dest.val = opr_src.val;
	operand_write(&opr_dest);
}

make_instr_impl_1op(push, r, v)
make_instr_impl_1op(push, rm, v)
make_instr_impl_1op(push, i, v)

make_instr_func(push_es) {
    OPERAND rm;
    rm.data_size = data_size;
    rm.type = OPR_MEM;
    rm.sreg = SREG_DS;

    cpu.esp -= data_size / 8;
    rm.addr = cpu.esp;
    rm.val = cpu.esp;
    operand_write(&rm);
    
    return 1;
}

make_instr_func(push_i_b) {
	int len = 1;
	opr_src.data_size = 8;
	opr_src.type = OPR_IMM; 
	opr_src.addr = eip + 1;
    len += 1; 
    cpu.esp -= data_size / 8;
	operand_read(&opr_src);
    print_asm_1("push", "b", len, &opr_src);

	opr_src.val = sign_ext(opr_src.val, 8);
	opr_dest.data_size = data_size;
	opr_dest.type = OPR_MEM;
	opr_dest.addr = cpu.esp;
	opr_dest.val = opr_src.val;
	operand_write(&opr_dest);
	return len;
}

make_instr_func(pusha) {
	print_asm_0("pusha", "", 1);

    uint32_t cur_esp = cpu.esp;    

	opr_dest.data_size = 32;
	opr_dest.type = OPR_MEM;
	opr_dest.sreg = SREG_SS;

	cpu.esp -= 4;
	opr_dest.addr = cpu.esp;
	opr_dest.val = cpu.eax;
	operand_write(&opr_dest);

	cpu.esp -= 4;
	opr_dest.addr = cpu.esp;
	opr_dest.val = cpu.ecx;
	operand_write(&opr_dest);

    cpu.esp -= 4;
	opr_dest.addr = cpu.esp;
	opr_dest.val = cpu.edx;
	operand_write(&opr_dest);

    cpu.esp -= 4;
	opr_dest.addr = cpu.esp;
	opr_dest.val = cpu.ebx;
	operand_write(&opr_dest);

    cpu.esp -= 4;
	opr_dest.addr = cpu.esp;
	opr_dest.val = cur_esp;
	operand_write(&opr_dest);

    cpu.esp -= 4;
	opr_dest.addr = cpu.esp;
	opr_dest.val = cpu.ebp;
	operand_write(&opr_dest);

    cpu.esp -= 4;
	opr_dest.addr = cpu.esp;
	opr_dest.val = cpu.esi;
	operand_write(&opr_dest);

    cpu.esp -= 4;
	opr_dest.addr = cpu.esp;
	opr_dest.val = cpu.edi;
	operand_write(&opr_dest);
	
	return 1;
}
