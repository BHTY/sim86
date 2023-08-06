// sim86.cpp : Defines the entry point for the console application.
//

//Add macros to generalize
//- modrm switchcases
//- reg switchcases

//Fix REP instructions when CX=0!

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

#include "cpu.h"
#include "sim86.h"

int single_step = 0;
int double_step = 0;

uint32_t lba = 0;
uint16_t current_sector[256];
uint8_t current_sector_ptr;
uint8_t current_disk_mode;
uint8_t current_seek_status = 0;
#define MODE_SEEK 0
#define MODE_READ 1
#define MODE_WRITE 2
#define MODE_SWITCH 3

uint8_t* memory;
//uint8_t* init_program = (uint8_t*)"\xB0\x41\xB4\x1A\xE6\x00\xFE\xC0\xFE\xCC\x74\xF4\xEB\xF6";
//uint8_t* init_program = (uint8_t*)"\xB0\x40\x8E\xD8\x8B\x35\x15\x00\x8A\x06\x3C\x00\x74\x05\xE6\x00\x46\xEB\xF5\xCC\xFEHello, world from sim86!\n"; //EBFE
//uint8_t* init_program = (uint8_t*)"\xB0\x40\x8E\xD8\x8B\x35\x15\x00\x8A\x06\x3C\x00\x74\x05\xE6\x00\x46\xEB\xF5\xCC\xFEThe IBM Personal Computer DOS\nVersion 1.10 (C)Copyright IBM Corp 1981, 1982\n"; //EBFE
//uint8_t* init_program = (uint8_t*)"\xB0\x01\xB3\x02\x50\x53\xE8\x05\x00\x83\xC4\x04\xEB\xFE\x55\x89\xE5\x8B\x45\x04\x03\x45\x06\x5D\xC3";
//uint8_t init_program[] = { 129, 252, 0, 1, 116, 1, 244, 188, 0, 16, 176, 46, 187, 0, 0, 75, 131, 251, 255, 117, 241, 232, 81, 1, 67, 117, 235, 232, 75, 1, 49, 201, 9, 203, 117, 226, 114, 224, 232, 64, 1, 185, 0, 128, 57, 217, 118, 214, 232, 54, 1, 1, 203, 114, 207, 232, 47, 1, 1, 219, 131, 209, 0, 121, 197, 114, 195, 81, 131, 225, 1, 116, 189, 232, 29, 1, 89, 249, 187, 0, 128, 25, 217, 117, 177, 114, 175, 232, 15, 1, 232, 0, 0, 91, 129, 251, 93, 0, 117, 162, 129, 252, 0, 16, 117, 156, 232, 252, 0, 187, 114, 0, 83, 195, 129, 252, 0, 16, 117, 142, 232, 238, 0, 144, 144, 144, 235, 1, 244, 232, 248, 0, 184, 197, 1, 232, 202, 0, 232, 239, 0, 176, 48, 232, 215, 0, 254, 192, 60, 127, 117, 247, 176, 35, 199, 6, 211, 1, 144, 1, 177, 80, 232, 196, 0, 254, 201, 117, 249, 129, 62, 211, 1, 224, 1, 117, 10, 177, 80, 199, 6, 211, 1, 128, 7, 235, 231, 199, 6, 211, 1, 224, 1, 177, 18, 232, 163, 0, 232, 160, 0, 131, 6, 211, 1, 76, 232, 152, 0, 232, 149, 0, 254, 201, 117, 235, 199, 6, 211, 1, 52, 2, 49, 192, 186, 1, 0, 185, 17, 0, 1, 194, 232, 159, 0, 80, 184, 32, 0, 232, 119, 0, 88, 146, 73, 117, 239, 199, 6, 211, 1, 212, 2, 185, 0, 0, 137, 200, 232, 63, 0, 232, 130, 0, 184, 32, 0, 232, 91, 0, 65, 131, 249, 20, 118, 236, 199, 6, 211, 1, 116, 3, 187, 2, 0, 128, 143, 213, 1, 0, 117, 25, 137, 216, 232, 97, 0, 184, 32, 0, 232, 58, 0, 137, 223, 128, 141, 213, 1, 1, 1, 223, 131, 255, 101, 118, 244, 67, 131, 251, 100, 118, 218, 244, 137, 195, 49, 210, 9, 219, 116, 5, 1, 194, 75, 235, 249, 137, 208, 195, 83, 82, 137, 195, 138, 23, 67, 134, 194, 232, 9, 0, 134, 194, 32, 210, 117, 242, 90, 91, 195, 83, 87, 187, 0, 128, 139, 62, 211, 1, 136, 1, 71, 137, 62, 211, 1, 95, 91, 195, 139, 62, 211, 1, 131, 239, 80, 121, 251, 41, 62, 211, 1, 195, 83, 80, 179, 48, 131, 248, 9, 118, 42, 131, 248, 99, 118, 19, 131, 232, 100, 254, 195, 131, 248, 99, 119, 246, 134, 216, 232, 194, 255, 134, 216, 179, 48, 131, 248, 9, 118, 6, 131, 232, 10, 67, 235, 245, 134, 195, 232, 174, 255, 136, 216, 4, 48, 232, 167, 255, 88, 91, 195, 72, 101, 108, 108, 111, 44, 32, 119, 111, 114, 108, 100, 33, 0, 0, 0 };
//uint8_t init_program[] = { 129, 252, 0, 1, 116, 1, 244, 188, 0, 16, 176, 46, 187, 0, 0, 75, 131, 251, 255, 117, 241, 232, 81, 1, 67, 117, 235, 232, 75, 1, 49, 201, 9, 203, 117, 226, 114, 224, 232, 64, 1, 185, 0, 128, 57, 217, 118, 214, 232, 54, 1, 1, 203, 114, 207, 232, 47, 1, 1, 219, 131, 209, 0, 121, 197, 114, 195, 81, 131, 225, 1, 116, 189, 232, 29, 1, 89, 249, 187, 0, 128, 25, 217, 117, 177, 114, 175, 232, 15, 1, 232, 0, 0, 91, 129, 251, 93, 0, 117, 162, 129, 252, 0, 16, 117, 156, 232, 252, 0, 187, 114, 0, 83, 195, 129, 252, 0, 16, 117, 142, 232, 238, 0, 144, 144, 144, 235, 1, 244, 232, 248, 0, 184, 197, 5, 232, 202, 0, 232, 239, 0, 176, 48, 232, 215, 0, 254, 192, 60, 127, 117, 247, 176, 35, 199, 6, 211, 1, 144, 1, 177, 80, 232, 196, 0, 254, 201, 117, 249, 129, 62, 211, 1, 224, 1, 117, 10, 177, 80, 199, 6, 211, 1, 128, 7, 235, 231, 199, 6, 211, 1, 224, 1, 177, 18, 232, 163, 0, 232, 160, 0, 131, 6, 211, 1, 76, 232, 152, 0, 232, 149, 0, 254, 201, 117, 235, 199, 6, 211, 1, 52, 2, 49, 192, 186, 1, 0, 185, 17, 0, 1, 194, 232, 159, 0, 80, 184, 32, 0, 232, 119, 0, 88, 146, 73, 117, 239, 199, 6, 211, 1, 212, 2, 185, 0, 0, 137, 200, 232, 63, 0, 232, 130, 0, 184, 32, 0, 232, 91, 0, 65, 131, 249, 20, 118, 236, 199, 6, 211, 1, 116, 3, 187, 2, 0, 128, 143, 213, 1, 0, 117, 25, 137, 216, 232, 97, 0, 184, 32, 0, 232, 58, 0, 137, 223, 128, 141, 213, 1, 1, 1, 223, 131, 255, 101, 118, 244, 67, 131, 251, 100, 118, 218, 244, 137, 195, 49, 210, 9, 219, 116, 5, 1, 194, 75, 235, 249, 137, 208, 195, 83, 82, 137, 195, 138, 23, 67, 134, 194, 232, 9, 0, 134, 194, 32, 210, 117, 242, 90, 91, 195, 83, 87, 187, 0, 128, 139, 62, 211, 1, 136, 1, 71, 137, 62, 211, 1, 95, 91, 195, 139, 62, 211, 1, 131, 239, 80, 121, 251, 41, 62, 211, 1, 195, 83, 80, 179, 48, 131, 248, 9, 118, 42, 131, 248, 99, 118, 19, 131, 232, 100, 254, 195, 131, 248, 99, 119, 246, 134, 216, 232, 194, 255, 134, 216, 179, 48, 131, 248, 9, 118, 6, 131, 232, 10, 67, 235, 245, 134, 195, 232, 174, 255, 136, 216, 4, 48, 232, 167, 255, 88, 91, 195, 72, 101, 108, 108, 111, 44, 32, 119, 111, 114, 108, 100, 33, 0, 0, 0 };
//uint8_t init_program[] = { 191, 0, 0, 199, 5, 18, 0, 191, 2, 0, 184, 64, 0, 137, 5, 205, 0, 244, 80, 176, 65, 230, 0, 88, 207 };
uint8_t* init_program;
uint8_t current_key = 0;

FILE* disk_fp;
HDD_DEVICE floppy;

uint8_t mem_read_8(uint32_t addr){
	return *(memory + addr);
}

uint16_t mem_read_16(uint32_t addr){
	return *(uint16_t*)(memory + addr);
}

void mem_write_8(uint32_t addr, uint8_t val){
	*(memory + addr) = val;
}

void mem_write_16(uint32_t addr, uint16_t val){
	*(uint16_t*)(memory + addr) = val;
}

uint8_t io_read_8(uint16_t port){
	if (port == 0x60){
		return current_key;
	}

	return 0;
}

uint16_t io_read_16(uint16_t port){
	//printf("Reading from port %x\n", port);
	uint16_t value;

	switch (port){
		case 0x03:
			value = current_sector[current_sector_ptr];
			current_sector_ptr++;
			break;
		default:
			break;
	}

	return value;
}

void io_write_8(uint16_t port, uint8_t val){
	switch (port){
		case 0x00:
			printf("%c", val);
			break;
		case 0x02:
			switch (val){
				case MODE_SEEK:
					current_disk_mode = MODE_SEEK;
					current_seek_status = 0;
					break;
				case MODE_READ:
					fseek(disk_fp, lba, SEEK_SET);
					fread(current_sector, 1, 512, disk_fp);
					current_sector_ptr = 0;
					current_disk_mode = MODE_READ;
					break;
			}
			break;
		case 0x03:
			switch (current_disk_mode){
				case MODE_SEEK:
					if (current_seek_status < 3){
						switch (current_seek_status){
							case 0:
								lba = val * floppy.tracks_per_cylinder * floppy.sectors_per_track * BYTES_PER_SECTOR;
								break;
							case 1:
								lba += val * floppy.sectors_per_track * BYTES_PER_SECTOR;
								break;
							case 2:
								lba += (val - 1) * BYTES_PER_SECTOR;
								printf("Seeking to linear block address %d (byte offset %05x)\n", lba >> 9, lba);
								break;
						}

						current_seek_status++;
					}
					break;
			}
			break;
		default:
			break;
	}
}

void io_write_16(uint16_t port, uint16_t val){
}

void create_machine(regs_x86* cpu, bus_x86* bus){
	memory = (uint8_t*)malloc(1024 * 1024);
	memset(memory, 0, 1024 * 1024);
	cpu_init(cpu);

	bus->peek_byte = mem_read_8;
	bus->peek_word = mem_read_16;
	bus->poke_byte = mem_write_8;
	bus->poke_word = mem_write_16;

	bus->in_byte = io_read_8;
	bus->in_word = io_read_16;
	bus->out_byte = io_write_8;
	bus->out_word = io_write_16;

	memory[0xffff0] = 0xea;
	memory[0xffff1] = 0x00;
	memory[0xffff2] = 0x7c;
	memory[0xffff3] = 0x00; //40
	memory[0xffff4] = 0x00;
	//memcpy(memory + 1024, init_program, 25);  //49 / 98 / 474

	//init_program = (uint8_t*)malloc(512);
	FILE* fp = fopen("c:\\users\\will\\bios", "rb");
	fread(memory + 0xf0000, 1, 65536, fp);
	fclose(fp);

	//disk_fp = fopen("c:\\users\\will\\desktop\\disk01.img", "rb");
	disk_fp = fopen("c:\\users\\will\\dos20.img", "rb");
	//disk_fp = fopen("dump.bin", "rb");

	//memcpy(memory + 0x7c00, init_program, 512);
	floppy.cylinders = 40;
	floppy.tracks_per_cylinder = 1;
	floppy.sectors_per_track = 9;
}

int main(int argc, char* argv[])
{
	single_step = 0;
	double_step = 0;

	regs_x86 registers;
	bus_x86 bus;

	create_machine(&registers, &bus);

	while (registers.running){
		
		if (single_step){ 
			cpu_dump(&registers); 
		}
		cpu_step(&registers, &bus);

		if (registers.cs == 0x70){
			single_step = 1;
			//double_step = 1;
		}

		if (registers.cs == 0x113){ //idk why this ends up getting blanked out or why 0x9f85:0x009a gets altered
			single_step = 1;
			double_step = 1;
		}

		if (double_step){ getch(); }
		/*if (registers.ip == 0x7C00){
			//cpu_dump(&registers);
			//registers.running = 0;
			double_step = 1;
			single_step = 1;
		}*/

		if (kbhit()){
			cpu_int(&registers, &bus, 9);
			current_key = getch();
		}
	}

	/*for (int i = 0x500; i < 0x700; i++){
		printf("%c", mem_read_8(i));
	}*/

	cpu_dump(&registers);
	
	printf("0x2130: ");
	for (int i = 0; i < 16; i++){
		printf("%02x ", mem_read_16(0x2130 + i));
	}

	printf("\n");

	FILE* fp = fopen("dump.bin", "wb");
	fwrite(memory, 1, 1024 * 1024, fp);
	fclose(fp);

	uint32_t vram_ptr = 0x8000;

	for (int i = 0; i < 25; i++){
		for (int p = 0; p < 80; p++){
			printf("%c", mem_read_8(vram_ptr));
			vram_ptr++;
		}
		printf("\n");
	}

	while (1);

	return 0;
}

