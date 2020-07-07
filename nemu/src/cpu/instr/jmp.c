#include "cpu/instr.h"

make_instr_func(jmp_near)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}

make_instr_func(jmp_near_indirect)
{
    int len = 1;    
    OPERAND rel;
    rel.data_size = data_size;
    len += modrm_rm(eip + 1, &rel);
    operand_read(&rel);
    print_asm_1("jmp", "", len, &rel);
    if(data_size == 16)
        cpu.eip = rel.val & 0xFFFF;
    else
        cpu.eip = rel.val;
    return 0;    
}

make_instr_func(jmp_short)
{
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = 8;
    rel.addr = eip + 1;

    operand_read(&rel);

    print_asm_1("jmp", "", 2, &rel);
    int offset = sign_ext(rel.val, rel.data_size);
    cpu.eip += offset;

    return 2;
}

make_instr_func(ljmp) 
{
    // printf("%x %x %x %x %x %x %x %x.\n", instr_fetch(eip, 1), instr_fetch(eip+1, 1),instr_fetch(eip+2, 1), instr_fetch(eip+3, 1), instr_fetch(eip+4, 1),instr_fetch(eip+5, 1), instr_fetch(eip+6, 1), instr_fetch(eip+7, 1));
	OPERAND rel;
	rel.type = OPR_IMM;
    rel.data_size = 32;
    rel.addr = eip + 1;
	operand_read(&rel);    
    print_asm_1("ljmp", "", 7, &rel);

	if(data_size == 16)
		cpu.eip = rel.val & 0xFFFF;
	else
		cpu.eip = rel.val;

	cpu.cs.val = instr_fetch(eip + 5, 2);
    // load CS, index 1.
	load_sreg(1);

    return 0;
}

