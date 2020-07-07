#include "cpu/instr.h"

static void instr_execute_1op_cc_pass() 
{
	opr_src.val = 1;
	operand_write(&opr_src);
}

static void instr_execute_1op_cc_fail() 
{
	opr_src.val = 0;
	operand_write(&opr_src);
}

/*
make_instr_func(setne_b)
{
    int len = 1;
    printf("%x", len);
    opr_src.data_size = 8;
    len += modrm_rm(cpu.eip + 1, &opr_src);
    // print_asm_1("setne", "", len, &opr_src);
    printf("%x", cpu.eflags.ZF);
    if(cpu.eflags.ZF == 0) {
        opr_src.val = 1;
	    operand_write(&opr_src);        
    } else {
        opr_src.val = 0;
	    operand_write(&opr_src);       
    }
    return len;
}
*/

make_instr_impl_1op_cc(set, rm, b, ne)
make_instr_impl_1op_cc(set, rm, b, a)
make_instr_impl_1op_cc(set, rm, b, ae)
make_instr_impl_1op_cc(set, rm, b, be)
make_instr_impl_1op_cc(set, rm, b, c)
make_instr_impl_1op_cc(set, rm, b, e)
make_instr_impl_1op_cc(set, rm, b, g)
make_instr_impl_1op_cc(set, rm, b, ge)
make_instr_impl_1op_cc(set, rm, b, l)
make_instr_impl_1op_cc(set, rm, b, le)
make_instr_impl_1op_cc(set, rm, b, no)
make_instr_impl_1op_cc(set, rm, b, np)
make_instr_impl_1op_cc(set, rm, b, ns)
make_instr_impl_1op_cc(set, rm, b, o)
make_instr_impl_1op_cc(set, rm, b, p)
make_instr_impl_1op_cc(set, rm, b, s)
