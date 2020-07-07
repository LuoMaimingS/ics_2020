#include "cpu/instr.h"

static void instr_execute_2op()
{
    operand_read(&opr_src);
    operand_read(&opr_dest);
    OPERAND temp;   
    temp.data_size = opr_src.data_size;
    temp.val = opr_dest.val;
    opr_dest.val = opr_src.val;
    opr_src.val = temp.val;

    operand_write(&opr_src);
    operand_write(&opr_dest);
}

make_instr_impl_2op(xcng, r, rm, b)
make_instr_impl_2op(xcng, r, rm, v)

make_instr_func(xcng_v)
{
    OPERAND r, a;
    r.type = a.type = OPR_REG;
    r.data_size = a.data_size = data_size;
    r.addr = opcode & 0x7;
    a.addr = 0;
    print_asm_1("xcng", "", 1, &r);
    
    OPERAND temp;
    temp.data_size = data_size;
    temp.val = r.val;
    r.val = a.val;
    a.val = temp.val;
    operand_write(&r);
    operand_write(&a);

    return 1;
}



