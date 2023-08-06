#pragma once
#include <stdint.h>

#define MOD(modrm) ((modrm & 0xc0) >> 6)
#define REG(modrm) ((modrm & 0x38) >> 3)
#define RM(modrm) (modrm & 0x7)

#define REPE_INSTRUCTION(byte)		(byte == 0xAE || byte == 0xAF || byte == 0xA6 || byte == 0xA7)

#define init_string_op()			uint16_t source_base = using_segment_override ? segment_override : cpu->ds; \
									uint16_t dest_base = cpu->es;

#define finish_string_op_8(name)	log_instruction(cpu, logging_instructions == 1, name); \
									if (cpu->flags & 0x400){ \
										cpu->si--; \
										cpu->di--; \
									} else{ \
										cpu->si++; \
										cpu->di++; \
									} \
									cpu->ip++;									

#define finish_string_op_16(name)	log_instruction(cpu, logging_instructions == 1, name); \
									if (cpu->flags & 0x400){ \
										cpu->si -= 2; \
										cpu->di -= 2; \
									} else{ \
										cpu->si += 2; \
										cpu->di += 2; \
									} \
									cpu->ip++;

#define seg_override(seg, name)		int old_logging = logging_instructions; \
									log_instruction(cpu, logging_instructions == 1, name); \
									logging_instructions = old_logging ? 2 : 0; \
									using_segment_override = 1; \
									segment_override = seg; \
									cpu->ip++; \
									cpu_step(cpu, bus); \
									using_segment_override = 0; \
									logging_instructions = old_logging;

#define decode_modrm_src_8()		uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1)); \
									uint16_t base = calc_base(cpu, modrm); \
									uint16_t offset; \
									uint8_t value; \
									switch (MOD(modrm)){ \
										case 3: \
											value = cpu_get_reg_8(cpu, RM(modrm)); \
											cpu->ip += 2; \
											break; \
										default: \
											offset = calc_modrm_addr(cpu, bus); \
											value = bus->peek_byte(linear_addr_rm(base, offset)); \
											break; \
									}

#define decode_modrm_src_16()		uint8_t modrm = bus->peek_byte(linear_addr_rm(cpu->cs, cpu->ip + 1)); \
									uint16_t base = calc_base(cpu, modrm); \
									uint16_t offset; \
									uint16_t value; \
									switch (MOD(modrm)){\
										case 3: \
											value = cpu_get_reg_16(cpu, RM(modrm)); \
											cpu->ip += 2; \
											break; \
										default: \
											offset = calc_modrm_addr(cpu, bus); \
											value = bus->peek_word(linear_addr_rm(base, offset)); \
											break; \
									}

#define alu_op_8(function, val)		value = function(cpu, value, val); \
									if (MOD(modrm) == 3){\
										cpu_set_reg_8(cpu, RM(modrm), value); \
									} else{\
										bus->poke_byte(linear_addr_rm(base, offset), value); \
									}

#define alu_op_16(function, val)	value = function(cpu, value, val); \
									if (MOD(modrm) == 3){\
										cpu_set_reg_16(cpu, RM(modrm), value); \
									}else{\
										bus->poke_word(linear_addr_rm(base, offset), value); \
									}

#define inc_16(val)					val++; \
									cpu_set_zf(cpu, val); \
									cpu_set_sf(cpu, val); \

#define dec_16(val)					val--; \
									cpu_set_zf(cpu, val); \
									cpu_set_sf(cpu, val); \

#define inc_8(val)					val++; \
									cpu_set_zf(cpu, val); \
									cpu_set_sf(cpu, (int16_t)(int8_t)val); \

#define dec_8(val)					val--; \
									cpu_set_zf(cpu, val); \
									cpu_set_sf(cpu, (int16_t)(int8_t)val); \

#define cjmp(cond)					uint16_t jump_target = calc_rel8_jmp(cpu, bus); \
									log_instruction(cpu, 0, "%04x\n", jump_target); \
									if (cond){ \
										cpu->ip = jump_target; \
									} else{ \
										cpu->ip += 2; \
									}

typedef struct{
	struct{
		union{
			struct{
				uint8_t al;
				uint8_t ah;
			};
			uint16_t ax;
		};
	};

	struct{
		union{
			struct{
				uint8_t bl;
				uint8_t bh;
			};
			uint16_t bx;
		};
	};

	struct{
		union{
			struct{
				uint8_t cl;
				uint8_t ch;
			};
			uint16_t cx;
		};
	};

	struct{
		union{
			struct{
				uint8_t dl;
				uint8_t dh;
			};
			uint16_t dx;
		};
	};

	uint16_t ss;
	uint16_t ds;
	uint16_t cs;
	uint16_t es;

	uint16_t si;
	uint16_t di;

	uint16_t ip;
	uint16_t sp;
	uint16_t bp;

	uint16_t flags;

	int running;
} regs_x86;

typedef struct{
	void(*poke_byte)(uint32_t, uint8_t);
	void(*poke_word)(uint32_t, uint16_t);
	uint8_t(*peek_byte)(uint32_t);
	uint16_t(*peek_word)(uint32_t);

	void(*out_byte)(uint16_t, uint8_t);
	void(*out_word)(uint16_t, uint16_t);
	uint8_t(*in_byte)(uint16_t);
	uint16_t(*in_word)(uint16_t);
} bus_x86;

void cpu_init(regs_x86*);
void cpu_dump(regs_x86*);
void cpu_step(regs_x86*, bus_x86*);
void cpu_int(regs_x86*, bus_x86*, uint8_t);
void cpu_iret(regs_x86*, bus_x86*);
void cpu_call_near(regs_x86*, bus_x86*, uint16_t);
void cpu_ret_near(regs_x86*, bus_x86*);
void cpu_call_far(regs_x86*, bus_x86*, uint16_t, uint16_t);
void cpu_ret_far(regs_x86*, bus_x86*);
void cpu_set_reg_8(regs_x86*, uint8_t, uint8_t);
void cpu_set_reg_16(regs_x86*, uint8_t, uint16_t);
void cpu_push_16(regs_x86*, bus_x86*, uint16_t);
uint16_t cpu_pop_16(regs_x86*, bus_x86*);
uint8_t cpu_get_reg_8(regs_x86*, uint8_t);
uint16_t cpu_get_reg_16(regs_x86*, uint8_t);
uint32_t linear_addr_rm(uint16_t, uint16_t);
void log_instruction(regs_x86*, int, char* fmt, ...);

extern int logging_instructions;
extern bus_x86* global_bus;