#include "cpu/cpu.h"

void set_ZF(uint32_t result, size_t data_size)
{
    result = result & (0xffffffff >> (32 - data_size));
    cpu.eflags.ZF = (result == 0);
}

void set_SF(uint32_t result, size_t data_size)
{
    result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
    cpu.eflags.SF = sign(result);
}

void set_PF(uint32_t result) {
	result = result & (0xffffffff >> 24);
	uint32_t sum = 0;
	for(uint32_t i=0; i<8; i++) {
		uint32_t temp = result & (0x80 >> i);
		sum = sum + (temp != 0);
	}
	if(sum % 2 == 0)
		cpu.eflags.PF = 1;
	else
		cpu.eflags.PF = 0;
}


void set_CF_add(uint32_t result,uint32_t src,size_t data_size)
{
    result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
    cpu.eflags.CF = (result < src);
}

void set_CF_adc(uint32_t result,uint32_t src,size_t data_size)
{
    result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
	if(cpu.eflags.CF == 1)    
		cpu.eflags.CF = (result <= src);
	else
		cpu.eflags.CF = (result < src);
}

void set_CF_sub(uint32_t dest,uint32_t src,size_t data_size)
{
    dest = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
    cpu.eflags.CF = (dest < src);
}

void set_CF_sbb(uint32_t dest,uint32_t src,size_t data_size)
{
    dest = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
	if(cpu.eflags.CF == 1)    
		cpu.eflags.CF = (dest <= src);
	else
		cpu.eflags.CF = (dest < src);
}

void set_OF_add(uint32_t result,uint32_t src,uint32_t dest,size_t data_size){
	result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
	dest = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
	if(sign(src) == sign(dest)) {
		if(sign(src) != sign(result))
			cpu.eflags.OF = 1;
		else
			cpu.eflags.OF = 0;
	} else {
		cpu.eflags.OF = 0;
	}
    // printf("ADD %d: %lu + %lu = %lu. %d.\n", data_size, src, dest, result, cpu.eflags.OF);
}


void set_OF_sub(uint32_t result,uint32_t src,uint32_t dest,size_t data_size){
	result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
	dest = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
	if(sign(src) != sign(dest)) {
		if(sign(dest) != sign(result))
			cpu.eflags.OF = 1;
		else
			cpu.eflags.OF = 0;
	} else {
		cpu.eflags.OF = 0;
	}
    // printf("SUB %d: %lu + %lu = %lu. %d.\n", data_size, src, dest, result, cpu.eflags.OF);
}


uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	uint32_t res = 0;
	res = dest + src;

    set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
    set_CF_add(res, src, data_size);
	set_OF_add(res, src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}


uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	uint32_t res = 0;
    res = dest + src + cpu.eflags.CF;

    set_PF(res);
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_CF_adc(res, src, data_size);
    set_OF_add(res, src, dest, data_size);
    return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}


uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	uint32_t res = 0;
    res = dest - src;

    set_PF(res);
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_CF_sub(dest, src, data_size);
    set_OF_sub(res, src, dest, data_size);
    return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}


uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	uint32_t res = 0;
    res = dest - src - cpu.eflags.CF;
    
    set_PF(res);
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_CF_sbb(dest, src, data_size);
    set_OF_sub(res, src, dest, data_size);
    return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}


uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
    uint64_t res = 0;
    uint64_t src64 = src;
    uint64_t dest64 = dest;
    res = src64 * dest64;
    uint32_t higher_bits = (res >> data_size) & (0xFFFFFFFF >> (32 - data_size));
    cpu.eflags.CF = (higher_bits != 0);
    cpu.eflags.OF = (higher_bits != 0);
    return res;
#endif
}


int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	uint64_t res = 0;
    uint64_t src64 = src;
    uint64_t dest64 = dest;
    res = src64 * dest64;
    return res;
#endif
}


uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
    uint32_t res = 0;
    if(src == 0)
        return 0;
    res = dest / src;
    return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}


int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	uint32_t res = 0;
    res = dest / src;
    if(src == 0)
        return 0;
    return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
    uint32_t remainder = 0;
    if(src == 0)
        return 0;
    remainder = dest % src;
    return remainder;
#endif
}


int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	uint32_t remainder = 0;
    if(src == 0)
        return 0;
    remainder = dest % src;
    return remainder;
#endif
}


uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	uint32_t res = 0;
    res = src & dest;

    set_PF(res);
    set_ZF(res, data_size);
    set_SF(res, data_size);
    cpu.eflags.OF = 0;    
    cpu.eflags.CF = 0;
    return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}


uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	uint32_t res = 0;
    res = src ^ dest;

    set_PF(res);
    set_ZF(res, data_size);
    set_SF(res, data_size);
    cpu.eflags.OF = 0;    
    cpu.eflags.CF = 0;
    return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}


uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	uint32_t res = 0;
    res = src | dest;

    set_PF(res);
    set_ZF(res, data_size);
    set_SF(res, data_size);
    cpu.eflags.OF = 0;    
    cpu.eflags.CF = 0;
    return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}


uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size) {
	cpu.eflags.CF=0;
	dest=dest&(0xFFFFFFFF>>(32-data_size));
	while(src){
		if(data_size==8)
			cpu.eflags.CF=((dest&0x80)==0x80);
		else if(data_size==16)
			cpu.eflags.CF=((dest&0x8000)==0x8000);
 		else
			cpu.eflags.CF=((dest&0x80000000)==0x80000000);
		dest=dest<<1;
		src--;
	}
	set_PF(dest);
    //set_AF();
    set_ZF(dest,data_size);
	set_SF(dest,data_size);
	return dest&(0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size) {
	cpu.eflags.CF=0;
	dest=dest&(0xFFFFFFFF>>(32-data_size));
	while(src){
		cpu.eflags.CF=((dest&0x1)==0x1);
		dest=dest>>1;
		src--;
	}
	set_PF(dest);
    //set_AF();
    set_ZF(dest,data_size);
	set_SF(dest,data_size);
	return dest&(0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size) {
	dest=dest&(0xFFFFFFFF>>(32-data_size));
	if(data_size==8&&(dest&0x00000080)==0x00000080)
		while(src){
			cpu.eflags.CF=((dest&0x1)==0x1);
			dest=dest>>1;
			dest=dest^0x00000080;
			src--;
		}
	else if(data_size==16&&(dest&0x00008000)==0x00008000)
		while(src){
			cpu.eflags.CF=((dest&0x1)==0x1);
			dest=dest>>1;
			dest=dest^0x00008000;
			src--;
		}
	else if(data_size==32&&(dest&0x80000000)==0x80000000)
		while(src){
			cpu.eflags.CF=((dest&0x1)==0x1);
			dest=dest>>1;
			dest=dest^0x80000000;
			src--;
		}
	else
		return alu_shr(src,dest,data_size);
	set_PF(dest);
	//set_AF();
	set_ZF(dest,data_size);
	set_SF(dest,data_size);
	return dest&(0xFFFFFFFF>>(32-data_size));
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size) {
	return alu_shl(src,dest,data_size);
}

