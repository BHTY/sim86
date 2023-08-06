#include "cpu.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

int using_segment_override;
uint16_t segment_override;

void cpu_clear_flag(regs_x86* cpu, uint8_t flag){
	cpu->flags &= ~(1 << flag);
}

void cpu_set_flag(regs_x86* cpu, uint8_t flag){
	cpu->flags |= (1 << flag);
}

void cpu_set_sf(regs_x86* cpu, uint16_t value){
	if (value & 0x8000){
		cpu_set_flag(cpu, 7);
	}else{
		cpu_clear_flag(cpu, 7);
	}
}

void cpu_set_of(regs_x86* cpu, uint16_t value){

}

void cpu_set_zf(regs_x86* cpu, uint16_t value){
	if (value == 0){
		cpu_set_flag(cpu, 6);
	}else{
		cpu_clear_flag(cpu, 6);
	}
}

void cpu_set_cf(regs_x86* cpu, uint16_t value){
	if (value){
		cpu_set_flag(cpu, 0);
	}else{
		cpu_clear_flag(cpu, 0);
	}
}

uint8_t alu_rol_8(regs_x86* cpu, uint16_t a, uint16_t b){
	uint8_t result = (a << b) | (a >> (8 - b));

	//handle flags

	return result;
}

uint32_t alu_mul_16(regs_x86* cpu, uint16_t a, uint16_t b){
	uint32_t result = a * b;

	//handle flags

	return result;
}

uint32_t alu_div_16(regs_x86* cpu, uint32_t a, uint16_t b){
	uint16_t result = a / b;
	uint16_t rem = a % b;

	// handle flags

	return result | (rem << 16);
}

uint8_t alu_shr_8(regs_x86* cpu, uint8_t a, uint8_t shamt){
	uint8_t result = a >> shamt;

	//handle flags

	return result;
}

uint16_t alu_shr_16(regs_x86* cpu, uint16_t a, uint8_t shamt){
	uint16_t result = a >> shamt;

	//handle flags

	return result;
}

uint8_t alu_sal_8(regs_x86* cpu, uint8_t a, uint8_t shamt){
	uint32_t result = a << shamt;

	//handle flags

	return result;
}

uint8_t alu_shl_8(regs_x86* cpu, uint8_t a, uint8_t shamt){
	uint32_t result = a << shamt;

	//handle flags

	return result;
}

uint16_t alu_shl_16(regs_x86* cpu, uint16_t a, uint8_t shamt){
	uint32_t result = a << shamt;

	//handle flags

	return result;
}

uint8_t alu_inc_8(regs_x86* cpu, uint8_t a, uint8_t unused){
	inc_8(a);
	return a;
}

uint8_t alu_dec_8(regs_x86* cpu, uint8_t a, uint8_t unused){
	dec_8(a);
	return a;
}

uint8_t alu_and_8(regs_x86* cpu, uint8_t a, uint8_t b){
	uint8_t result = a & b;

	cpu_clear_flag(cpu, 0); //clear carry
	cpu_clear_flag(cpu, 11); //clear overflow

	cpu_set_zf(cpu, result);
	cpu_set_sf(cpu, result);

	return result;
}

uint16_t alu_and_16(regs_x86* cpu, uint16_t a, uint16_t b){
	uint16_t result = a & b;

	cpu_clear_flag(cpu, 0); //clear carry
	cpu_clear_flag(cpu, 11); //clear overflow

	cpu_set_zf(cpu, result);
	cpu_set_sf(cpu, result);

	return result;
}

uint8_t alu_add_8(regs_x86* cpu, uint8_t a, uint8_t b){
	uint8_t result = a + b;

	cpu_set_zf(cpu, result);
	cpu_set_sf(cpu, result);
	//overflow flag
	cpu_set_cf(cpu, result < a);

	return result;
}

uint16_t alu_add_16(regs_x86* cpu, uint16_t a, uint16_t b){
	uint16_t result = a + b;

	cpu_set_zf(cpu, result);
	cpu_set_sf(cpu, result);
	//overflow flag
	cpu_set_cf(cpu, result < a);
	
	return result;
}

uint8_t alu_sub_8(regs_x86* cpu, uint8_t a, uint8_t b){
	uint8_t result = a - b;

	cpu_set_zf(cpu, result);
	//overflow flag
	cpu_set_sf(cpu, (int16_t)(int8_t)result);
	cpu_set_cf(cpu, result > a);

	return result;
}

uint16_t alu_sub_16(regs_x86* cpu, uint16_t a, uint16_t b){
	uint16_t result = a - b;

	cpu_set_zf(cpu, result);
	//overflow flag
	cpu_set_sf(cpu, result);
	cpu_set_cf(cpu, result > a);

	return result;
}

uint8_t alu_xor_8(regs_x86* cpu, uint8_t a, uint8_t b){ //returns value and sets flags
	uint8_t value = a ^ b;

	cpu_clear_flag(cpu, 11); //clear overflow flag
	cpu_clear_flag(cpu, 0); // clear carry flag

	cpu_set_sf(cpu, value);
	cpu_set_zf(cpu, value);

	//set parity flag nah

	return value;
}

uint16_t alu_xor_16(regs_x86* cpu, uint16_t a, uint16_t b){ //returns value and sets flags
	uint16_t value = a ^ b;

	cpu_clear_flag(cpu, 11); //clear overflow flag
	cpu_clear_flag(cpu, 0); // clear carry flag

	cpu_set_sf(cpu, value);
	cpu_set_zf(cpu, value);

	//set parity flag nah

	return value;
}

uint16_t alu_or_16(regs_x86* cpu, uint16_t a, uint16_t b){ //returns value and sets flags
	uint16_t value = a | b;

	cpu_clear_flag(cpu, 11); //clear overflow flag
	cpu_clear_flag(cpu, 0); // clear carry flag

	cpu_set_sf(cpu, value);
	cpu_set_zf(cpu, value);

	//set parity flag nah

	return value;
}

uint8_t alu_or_8(regs_x86* cpu, uint8_t a, uint8_t b){ //returns value and sets flags
	uint8_t value = a | b;

	cpu_clear_flag(cpu, 11); //clear overflow flag
	cpu_clear_flag(cpu, 0); // clear carry flag

	cpu_set_sf(cpu, (int16_t)(int8_t)value);
	cpu_set_zf(cpu, value);

	//set parity flag nah

	return value;
}

uint16_t calc_base(regs_x86* cpu, uint8_t modrm){
	if (using_segment_override){ //segment override
		return segment_override;
	}
	else if (RM(modrm) == 2 || RM(modrm) == 3 || (RM(modrm) == 6 && MOD(modrm) != 0)){
		return cpu->ss;
	}
	else{
		return cpu->ds;
	}
}

uint16_t calc_rel8_jmp(regs_x86* cpu, bus_x86* bus){
	return cpu->ip + 2 + (int16_t)(int8_t)bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
}

uint16_t calc_modrm_addr(regs_x86* cpu, bus_x86* bus){
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1)); //this also advances the IP
	uint16_t offset;
	uint16_t inst_size = 2;

	switch (RM(modrm)){
		case 0:
			offset = cpu->bx + cpu->si;
			break;
		case 1:
			offset = cpu->bx + cpu->di;
			break;
		case 2:
			offset = cpu->bp + cpu->si;
			break;
		case 3:
			offset = cpu->bp + cpu->di;
			break;
		case 4:
			offset = cpu->si;
			break;
		case 5:
			offset = cpu->di;
			break;
		case 6:
			if (MOD(modrm)){
				offset = cpu->bp;
			}else{
				offset = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 2));
				inst_size += 2;
			}
			break;
		case 7:
			offset = cpu->bx;
	}

	if (MOD(modrm) == 1){
		offset += (int16_t)(int8_t)bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 2));
		inst_size++;
	} else if (MOD(modrm) == 2){
		offset += (int16_t)bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 2));
		inst_size += 2;
	}

	cpu->ip += inst_size;

	return offset;
}

/*
Important flags: carry, sign, zero, overflow
*/

void op_00(regs_x86* cpu, bus_x86* bus){ //ADD r/m8, r8
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint16_t value;
	uint32_t linear_addr;

	switch (MOD(modrm)){
		case 3:
			value = alu_add_8(cpu, cpu_get_reg_8(cpu, RM(modrm)), cpu_get_reg_8(cpu, REG(modrm)));
			cpu_set_reg_8(cpu, RM(modrm), value);
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			linear_addr = linear_addr_rm(base, offset);
			value = alu_add_8(cpu, bus->peek_byte(linear_addr), cpu_get_reg_8(cpu, REG(modrm)));
			bus->poke_byte(linear_addr, value);
			break;
	}
}

void op_01(regs_x86* cpu, bus_x86* bus){ //ADD r/m16, r16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint16_t value;
	uint32_t linear_addr;

	switch (MOD(modrm)){
		case 3:
			value = alu_add_16(cpu, cpu_get_reg_16(cpu, RM(modrm)), cpu_get_reg_16(cpu, REG(modrm)));
			cpu_set_reg_16(cpu, RM(modrm), value);
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			linear_addr = linear_addr_rm(base, offset);
			value = alu_add_16(cpu, bus->peek_word(linear_addr), cpu_get_reg_16(cpu, REG(modrm)));
			bus->poke_word(linear_addr, value);
			break;
	}
}

void op_02(regs_x86* cpu, bus_x86* bus){ //add r8, r/m8
	decode_modrm_src_8();
	value = alu_add_8(cpu, value, cpu_get_reg_8(cpu, REG(modrm)));
	cpu_set_reg_8(cpu, REG(modrm), value);
}

void op_03(regs_x86* cpu, bus_x86* bus){ //add r16, r/m16
	decode_modrm_src_16();
	value = alu_add_16(cpu, value, cpu_get_reg_16(cpu, REG(modrm)));
	cpu_set_reg_16(cpu, REG(modrm), value);
}

void op_04(regs_x86* cpu, bus_x86* bus){ //add al, imm8
	uint8_t value = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->al = alu_add_8(cpu, cpu->al, value);
	cpu->ip += 2;
}

void op_05(regs_x86* cpu, bus_x86* bus){ //add ax, imm16
	uint16_t value = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ax = alu_add_16(cpu, cpu->ax, value);
	cpu->ip += 3;
}

void op_06(regs_x86* cpu, bus_x86* bus){ //PUSH ES
	log_instruction(cpu, 1, "PUSH ES\n");
	cpu_push_16(cpu, bus, cpu->es);
	cpu->ip++;
}

void op_07(regs_x86* cpu, bus_x86* bus){ //POP ES
	log_instruction(cpu, 1, "POP ES\n");
	cpu->es = cpu_pop_16(cpu, bus);
	cpu->ip++;
}

void op_09(regs_x86* cpu, bus_x86* bus){ //OR r/m16, r16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint16_t value;
	uint32_t linear_addr;

	switch (MOD(modrm)){
		case 3:
			value = alu_or_16(cpu, cpu_get_reg_16(cpu, RM(modrm)), cpu_get_reg_16(cpu, REG(modrm)));
			cpu_set_reg_16(cpu, RM(modrm), value);
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			linear_addr = linear_addr_rm(base, offset);
			value = alu_or_16(cpu, bus->peek_word(linear_addr), cpu_get_reg_16(cpu, REG(modrm)));
			bus->poke_word(linear_addr, value);
			break;
	}
}

void op_0A(regs_x86* cpu, bus_x86* bus){ //OR r8, r/m8
	decode_modrm_src_8();
	value = alu_or_8(cpu, value, cpu_get_reg_8(cpu, RM(modrm)));
	cpu_set_reg_8(cpu, RM(modrm), value);
}

void op_0E(regs_x86* cpu, bus_x86* bus){ //PUSH CS
	log_instruction(cpu, 1, "PUSH CS\n");
	cpu_push_16(cpu, bus, cpu->cs);
	cpu->ip++;
}

void op_16(regs_x86* cpu, bus_x86* bus){ //PUSH SS
	log_instruction(cpu, 1, "PUSH SS\n");
	cpu_push_16(cpu, bus, cpu->cs);
	cpu->ip++;

}

void op_19(regs_x86* cpu, bus_x86* bus){ //SBB r/m16, r16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t value = (cpu->flags & 0x1) + cpu_get_reg_16(cpu, REG(modrm));
	uint16_t offset;
	uint32_t linear_addr;

	switch (MOD(modrm)){
		case 3:
			value = alu_sub_16(cpu, cpu_get_reg_16(cpu, RM(modrm)), value);
			cpu_set_reg_16(cpu, RM(modrm), value);
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			linear_addr = linear_addr_rm(base, offset);
			value = alu_sub_16(cpu, bus->peek_word(linear_addr), value);
			bus->poke_word(linear_addr, value);
			break;
	}
}

void op_1E(regs_x86* cpu, bus_x86* bus){ //push ds
	log_instruction(cpu, 1, "PUSH DS\n");
	cpu_push_16(cpu, bus, cpu->ds);
	cpu->ip++;
}

void op_1F(regs_x86* cpu, bus_x86* bus){ //pop ds
	cpu->ds = cpu_pop_16(cpu, bus);
	cpu->ip++;
}

void op_20(regs_x86* cpu, bus_x86* bus){ //AND r/m8, r8
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t value = cpu_get_reg_8(cpu, REG(modrm));
	uint16_t offset;
	uint32_t linear_addr;

	switch (MOD(modrm)){
		case 3:
			value = alu_and_8(cpu, cpu_get_reg_8(cpu, RM(modrm)), value);
			cpu_set_reg_8(cpu, RM(modrm), value);
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			linear_addr = linear_addr_rm(base, offset);
			value = alu_and_16(cpu, bus->peek_word(linear_addr), value);
			bus->poke_word(linear_addr, value);
			break;
	}
}

void op_24(regs_x86* cpu, bus_x86* bus){ //AND AL, imm8
	uint8_t imm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	log_instruction(cpu, 1, "AND AL, %02x\n", imm);
	cpu->al = alu_and_8(cpu, cpu->al, imm);
	cpu->ip += 2;
}

void op_25(regs_x86* cpu, bus_x86* bus){ //AND AX, imm16
	uint16_t imm = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	log_instruction(cpu, 1, "AND AX, %04x\n", imm);
	cpu->ax = alu_and_16(cpu, cpu->al, imm);
	cpu->ip += 3;
}

void op_26(regs_x86* cpu, bus_x86* bus){ //ES segment override
	seg_override(cpu->es, "ES OVERRIDE: ");
}

void op_28(regs_x86* cpu, bus_x86* bus){ //SUB r/m8, r8
	decode_modrm_src_8();
	alu_op_8(alu_sub_8, cpu_get_reg_8(cpu, REG(modrm)));
}

void op_29(regs_x86* cpu, bus_x86* bus){ //SUB r/m16, r16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t value = cpu_get_reg_16(cpu, REG(modrm));
	uint16_t offset;
	uint32_t linear_addr;

	switch (MOD(modrm)){
		case 3:
			value = alu_sub_16(cpu, cpu_get_reg_16(cpu, RM(modrm)), value);
			cpu_set_reg_16(cpu, RM(modrm), value);
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			linear_addr = linear_addr_rm(base, offset);
			value = alu_sub_16(cpu, bus->peek_word(linear_addr), value);
			bus->poke_word(linear_addr, value);
			break;
	}
}

void op_2A(regs_x86* cpu, bus_x86* bus){ //SUB r8, r/m8
	decode_modrm_src_8();
	value = alu_sub_8(cpu, cpu_get_reg_8(cpu, REG(modrm)), value);
	cpu_set_reg_8(cpu, REG(modrm), value);
}

void op_2B(regs_x86* cpu, bus_x86* bus){ //SUB r16, r/m16
	decode_modrm_src_16();
	value = alu_sub_16(cpu, cpu_get_reg_16(cpu, REG(modrm)), value);
	cpu_set_reg_16(cpu, REG(modrm), value);
}


void op_2C(regs_x86* cpu, bus_x86* bus){ //SUB AL, imm8
	uint8_t imm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->al = alu_sub_8(cpu, cpu->al, imm);
	cpu->ip += 2;
}

void op_2E(regs_x86* cpu, bus_x86* bus){ //CS segment override
	seg_override(cpu->cs, "CS OVERRIDE: ");
}

void op_30(regs_x86* cpu, bus_x86* bus){ //XOR r/m8, r8
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint16_t value;
	uint32_t linear_addr;

	switch (MOD(modrm)){
	case 3:
		value = alu_xor_8(cpu, cpu_get_reg_8(cpu, RM(modrm)), cpu_get_reg_8(cpu, REG(modrm)));
		cpu_set_reg_8(cpu, RM(modrm), value);
		cpu->ip += 2;
		break;
	default:
		offset = calc_modrm_addr(cpu, bus);
		linear_addr = linear_addr_rm(base, offset);
		value = alu_xor_8(cpu, bus->peek_byte(linear_addr), cpu_get_reg_8(cpu, REG(modrm)));
		bus->poke_byte(linear_addr, value);
		break;
	}
}

void op_31(regs_x86* cpu, bus_x86* bus){ //XOR r/m16, r16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint16_t value;
	uint32_t linear_addr;

	switch (MOD(modrm)){
		case 3:
			value = alu_xor_16(cpu, cpu_get_reg_16(cpu, RM(modrm)), cpu_get_reg_16(cpu, REG(modrm)));
			cpu_set_reg_16(cpu, RM(modrm), value);
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			linear_addr = linear_addr_rm(base, offset);
			value = alu_xor_16(cpu, bus->peek_word(linear_addr), cpu_get_reg_16(cpu, REG(modrm)));
			bus->poke_word(linear_addr, value);
			break;
	}
}

void op_32(regs_x86* cpu, bus_x86* bus){ //XOR r8, r/m8
	decode_modrm_src_8();
	value = alu_xor_8(cpu, value, cpu_get_reg_8(cpu, RM(modrm)));
	cpu_set_reg_8(cpu, RM(modrm), value);
}

void op_33(regs_x86* cpu, bus_x86* bus){ //XOR r16, r/m16
	decode_modrm_src_16();
	value = alu_xor_16(cpu, value, cpu_get_reg_16(cpu, RM(modrm)));
	cpu_set_reg_16(cpu, RM(modrm), value);
}

void op_36(regs_x86* cpu, bus_x86* bus){ //SS override
	seg_override(cpu->ss, "SS OVERRIDE: ");
}

void op_39(regs_x86* cpu, bus_x86* bus){ //CMP r/m16, r16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;

	switch (MOD(modrm)){
		case 3:
			alu_sub_16(cpu, cpu_get_reg_16(cpu, RM(modrm)), cpu_get_reg_16(cpu, REG(modrm)));
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			alu_sub_16(cpu, bus->peek_word(linear_addr_rm(base, offset)), cpu_get_reg_16(cpu, REG(modrm)));
			break;
	}
}

void op_3A(regs_x86* cpu, bus_x86* bus){ //cmp r8, r/m8
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint8_t value;

	switch (MOD(modrm)){
		case 3:
			value = cpu_get_reg_8(cpu, RM(modrm));
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			value = bus->peek_byte(linear_addr_rm(base, offset));
			break;
	}

	alu_sub_8(cpu, cpu_get_reg_8(cpu, REG(modrm)), value);
}

void op_3B(regs_x86* cpu, bus_x86* bus){ //cmp r16, r/m16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint16_t value;

	switch (MOD(modrm)){
		case 3:
			value = cpu_get_reg_16(cpu, RM(modrm));
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			value = bus->peek_word(linear_addr_rm(base, offset));
			break;
	}

	alu_sub_16(cpu, cpu_get_reg_16(cpu, REG(modrm)), value);
}

void op_3C(regs_x86* cpu, bus_x86* bus){ //CMP AL, imm8
	alu_sub_8(cpu, cpu->al, bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1)));
	cpu->ip += 2;
}

void op_3E(regs_x86* cpu, bus_x86* bus){ //DS override
	seg_override(cpu->ds, "DS OVERRIDE: ");
}

void op_40(regs_x86* cpu, bus_x86* bus){ //inc ax
	inc_16(cpu->ax);
	cpu->ip++;
}

void op_41(regs_x86* cpu, bus_x86* bus){ //inc cx
	inc_16(cpu->cx);
	cpu->ip++;
}

void op_42(regs_x86* cpu, bus_x86* bus){ //inc dx
	inc_16(cpu->dx);
	cpu->ip++;
}

void op_43(regs_x86* cpu, bus_x86* bus){ //inc bx
	inc_16(cpu->bx);
	cpu->ip++;
}

void op_46(regs_x86* cpu, bus_x86* bus){ //inc si
	inc_16(cpu->si);
	cpu->ip++;
}

void op_47(regs_x86* cpu, bus_x86* bus){ //inc di
	inc_16(cpu->di);
	cpu->ip++;
}

void op_48(regs_x86* cpu, bus_x86* bus){ //dec ax
	dec_16(cpu->ax);
	cpu->ip++;
}

void op_49(regs_x86* cpu, bus_x86* bus){ //dec cx
	dec_16(cpu->cx);
	cpu->ip++;
}

void op_4A(regs_x86* cpu, bus_x86* bus){ //dec dx
	dec_16(cpu->dx);
	cpu->ip++;
}

void op_4B(regs_x86* cpu, bus_x86* bus){ //DEC BX
	dec_16(cpu->bx);
	cpu->ip++;
}

void op_50(regs_x86* cpu, bus_x86* bus){ //push ax
	cpu_push_16(cpu, bus, cpu->ax);
	cpu->ip++;
}

void op_51(regs_x86* cpu, bus_x86* bus){ //push cx
	cpu_push_16(cpu, bus, cpu->cx);
	cpu->ip++;
}

void op_52(regs_x86* cpu, bus_x86* bus){ //push dx
	cpu_push_16(cpu, bus, cpu->dx);
	cpu->ip++;
}

void op_53(regs_x86* cpu, bus_x86* bus){ //push bx
	cpu_push_16(cpu, bus, cpu->bx);
	cpu->ip++;
}

void op_54(regs_x86* cpu, bus_x86* bus){ //push sp
	cpu_push_16(cpu, bus, cpu->sp);
	cpu->ip++;
}

void op_55(regs_x86* cpu, bus_x86* bus){ //push bp
	cpu_push_16(cpu, bus, cpu->bp);
	cpu->ip++;
}

void op_56(regs_x86* cpu, bus_x86* bus){ //push si
	cpu_push_16(cpu, bus, cpu->si);
	cpu->ip++;
}

void op_57(regs_x86* cpu, bus_x86* bus){ //push di
	cpu_push_16(cpu, bus, cpu->di);
	cpu->ip++;
}

void op_58(regs_x86* cpu, bus_x86* bus){ //pop ax
	cpu->ax = cpu_pop_16(cpu, bus);
	cpu->ip++;
}

void op_59(regs_x86* cpu, bus_x86* bus){ //pop cx
	cpu->cx = cpu_pop_16(cpu, bus);
	cpu->ip++;
}

void op_5A(regs_x86* cpu, bus_x86* bus){ //pop dx
	cpu->dx = cpu_pop_16(cpu, bus);
	cpu->ip++;
}

void op_5B(regs_x86* cpu, bus_x86* bus){ //pop bx
	cpu->bx = cpu_pop_16(cpu, bus);
	cpu->ip++;
}

void op_5D(regs_x86* cpu, bus_x86* bus){ //pop bp
	cpu->bp = cpu_pop_16(cpu, bus);
	cpu->ip++;
}

void op_5E(regs_x86* cpu, bus_x86* bus){ //pop si
	cpu->si = cpu_pop_16(cpu, bus);
	cpu->ip++;
}

void op_5F(regs_x86* cpu, bus_x86* bus){ //pop di
	cpu->di = cpu_pop_16(cpu, bus);
	cpu->ip++;
}

void op_68(regs_x86* cpu, bus_x86* bus){ //push imm16
	uint16_t value = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu_push_16(cpu, bus, value);
	cpu->ip += 3;
}

void op_6A(regs_x86* cpu, bus_x86* bus){ //push imm8
	uint16_t value = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu_push_16(cpu, bus, value);
	cpu->ip += 2;
}

void op_6D(regs_x86* cpu, bus_x86* bus){ //insw
	uint32_t linear_addr = linear_addr_rm(cpu->es, cpu->di);
	uint16_t word = bus->in_word(cpu->dx);
	bus->poke_word(linear_addr, word);

	log_instruction(cpu, logging_instructions == 1, "INSW\n");

	if (cpu->flags & 0x400){
		cpu->di-=2;
	}
	else{
		cpu->di+=2;
	}

	cpu->ip++;
}

void op_72(regs_x86* cpu, bus_x86* bus){ //jc rel8
	cjmp(cpu->flags & 0x01);
}

void op_73(regs_x86* cpu, bus_x86* bus){ //jae rel8
	cjmp(!(cpu->flags & 0x01));
}

void op_74(regs_x86* cpu, bus_x86* bus){ //je rel8
	cjmp(cpu->flags & 0x40);
}

void op_75(regs_x86* cpu, bus_x86* bus){ //jnz rel8
	cjmp(!(cpu->flags & 0x40));
}

void op_76(regs_x86* cpu, bus_x86* bus){ //jna rel8
	cjmp(cpu->flags & 0x40 || cpu->flags & 0x01);
}

void op_77(regs_x86* cpu, bus_x86* bus){ //ja rel8
	cjmp(!(cpu->flags & 0x40) && !(cpu->flags & 0x01));
}

void op_79(regs_x86* cpu, bus_x86* bus){ //jns rel8
	cjmp(!(cpu->flags & 0x80));
}

void op_7E(regs_x86* cpu, bus_x86* bus){ //jng rel8
	cjmp((cpu->flags & 0x40) || (!(cpu->flags & 0x80) && (cpu->flags & 0x800)) || ((cpu->flags & 0x80) && !(cpu->flags & 0x800)));
}


void op_80(regs_x86* cpu, bus_x86* bus){ //OR r/m8, imm8
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t offset;
	uint16_t base = calc_base(cpu, modrm);
	uint8_t value;
	uint32_t linear_addr;
	uint8_t imm;

	switch (MOD(modrm)){
		case 3:
			value = cpu_get_reg_8(cpu, RM(modrm));
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			value = bus->peek_byte(linear_addr_rm(base, offset));
			break;
	}

	imm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip));
	cpu->ip++;

	switch (REG(modrm)){
		case 1:
			alu_op_8(alu_or_8, imm);
			break;
		case 4:
			alu_op_8(alu_and_8, imm);
			break;
		case 7:
			alu_sub_8(cpu, value, imm);
			break;
		default:
			printf("Unimplemented reg (%d) for 0x80!\n", REG(modrm));
			cpu->running = 0;
			break;
	}
}

void op_81(regs_x86* cpu, bus_x86* bus){ //cmp r/m16, imm16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t offset;
	uint16_t base = calc_base(cpu, modrm);
	uint16_t value;
	uint16_t cmp;
	uint16_t result;
	uint32_t linear_addr;

	switch (MOD(modrm)){
		case 3:
			value = cpu_get_reg_16(cpu, RM(modrm));
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			value = bus->peek_word(linear_addr_rm(base, offset));
			break;
	}

	cmp = (int16_t)bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip));
	cpu->ip+=2;

	switch (REG(modrm)){
		case 5: //subtract
			value = alu_sub_16(cpu, value, cmp);

			if (MOD(modrm) == 3){
				cpu_set_reg_16(cpu, RM(modrm), value);
			}
			else{
				bus->poke_word(linear_addr_rm(base, offset), value);
			}

			break;

		case 4: //and
			value = alu_and_16(cpu, value, cmp);

			if (MOD(modrm) == 3){
				cpu_set_reg_16(cpu, RM(modrm), value);
			}
			else{
				bus->poke_word(linear_addr_rm(base, offset), value);
			}

			break;
		case 2: //adc
			value = alu_add_16(cpu, value, cmp + (cpu->flags & 0x1));

			if (MOD(modrm) == 3){
				cpu_set_reg_16(cpu, RM(modrm), value);
			}
			else{
				bus->poke_word(linear_addr_rm(base, offset), value);
			}

			break;
		case 7: //cmp
			alu_sub_16(cpu, value, cmp);
			break;
		case 0: //add
			value = alu_add_16(cpu, value, cmp);

			if (MOD(modrm) == 3){
				cpu_set_reg_16(cpu, RM(modrm), value);
			}
			else{
				bus->poke_word(linear_addr_rm(base, offset), value);
			}

			break;
		default:
			printf("op81 %d!\n", REG(modrm));
			while (1);
			break;
	}
}

void op_83(regs_x86* cpu, bus_x86* bus){ //cmp r/m16, imm8
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t offset;
	uint16_t base = calc_base(cpu, modrm);
	uint16_t value;
	uint16_t cmp;
	uint16_t result;
	uint32_t linear_addr;

	switch (MOD(modrm)){
		case 3:
			value = cpu_get_reg_16(cpu, RM(modrm));
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			value = bus->peek_word(linear_addr_rm(base, offset));
			break;
	}

	cmp = (int16_t)(int8_t)bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip));
	cpu->ip++;

	switch (REG(modrm)){
		case 0: //add
			alu_op_16(alu_add_16, cmp);
			break;

		case 5: //subtract
			value = alu_sub_16(cpu, value, cmp);

			if (MOD(modrm) == 3){
				cpu_set_reg_16(cpu, RM(modrm), value);
			}
			else{
				bus->poke_word(linear_addr_rm(base, offset), value);
			}

			break;

		case 4: //and
			value = alu_and_16(cpu, value, cmp);

			if (MOD(modrm) == 3){
				cpu_set_reg_16(cpu, RM(modrm), value);
			} else{
				bus->poke_word(linear_addr_rm(base, offset), value);
			}

			break;
		case 2: //adc
			value = alu_add_16(cpu, value, cmp + (cpu->flags & 0x1));
			
			if (MOD(modrm) == 3){
				cpu_set_reg_16(cpu, RM(modrm), value);
			}
			else{
				bus->poke_word(linear_addr_rm(base, offset), value);
			}

			break;
		case 7: //cmp
			alu_sub_16(cpu, value, cmp);
			break;
		default:
			printf("op83!\n");
			while (1);
			break;
	}
}

void op_86(regs_x86* cpu, bus_x86* bus){ //xchg r/m8, r/m8
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint32_t linear_addr;
	uint16_t a = cpu_get_reg_8(cpu, REG(modrm));
	uint16_t b;

	switch (MOD(modrm)){
		case 3:
			b = cpu_get_reg_8(cpu, RM(modrm));
			cpu_set_reg_8(cpu, RM(modrm), a);
			cpu_set_reg_8(cpu, REG(modrm), b);
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			linear_addr = linear_addr_rm(base, offset);
			b = bus->peek_byte(linear_addr);
			cpu_set_reg_8(cpu, REG(modrm), b);
			bus->poke_byte(linear_addr, a);
			break;
	}
}

void op_87(regs_x86* cpu, bus_x86* bus){ //xchg r/m16, r/m16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint32_t linear_addr;
	uint16_t a, b;

	switch (MOD(modrm)){
		case 3:
			a = cpu_get_reg_16(cpu, REG(modrm));
			b = cpu_get_reg_16(cpu, RM(modrm));
			cpu_set_reg_16(cpu, RM(modrm), a);
			cpu_set_reg_16(cpu, REG(modrm), b);
			cpu->ip += 2;
			break;
		default:
			printf("xchg16 with memory!!!\n");
			while (1);
			break;
	}
}

void op_88(regs_x86* cpu, bus_x86* bus){ //mov r/m8, r8
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t value = cpu_get_reg_8(cpu, REG(modrm));
	uint16_t offset;
	uint16_t base = calc_base(cpu, modrm);
	uint32_t linear_addr;

	switch (MOD(modrm)){
		case 3:
			cpu_set_reg_8(cpu, RM(modrm), value);
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			linear_addr = linear_addr_rm(base, offset);
			bus->poke_byte(linear_addr, value);
			break;
	}
}

void op_89(regs_x86* cpu, bus_x86* bus){ //mov r/m16, r16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t value = cpu_get_reg_16(cpu, REG(modrm));
	uint16_t offset;
	uint16_t base = calc_base(cpu, modrm);
	uint32_t linear_addr;

	switch (MOD(modrm)){
		case 3:
			cpu_set_reg_16(cpu, RM(modrm), value);
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			linear_addr = linear_addr_rm(base, offset);
			bus->poke_word(linear_addr, value);
			break;
	}
}

void op_8A(regs_x86* cpu, bus_x86* bus){ //mov r8, r/m8
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t value;
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;

	switch (MOD(modrm)){
		case 3: //register
			value = cpu_get_reg_8(cpu, RM(modrm));
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			value = bus->peek_byte(linear_addr_rm(base, offset));
			break;
	}

	cpu_set_reg_8(cpu, REG(modrm), value);
}

void op_8B(regs_x86* cpu, bus_x86* bus){ //mov r16, r/m16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t value;
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;

	switch (MOD(modrm)){
		case 3: //register
			value = cpu_get_reg_16(cpu, RM(modrm));
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			value = bus->peek_word(linear_addr_rm(base, offset));
			break;
	}

	cpu_set_reg_16(cpu, REG(modrm), value);
}

void op_8C(regs_x86* cpu, bus_x86* bus){ //mov r/m16, Sreg
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint16_t value;

	switch (REG(modrm)){
		case 0:
			value = cpu->es;
			break;
		case 1:
			value = cpu->cs;
			break;
		case 2:
			value = cpu->ss;
			break;
		case 3:
			value = cpu->ds;
			break;
	}

	switch (MOD(modrm)){
		case 3:
			cpu_set_reg_16(cpu, RM(modrm), value);
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			bus->poke_word(linear_addr_rm(base, offset), value);
			break;
	}
}

void op_8E(regs_x86* cpu, bus_x86* bus){ //mov Sreg, r/m16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t value;
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;

	switch (MOD(modrm)){
		case 3:
			value = cpu_get_reg_16(cpu, RM(modrm));
			cpu->ip += 2;
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			value = bus->peek_word(linear_addr_rm(base, offset));
			break;
	}

	switch (REG(modrm)){
		case 0:
			cpu->es = value;
			break;
		case 1:
			cpu->cs = value;
			break;
		case 2:
			cpu->ss = value;
			break;
		case 3:
			cpu->ds = value;
			break;
	}
}

void op_8F(regs_x86* cpu, bus_x86* bus){ //pop r/m16
	uint16_t value = cpu_pop_16(cpu, bus);
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;

	if (REG(modrm) != 0){
		printf("We're fucked!\n");
		cpu->running = 0;
	}

	switch (MOD(modrm)){
		case 3:
			cpu_set_reg_16(cpu, RM(modrm), value);
			break;
		default:
			offset = calc_modrm_addr(cpu, bus);
			bus->poke_word(linear_addr_rm(base, offset), value);
			break;
	}
}

void op_90(regs_x86* cpu, bus_x86* bus){ //nop
	cpu->ip++;
}

void op_92(regs_x86* cpu, bus_x86* bus){ //xchg ax, dx
	uint16_t a = cpu->ax;
	uint16_t b = cpu->dx;

	cpu->ax = b;
	cpu->dx = a;

	cpu->ip++;
}

void op_93(regs_x86* cpu, bus_x86* bus){ //xchg ax, bx
	uint16_t a = cpu->ax;
	uint16_t b = cpu->bx;

	cpu->ax = b;
	cpu->bx = a;

	cpu->ip++;
}

void op_98(regs_x86* cpu, bus_x86* bus){ //cbw
	cpu->ax = (int16_t)(int8_t)cpu->al;
	cpu->ip++;
}

void op_A0(regs_x86* cpu, bus_x86* bus){ //mov al, moffs8
	uint16_t base = using_segment_override ? segment_override : cpu->ds;
	uint16_t offset = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->al = bus->peek_byte(linear_addr_rm(base, offset));
	cpu->ip += 3;
}

void op_A1(regs_x86* cpu, bus_x86* bus){ //mov ax, moffs16
	uint16_t base = using_segment_override ? segment_override : cpu->ds;
	uint16_t offset = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ax = bus->peek_word(linear_addr_rm(base, offset));
	cpu->ip += 3;
}

void op_A2(regs_x86* cpu, bus_x86* bus){ //mov moffs8, al
	uint16_t base = using_segment_override ? segment_override : cpu->ds;
	uint16_t offset = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	bus->poke_byte(linear_addr_rm(base, offset), cpu->al);
	cpu->ip += 3;
}

void op_A3(regs_x86* cpu, bus_x86* bus){ //mov moffs16, ax
	uint16_t base = using_segment_override ? segment_override : cpu->ds;
	uint16_t offset = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	bus->poke_word(linear_addr_rm(base, offset), cpu->ax);
	cpu->ip += 3;
}

void op_A4(regs_x86* cpu, bus_x86* bus){ //movsb
	init_string_op();
	uint8_t byte = bus->peek_byte(linear_addr_rm(source_base, cpu->si));
	bus->poke_byte(linear_addr_rm(dest_base, cpu->di), byte);

	finish_string_op_8("MOVSB\n");
}

void op_A5(regs_x86* cpu, bus_x86* bus){ //movsw
	init_string_op();
	uint16_t byte = bus->peek_word(linear_addr_rm(source_base, cpu->si));
	bus->poke_word(linear_addr_rm(dest_base, cpu->di), byte);
	finish_string_op_16("MOVSW\n");
}

void op_A6(regs_x86* cpu, bus_x86* bus){ //cmpsb
	init_string_op();
	uint8_t byte_src = bus->peek_byte(linear_addr_rm(source_base, cpu->si));
	uint8_t byte_dst = bus->peek_byte(linear_addr_rm(dest_base, cpu->di));

	alu_sub_8(cpu, byte_src, byte_dst);

	finish_string_op_8("CMPSB\n");
}

void op_AA(regs_x86* cpu, bus_x86* bus){ //stosb
	log_instruction(cpu, logging_instructions == 1, "STOSB\n");
	bus->poke_byte(linear_addr_rm(cpu->es, cpu->di), cpu->al);

	if (cpu->flags & 0x400){
		cpu->di--;
	}
	else{
		cpu->di++;
	}
	cpu->ip++;
}

void op_AB(regs_x86* cpu, bus_x86* bus){ //stosw
	log_instruction(cpu, logging_instructions == 1, "STOSW\n");
	bus->poke_byte(linear_addr_rm(cpu->es, cpu->di), cpu->ax);

	if (cpu->flags & 0x400){
		cpu->di -= 2;
	}
	else{
		cpu->di += 2;
	}
	cpu->ip++;
}

void op_AC(regs_x86* cpu, bus_x86* bus){ //lodsb
	uint16_t source_base = using_segment_override ? segment_override : cpu->ds;
	cpu->al = bus->peek_byte(linear_addr_rm(source_base, cpu->si));

	log_instruction(cpu, logging_instructions == 1, "LODSB\n");

	if (cpu->flags & 0x400){
		cpu->si--;
	}else{
		cpu->si++;
	}

	cpu->ip++;
}

void op_B0(regs_x86* cpu, bus_x86* bus){ //mov al, imm8
	cpu->al = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 2;
}

void op_B1(regs_x86* cpu, bus_x86* bus){ //mov cl, imm8
	cpu->cl = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 2;
}

void op_B2(regs_x86* cpu, bus_x86* bus){ //mov dl, imm8
	cpu->dl = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 2;
}

void op_B3(regs_x86* cpu, bus_x86* bus){ //mov bl, imm8
	cpu->bl = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 2;
}

void op_B4(regs_x86* cpu, bus_x86* bus){ //mov ah, imm8
	cpu->ah = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 2;
}

void op_B5(regs_x86* cpu, bus_x86* bus){ //mov ch, imm8
	cpu->ch = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 2;
}

void op_B6(regs_x86* cpu, bus_x86* bus){ //mov dh, imm8
	cpu->dh = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 2;
}

void op_B7(regs_x86* cpu, bus_x86* bus){ //mov bh, imm8
	cpu->bh = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 2;
}

void op_B8(regs_x86* cpu, bus_x86* bus){ //mov ax, imm16
	cpu->ax = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 3;
}

void op_B9(regs_x86* cpu, bus_x86* bus){ //mov cx, imm16
	cpu->cx = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 3;
}

void op_BA(regs_x86* cpu, bus_x86* bus){ //mov dx, imm16
	cpu->dx = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 3;
}

void op_BB(regs_x86* cpu, bus_x86* bus){ //mov bx, imm16
	cpu->bx = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 3;
}

void op_BC(regs_x86* cpu, bus_x86* bus){ //mov sp, imm16
	cpu->sp = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 3;
}

void op_BE(regs_x86* cpu, bus_x86* bus){ //mov si, imm16
	cpu->si = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 3;
}

void op_BF(regs_x86* cpu, bus_x86* bus){ //mov di, imm16
	cpu->di = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 3;
}

void op_C3(regs_x86* cpu, bus_x86* bus){ //near ret
	cpu_ret_near(cpu, bus);
}

void op_C5(regs_x86* cpu, bus_x86* bus){ //LDS
	decode_modrm_src_16();
	cpu_set_reg_16(cpu, REG(modrm), value);
}

void op_C6(regs_x86* cpu, bus_x86* bus){ //MOV r/m8, imm8
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint16_t value;

	if (REG(modrm) != 0){
		printf("OpC6 needs some work!\n");
		while (1);
	}

	if (MOD(modrm) == 3){ //register
		cpu->ip += 2;
		value = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip));
		cpu_set_reg_8(cpu, RM(modrm), value);
	}
	else{ //memory
		offset = calc_modrm_addr(cpu, bus);
		value = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip));
		bus->poke_byte(linear_addr_rm(base, offset), value);
	}

	cpu->ip += 1;
}

void op_C7(regs_x86* cpu, bus_x86* bus){ //mov r/m16, imm16
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t base = calc_base(cpu, modrm);
	uint16_t offset;
	uint16_t value;

	if (REG(modrm) != 0){
		printf("OpC7 needs some work!\n");
		while (1);
	}

	if (MOD(modrm) == 3){ //register
		cpu->ip += 2;
		value = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip));
		cpu_set_reg_16(cpu, RM(modrm), value);
	}else{ //memory
		offset = calc_modrm_addr(cpu, bus);
		value = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip));
		bus->poke_word(linear_addr_rm(base, offset), value);
	}

	cpu->ip += 2;
}

void op_CB(regs_x86* cpu, bus_x86* bus){ //far ret
	cpu_ret_far(cpu, bus);
}

void op_CC(regs_x86* cpu, bus_x86* bus){ //int3
	cpu->ip++;
	cpu_int(cpu, bus, 3);
}

void op_CD(regs_x86* cpu, bus_x86* bus){ //int imm8
	uint8_t vect = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 2;
	cpu_int(cpu, bus, vect);
}

void op_CF(regs_x86* cpu, bus_x86* bus){ //iret
	cpu_iret(cpu, bus);
}

void op_D0(regs_x86* cpu, bus_x86* bus){ //rotate r/m8, 1
	decode_modrm_src_8();

	switch (REG(modrm)){
		case 0:
			alu_op_8(alu_rol_8, 1);
			break;
		case 4:
			alu_op_8(alu_sal_8, 1);
			break;
		case 5:
			alu_op_8(alu_shr_8, 1);
			break;
		default:
			printf("Unimplemented case %d for 0xD0!\n", REG(modrm));
			cpu->running = 0;
			break;
	}
}

void op_D1(regs_x86* cpu, bus_x86* bus){ //shift r/m16, 1
	decode_modrm_src_16();

	switch (REG(modrm)){
		case 4:
			alu_op_16(alu_shl_16, 1);
			break;
		case 5:
			alu_op_16(alu_shr_16, 1);
			break;
		default:
			printf("Unimplemented case %d for 0xD1!\n", REG(modrm));
			cpu->running = 0;
			break;
	}
}

void op_D2(regs_x86* cpu, bus_x86* bus){ //shift r/m8, cl
	decode_modrm_src_8();

	switch (REG(modrm)){
		case 4:
			alu_op_8(alu_shl_8, cpu->cl);
			break;
		default:
			printf("Unimplemented case %d for 0xD2!\n", REG(modrm));
			cpu->running = 0;
			break;
	}
}

void op_D3(regs_x86* cpu, bus_x86* bus){ //shift r/m16, cl
	decode_modrm_src_16();

	switch (REG(modrm)){
		case 4:
			alu_op_16(alu_shl_16, cpu->cl);
			break;
		default:
			printf("Unimplemented case %d for 0xD3!\n", REG(modrm));
			cpu->running = 0;
			break;
	}
}

void op_E2(regs_x86* cpu, bus_x86* bus){ //loop rel8
	log_instruction(cpu, 1, "LOOP CS:");
	
	cpu->cx--;
	cjmp(cpu->cx);
}

void op_E3(regs_x86* cpu, bus_x86* bus){
	cjmp(cpu->cx == 0);
}

void op_E4(regs_x86* cpu, bus_x86* bus){ //in al, imm8
	uint8_t port = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	log_instruction(cpu, 1, "IN AL, %02x", port);
	cpu->al = bus->in_byte(port);
	cpu->ip += 2;
}

void op_E9(regs_x86* cpu, bus_x86* bus){ //near jmp rel16
	uint16_t offset = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));

	cpu->ip += 3 + offset;
}

void op_EA(regs_x86* cpu, bus_x86* bus){ //far jmp ptr16:16
	uint16_t new_cs = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 3));
	uint16_t new_ip = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));

	cpu->ip = new_ip;
	cpu->cs = new_cs;
}

void op_EB(regs_x86* cpu, bus_x86* bus){ //jmp rel8
	cjmp(1);
}

void op_E6(regs_x86* cpu, bus_x86* bus){ //OUT imm8, AL
	bus->out_byte(bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1)), cpu->al);
	cpu->ip += 2;
}

void op_E7(regs_x86* cpu, bus_x86* bus){ //OUT imm8, AX
	bus->out_word(bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1)), cpu->ax);
	cpu->ip += 2;
}

void op_E8(regs_x86* cpu, bus_x86* bus){ //near CALL rel16
	uint16_t target = cpu->ip + 3 + bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 1));
	cpu->ip += 3;
	cpu_call_near(cpu, bus, target);
}

void op_EE(regs_x86* cpu, bus_x86* bus){ //OUT DX, AL
	bus->out_byte(cpu->dx, cpu->al);
	cpu->ip += 1;
}

void op_EF(regs_x86* cpu, bus_x86* bus){ //OUT DX, AX
	bus->out_word(cpu->dx, cpu->ax);
	cpu->ip += 1;
}

void op_F2(regs_x86* cpu, bus_x86* bus){ //REPNE prefix
	printf("Haha!\n");
	while (1);

	uint16_t cur_ip = cpu->ip;
	int old_logging = logging_instructions;
	logging_instructions = logging_instructions ? 2 : 0;
	log_instruction(cpu, 1, "REPNE ");

	while (cpu->cx){
		cpu->ip++;
		cpu_step(cpu, bus);
		cpu->cx--;
		if (cpu->cx) cpu->ip = cur_ip;
		if (cpu->flags & 0x40) break;
	}

	logging_instructions = old_logging;
}

void op_F3(regs_x86* cpu, bus_x86* bus){ //REPE prefix
	uint8_t instruction_byte = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t cur_ip = cpu->ip;
	int old_logging = logging_instructions;
	log_instruction(cpu, logging_instructions == 1, "REPE ");
	logging_instructions = logging_instructions ? 2 : 0;

	while (cpu->cx){
		cpu->ip++;
		cpu_step(cpu, bus);
		cpu->cx--;

		if (REPE_INSTRUCTION(instruction_byte) && !(cpu->flags & 0x40)) break;
		if (cpu->cx){
			cpu->ip = cur_ip;
		}

		logging_instructions = 0;
	}

	logging_instructions = old_logging;
}

void op_F4(regs_x86* cpu, bus_x86* bus){ //HLT
	cpu->running = 0;
}

void op_F6(regs_x86* cpu, bus_x86* bus){ //TEST r/m8, imm8
	decode_modrm_src_8();
	uint8_t imm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip));
	cpu->ip++;

	switch (REG(modrm)){
		case 0:
			alu_and_8(cpu, value, imm);
			break;
		default:
			printf("Unimplemented case %d for instruction 0xF6!\n", REG(modrm));
			cpu->running = 0;
			break;
	}
}

void op_F7(regs_x86* cpu, bus_x86* bus){ //MUL r/m16
	uint32_t temp;
	decode_modrm_src_16();

	switch (REG(modrm)){
		case 4: //mul
			temp = alu_mul_16(cpu, cpu->ax, value);
			cpu->ax = temp;
			cpu->dx = temp >> 16;
			break;
		case 6: //div
			temp = (cpu->dx << 16) | cpu->ax;
			temp = alu_div_16(cpu, temp, value);
			cpu->ax = temp;
			cpu->dx = temp >> 16;
			break;
		default:
			printf("Unimplemented %d!\n", REG(modrm));
			cpu->running = 0;
			break;
	}
}

void op_F8(regs_x86* cpu, bus_x86* bus){ //clc
	cpu_set_flag(cpu, 1);
	cpu->ip++;
}

void op_F9(regs_x86* cpu, bus_x86* bus){ //stc
	cpu_set_flag(cpu, 0);
	cpu->ip++;
}

void op_FA(regs_x86* cpu, bus_x86* bus){ //cli
	cpu_clear_flag(cpu, 9);
	cpu->ip++;
}

void op_FB(regs_x86* cpu, bus_x86* bus){ //sti
	cpu_set_flag(cpu, 9);
	cpu->ip++;
}

void op_FC(regs_x86* cpu, bus_x86* bus){ //CLD
	log_instruction(cpu, 1, "CLD\n");

	cpu_clear_flag(cpu, 10);
	cpu->ip++;
}

void op_FE(regs_x86* cpu, bus_x86* bus){ //INC/DEC r/m8
	decode_modrm_src_8();
	alu_op_8((REG(modrm) ? alu_dec_8 : alu_inc_8), 0);
}

void op_FF(regs_x86* cpu, bus_x86* bus){ //CALL
	uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1));
	uint16_t address, seg;
	uint16_t base = using_segment_override ? segment_override : cpu->ds;

	switch (REG(modrm)){
		case 2: //CALL r/m16 (near call)
			switch (MOD(modrm)){
				case 3:
					address = cpu_get_reg_16(cpu, RM(modrm));
					cpu->ip += 2;
					break;
				default:
					printf("Absolute indirect near calls to m16 aren't implemented yet!\n");
					cpu->running = 0;
					break;
			}
			//printf("Calling address CS:%04x\n", address);
			cpu_call_near(cpu, bus, address);
			break;
		case 3: //CALL m16:16 (absolute indirect far call)
			address = bus->peek_word(linear_addr_rm(cpu->cs, cpu->ip + 2));
			seg = bus->peek_word(linear_addr_rm(base, address + 2));
			address = bus->peek_word(linear_addr_rm(base, address));
			cpu->ip += 4;
			cpu_call_far(cpu, bus, seg, address);
			break;
		default:
			printf("help!\n");
			cpu->running = 0;
			break;
	}
}

void(*op_table[256])(regs_x86* cpu, bus_x86* bus) = {
	0, //0x0
	op_01, //0x1
	op_02, //0x2
	op_03, //0x3
	op_04, //0x4
	op_05, //0x5
	op_06, //0x6
	op_07, //0x7
	0, //0x8
	op_09, //0x9
	op_0A, //0xa
	0, //0xb
	0, //0xc
	0, //0xd
	op_0E, //0xe
	0, //0xf
	0, //0x10
	0, //0x11
	0, //0x12
	0, //0x13
	0, //0x14
	0, //0x15
	op_16, //0x16
	0, //0x17
	0, //0x18
	op_19, //0x19
	0, //0x1a
	0, //0x1b
	0, //0x1c
	0, //0x1d
	op_1E, //0x1e
	op_1F, //0x1f
	op_20, //0x20
	0, //0x21
	0, //0x22
	0, //0x23
	op_24, //0x24
	op_25, //0x25
	op_26, //0x26
	0, //0x27
	op_28, //0x28
	op_29, //0x29
	op_2A, //0x2a
	op_2B, //0x2b
	op_2C, //0x2c
	0, //0x2d
	op_2E, //0x2e
	0, //0x2f
	op_30, //0x30
	op_31, //0x31
	op_32, //0x32
	op_33, //0x33
	0, //0x34
	0, //0x35
	op_36, //0x36
	0, //0x37
	0, //0x38
	op_39, //0x39
	op_3A, //0x3a
	op_3B, //0x3b
	op_3C, //0x3c
	0, //0x3d
	op_3E, //0x3e
	0, //0x3f
	op_40, //0x40
	op_41, //0x41
	op_42, //0x42
	op_43, //0x43
	0, //0x44
	0, //0x45
	op_46, //0x46
	op_47, //0x47
	op_48, //0x48
	op_49, //0x49
	op_4A, //0x4a
	op_4B, //0x4b
	0, //0x4c
	0, //0x4d
	0, //0x4e
	0, //0x4f
	op_50, //0x50
	op_51, //0x51
	op_52, //0x52
	op_53, //0x53
	op_54, //0x54
	op_55, //0x55
	op_56, //0x56
	op_57, //0x57
	op_58, //0x58
	op_59, //0x59
	op_5A, //0x5a
	op_5B, //0x5b
	0, //0x5c
	op_5D, //0x5d
	op_5E, //0x5e
	op_5F, //0x5f
	0, //0x60
	0, //0x61
	0, //0x62
	0, //0x63
	0, //0x64
	0, //0x65
	0, //0x66
	0, //0x67
	op_68, //0x68
	0, //0x69
	op_6A, //0x6a
	0, //0x6b
	0, //0x6c
	op_6D, //0x6d
	0, //0x6e
	0, //0x6f
	0, //0x70
	0, //0x71
	op_72, //0x72
	op_73, //0x73
	op_74, //0x74
	op_75, //0x75
	op_76, //0x76
	op_77, //0x77
	0, //0x78
	op_79, //0x79
	0, //0x7a
	0, //0x7b
	0, //0x7c
	0, //0x7d
	op_7E, //0x7e
	0, //0x7f
	op_80, //0x80
	op_81, //0x81
	0, //0x82
	op_83, //0x83
	0, //0x84
	0, //0x85
	op_86, //0x86
	op_87, //0x87
	op_88, //0x88
	op_89, //0x89
	op_8A, //0x8a
	op_8B, //0x8b
	op_8C, //0x8c
	0, //0x8d
	op_8E, //0x8e
	op_8F, //0x8f
	op_90, //0x90
	0, //0x91
	op_92, //0x92
	op_93, //0x93
	0, //0x94
	0, //0x95
	0, //0x96
	0, //0x97
	op_98, //0x98
	0, //0x99
	0, //0x9a
	0, //0x9b
	0, //0x9c
	0, //0x9d
	0, //0x9e
	0, //0x9f
	op_A0, //0xa0
	op_A1, //0xa1
	op_A2, //0xa2
	op_A3, //0xa3
	op_A4, //0xa4
	op_A5, //0xa5
	op_A6, //0xa6
	0, //0xa7
	0, //0xa8
	0, //0xa9
	op_AA, //0xaa
	op_AB, //0xab
	op_AC, //0xac
	0, //0xad
	0, //0xae
	0, //0xaf
	op_B0, //0xb0
	op_B1, //0xb1
	op_B2, //0xb2
	op_B3, //0xb3
	op_B4, //0xb4
	op_B5, //0xb5
	op_B6, //0xb6
	op_B7, //0xb7
	op_B8, //0xb8
	op_B9, //0xb9
	op_BA, //0xba
	op_BB, //0xbb
	op_BC, //0xbc
	0, //0xbd
	op_BE, //0xbe
	op_BF, //0xbf
	0, //0xc0
	0, //0xc1
	0, //0xc2
	op_C3, //0xc3
	0, //0xc4
	op_C5, //0xc5
	op_C6, //0xc6
	op_C7, //0xc7
	0, //0xc8
	0, //0xc9
	0, //0xca
	op_CB, //0xcb
	op_CC, //0xcc
	op_CD, //0xcd
	0, //0xce
	op_CF, //0xcf
	op_D0, //0xd0
	op_D1, //0xd1
	op_D2, //0xd2
	op_D3, //0xd3
	0, //0xd4
	0, //0xd5
	0, //0xd6
	0, //0xd7
	0, //0xd8
	0, //0xd9
	0, //0xda
	0, //0xdb
	0, //0xdc
	0, //0xdd
	0, //0xde
	0, //0xdf
	0, //0xe0
	0, //0xe1
	op_E2, //0xe2
	op_E3, //0xe3
	op_E4, //0xe4
	0, //0xe5
	op_E6, //0xe6
	op_E7, //0xe7
	op_E8, //0xe8
	op_E9, //0xe9
	op_EA, //0xea
	op_EB, //0xeb
	0, //0xec
	0, //0xed
	op_EE, //0xee
	op_EF, //0xef
	0, //0xf0
	0, //0xf1
	0, //0xf2
	op_F3, //0xf3
	op_F4, //0xf4
	0, //0xf5
	op_F6, //0xf6
	op_F7, //0xf7
	op_F8, //0xf8
	op_F9, //0xf9
	op_FA, //0xfa
	op_FB, //0xfb
	op_FC, //0xfc
	0, //0xfd
	op_FE, //0xfe
	op_FF, //0xff
};

void cpu_init(regs_x86* cpu){
	memset(cpu, 0, sizeof(regs_x86));
	cpu->cs = 0xffff;
	cpu->sp = 0x100;
	cpu->running = 1;
}

void cpu_dump(regs_x86* cpu){
	printf("AX=%04x BX=%04x CX=%04x DX=%04x DS:SI=%04x:%04x ES:DI=%04x:%04x\n", cpu->ax, cpu->bx, cpu->cx, cpu->dx, cpu->ds, cpu->si, cpu->es, cpu->di);
	printf("SS:SP=%04x:%04x CS:IP=%04x:%04x BP=%04x FLAGS=%x\n\n", cpu->ss, cpu->sp, cpu->cs, cpu->ip, cpu->bp, cpu->flags);
}

uint32_t linear_addr_rm(uint16_t seg, uint16_t offset){ //converts base:offset to linear address
	return (seg << 4) + offset;
}

void cpu_step(regs_x86* cpu, bus_x86* bus){
	uint8_t byte = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip));

	if (op_table[byte] == 0){
		printf("Unimplemented instruction %x at %04x:%04x\n", byte, cpu->cs, cpu->ip);
		cpu->running = 0;
	}
	else{
		op_table[byte](cpu, bus);
	}
}

void cpu_push_16(regs_x86* cpu, bus_x86* bus, uint16_t val){
	cpu->sp -= 2;
	bus->poke_word(linear_addr_rm(cpu->ss, cpu->sp), val);
}

uint16_t cpu_pop_16(regs_x86* cpu, bus_x86* bus){
	uint16_t value = bus->peek_word(linear_addr_rm(cpu->ss, cpu->sp));
	cpu->sp += 2;

	return value;
}

void cpu_set_reg_8(regs_x86* cpu, uint8_t reg, uint8_t value){
	switch (reg){
		case 0:
			cpu->al = value;
			break;
		case 1:
			cpu->cl = value;
			break;
		case 2:
			cpu->dl = value;
			break;
		case 3:
			cpu->bl = value;
			break;
		case 4:
			cpu->ah = value;
			break;
		case 5:
			cpu->ch = value;
			break;
		case 6:
			cpu->dh = value;
			break;
		case 7:
			cpu->bh = value;
			break;
	}
}

void cpu_set_reg_16(regs_x86* cpu, uint8_t reg, uint16_t value){
	switch (reg){
		case 0:
			cpu->ax = value;
			break;
		case 1:
			cpu->cx = value;
			break;
		case 2:
			cpu->dx = value;
			break;
		case 3:
			cpu->bx = value;
			break;
		case 4:
			cpu->sp = value;
			break;
		case 5:
			cpu->bp = value;
			break;
		case 6:
			cpu->si = value;
			break;
		case 7:
			cpu->di = value;
			break;
	}
}

uint8_t cpu_get_reg_8(regs_x86* cpu, uint8_t reg){
	uint8_t regs[8] = {cpu->al, cpu->cl, cpu->dl, cpu->bl, cpu->ah, cpu->ch, cpu->dh, cpu->bh};
	return regs[reg];
}

uint16_t cpu_get_reg_16(regs_x86* cpu, uint8_t reg){
	uint16_t regs[8] = { cpu->ax, cpu->cx, cpu->dx, cpu->bx, cpu->sp, cpu->bp, cpu->si, cpu->di };
	return regs[reg];
}

//remember that the IP has to be adjusted before calling one of those functions (especially so that reljumps can be calculated)

void cpu_int(regs_x86* cpu, bus_x86* bus, uint8_t vect){
	if (!(cpu->flags & 0x200)){
		return;
	}

	if (vect == 0x13){
		//printf("Interrupt %xh has been raised! (AH=%02x AL=%02x CH=%02x CL=%02x DH=%02x DL=%02x ES:BX=%04x:%04x CS:IP=%04x:%04x)\n", vect, cpu->ah, cpu->al, cpu->ch, cpu->cl, cpu->dh, cpu->dl, cpu->es, cpu->bx, cpu->cs, cpu->ip);
		if (cpu->ah == 0x02){
			printf("%04x:%04x: Loading %d sectors from cylinder %d head %d sector %d to %04x:%04x (%05x)\n", cpu->cs, cpu->ip, cpu->al, cpu->ch, cpu->dh, cpu->cl, cpu->es, cpu->bx, linear_addr_rm(cpu->es, cpu->bx));
		}
		else{
			printf("%04x:%04x	Disk interrupt code %02x\n", cpu->cs, cpu->ip, cpu->ah);
		}
	}
	else{
		//printf("Interrupt %xh has been raised! (AH=%02x AL=%02x CH=%02x CL=%02x DH=%02x DL=%02x ES:BX=%04x:%04x CS:IP=%04x:%04x)\n", vect, cpu->ah, cpu->al, cpu->ch, cpu->cl, cpu->dh, cpu->dl, cpu->es, cpu->bx, cpu->cs, cpu->ip);
	}

	//cpu_dump(cpu);

	cpu_push_16(cpu, bus, cpu->flags);
	cpu_push_16(cpu, bus, cpu->cs);
	cpu_push_16(cpu, bus, cpu->ip);

	cpu->ip = bus->peek_word(vect << 2);
	cpu->cs = bus->peek_word((vect << 2) + 2);
	cpu->running = 1;

	/*if (vect == 0x09){
		printf("Jumping to %04x:%04x\n", cpu->cs, cpu->ip);
		printf("The byte located there is %02x\n", bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip)));
		//while (1);
	}*/

	//printf("\n---DUMPING THE IVT---\n");

	for (int i = 0; i < 256; i++){
		//printf("Interrupt %02x: %04x:%04x\n", i, bus->peek_word(i * 4 + 2), bus->peek_word(i * 4));
	}
}

void cpu_iret(regs_x86* cpu, bus_x86* bus){
	cpu->ip = cpu_pop_16(cpu, bus);
	cpu->cs = cpu_pop_16(cpu, bus);
	cpu->flags = cpu_pop_16(cpu, bus);

	//printf("IRET back to %04x:%04x\n", cpu->cs, cpu->ip);
}

void cpu_call_near(regs_x86* cpu, bus_x86* bus, uint16_t ip){
	cpu_push_16(cpu, bus, cpu->ip);
	cpu->ip = ip;
}

void cpu_ret_near(regs_x86* cpu, bus_x86* bus){
	cpu->ip = cpu_pop_16(cpu, bus);
	//printf("Returning to %04x:%04x\n", cpu->cs, cpu->ip);
}

void cpu_call_far(regs_x86* cpu, bus_x86* bus, uint16_t cs, uint16_t ip){
	cpu_push_16(cpu, bus, cpu->cs);
	cpu_push_16(cpu, bus, cpu->ip);

	cpu->cs = cs;
	cpu->ip = ip;
}

void cpu_ret_far(regs_x86* cpu, bus_x86* bus){
	cpu->ip = cpu_pop_16(cpu, bus);
	cpu->cs = cpu_pop_16(cpu, bus);
}

int logging_instructions = 0;

void log_instruction(regs_x86* cpu, int print_csip, char* fmt, ...){
	va_list args;

	if (logging_instructions){
		if (print_csip){
			printf("%04x:%04x		", cpu->cs, cpu->ip);
		}

		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}