#include "cpu/instr.h"

static void instr_execute_1op() {
	operand_read(&opr_src);
	opr_src.val = alu_add(1, opr_src.val, opr_src.data_size);
	operand_write(&opr_src);
}

make_instr_impl_1op(inc, r, v)
make_instr_impl_1op(inc, rm, v)

/*
make_instr_func(inc_r_v)
{
    OPERAND r;
    r.data_size = data_size;
    r.type = OPR_REG;
    r.addr = opcode & 0x7;
    operand_read(&r);
    r.val = alu_add(1, r.val, data_size);
    operand_write(&r);
    
    print_asm_1("inc", "", 1, &r);
    return 1;
}

make_instr_func(inc_rm_v)
{
    OPERAND rm;
    rm.data_size = data_size;
    rm.type = OPR_MEM;
    int len = 1;
    len += modrm_rm(eip + 1, &rm);
    operand_read(&rm);
    rm.val = alu_add(1, rm.val, data_size);
    operand_write(&rm);

    print_asm_1("inc", "", len, &rm);
    return len;
}
*/


