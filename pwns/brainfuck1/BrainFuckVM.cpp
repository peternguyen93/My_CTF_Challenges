#include "BrainFuckVM.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int optimize_opcode(char *bf_code)
{
	int c = 0;
	int i;
	for(i = 0; i < strlen(bf_code); i++){
		if((bf_code[i] != bf_code[i + 1]) || ((i + 1) >= strlen(bf_code)))
			break;
		c++;
	}
	return c + 1;
}

BrainFuckVM::BrainFuckVM()
{
	this->memory = new char[MEMORY_MAX];
	this->jit_page = (char *)mmap(0, JIT_PAGE, 
		PROT_READ | PROT_WRITE | PROT_EXEC,
		MAP_PRIVATE | MAP_ANONYMOUS, -1 , 0);

	this->jit_page_size = JIT_PAGE;
	this->memory_size = MEMORY_MAX;
	this->code_size = 0;
}

void BrainFuckVM::extend_jit_page()
{
	char *new_page;
	if((this->code_size + 100) >= this->jit_page_size){
		this->jit_page = (char *)mmap(this->jit_page, this->jit_page_size * 2, 
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_PRIVATE | MAP_ANONYMOUS, -1 , 0);
		this->jit_page_size *= 2;
	}
}

int BrainFuckVM::jit_add_addr(size_t add_op_length)
{
	char *code = this->jit_page + this->code_size;
	int retval = 0;
	if (add_op_length == 0){
		return 0;
	}

	extend_jit_page();

	if (add_op_length < 2){
		memcpy(code,"\x49\xff\xc6",3); // inc r14
		this->code_size += 3;
		retval = 1;
	}else{
		memcpy(code,"\x49\x81\xc6",3);
		memcpy(code + 3,(char *)&add_op_length,4); // add r14,value
		this->code_size += 7;
		retval = 1;
	}
	return retval;
}

int BrainFuckVM::jit_sub_addr(size_t sub_op_length)
{
	char *code = this->jit_page + this->code_size;
	int retval = 0;

	if (sub_op_length == 0){
		return 0;
	}

	if(sub_op_length < 2){
		memcpy(code,"\x49\xff\xce",3); //dec r14
		this->code_size += 3;
		retval = 1;
	}else{
		memcpy(code,"\x49\x81\xee",3); // sub r14,value
		memcpy(code + 3,(char *)&sub_op_length,4);
		this->code_size += 7;
		retval = 1;
	}
	return retval;
}

int BrainFuckVM::jit_add_value(size_t add_op_length)
{
	char *code = this->jit_page + this->code_size;
	int retval = 0;

	if (add_op_length == 0){
		return 0;
	}

	extend_jit_page();

	if(add_op_length < 2){
		memcpy(code,"\x41\xfe\x06",3); // inc [r14]
		this->code_size += 3;
		retval = 1;
	}else{
		memcpy(code,"\x41\x80\x06",3); // add [r14],value
		code[3] = (unsigned char)add_op_length;
		this->code_size += 4;
		retval = 1;
	}
	return retval;
}

int BrainFuckVM::jit_sub_value(size_t sub_op_length)
{
	char *code = this->jit_page + this->code_size;
	int retval = 0;

	if(sub_op_length == 0){
		return 0;
	}

	extend_jit_page();

	if(sub_op_length < 2){
		memcpy(code,"\x41\xfe\x0e",3); // dec [r14]
		this->code_size += 3;
		retval = 1;
	}else{
		memcpy(code,"\x41\x80\x2e",3); // sub [r14],value
		code[3] = (unsigned char)sub_op_length;
		this->code_size += 4;
		retval = 1;
	}
	return retval;
}

int BrainFuckVM::jit_cmp()
{
	char *code = this->jit_page + this->code_size;
	extend_jit_page();
	memcpy(code, "\x41\x0f\xb6\x06",4);
	memcpy(code + 4,"\x0f\xbf\xc0",3);
	memcpy(code + 7,"\x66\x85\xc0",3);
	this->code_size += 10;
	return 1;
}

int BrainFuckVM::jit_call_func(size_t func_addr,size_t argc)
{
	extend_jit_page();
	int retval = 0;
	char *code = this->jit_page + this->code_size;

	if (argc == 1){ // putchar
		/*
			movzx  edi,BYTE PTR [r14]
			movsx  edi,di
			mov    r12d,value
			call   r12
		*/
		memcpy(code,"\x41\x0f\xb6\x3e\x0f\xbf\xff",7);
		memcpy(code + 7,"\x49\xbc",2);memcpy(code + 9,(char *)&func_addr,8);
		memcpy(code + 17,"\x41\xff\xd4",3);
		this->code_size += 20;
		retval = 1;
	}else if(argc == 0){ // getchar
		/*
			mov    r12d,value
			call   r12
			mov    BYTE PTR [r14],al
		*/
		memcpy(code,"\x49\xbc",2);memcpy(code + 2,(char *)&func_addr,8);
		memcpy(code + 10,"\x41\xff\xd4",3);
		memcpy(code + 13,"\x41\x88\x06",3);
		this->code_size += 16;
		retval = 1;
	}
	return retval;
}

int BrainFuckVM::translate_code(char *bf_code,size_t bf_code_size)
{
	size_t *stack_offset = new size_t[256];
	size_t i = 0;
	size_t l_code = 0;
	size_t nested_loop_count = 0;
	size_t func_call;

	memcpy(this->jit_page,JIT_BEGIN,11);
	this->code_size += 11;

	while(i < bf_code_size){
		switch(bf_code[i]){
			case '<':
				l_code = optimize_opcode(bf_code + i);
				if(!jit_sub_addr(l_code)){
					fputs("An error occurred while translating code into JIT",stderr);
					exit(1);
				}
				i += l_code;
				break;
			case '>':
				l_code = optimize_opcode(bf_code + i);
				if(!jit_add_addr(l_code)){
					fputs("An error occurred while translating code into JIT",stderr);
					exit(1);
				}
				i += l_code;
				break;
			case '+':
				l_code = optimize_opcode(bf_code + i);
				if(!jit_add_value(l_code)){
					fputs("An error occurred while translating code into JIT",stderr);
					exit(1);
				}
				i += l_code;
				break;
			case '-':
				l_code = optimize_opcode(bf_code + i);
				if(!jit_sub_value(l_code)){
					fputs("An error occurred while translating code into JIT",stderr);
					exit(1);
				}
				i += l_code;
				break;
			case ',':
				func_call = (size_t)getchar;
				if(!jit_call_func(func_call,0)){
					fputs("An error occurred while translating code into JIT",stderr);
					exit(1);
				}
				i++;
				break;
			case '.':
				func_call = (size_t)putchar;
				if(!jit_call_func(func_call,1)){
					fputs("An error occurred while translating code into JIT",stderr);
					exit(1);
				}
				i++;
				break;
			case '[':
				if(nested_loop_count > 255){
					fputs("An error occurred while translating code into JIT",stderr);
					exit(1);
				}
				stack_offset[nested_loop_count++] = this->code_size;
				i += 1;
				break;
			case ']':
				if(stack_offset[nested_loop_count - 1]){
					size_t current_offset;
					size_t distance;

					jit_cmp();
					current_offset = this->code_size;
					distance = stack_offset[nested_loop_count - 1] - current_offset;

					// jne opcode is the same on x86 and x86_64
					if(distance >= -128 && distance < 127){
						distance -= 2;
						this->jit_page[current_offset] = 0x75; //jne 1 byte
						this->jit_page[current_offset + 1] = (char)distance;
						this->code_size += 2;
					}else{
						distance -= 6;
						memcpy(this->jit_page + current_offset,"\x0f\x85",2); //jne 4 bytes
						memcpy(this->jit_page + current_offset + 2,(char *)&distance,4);
						this->code_size += 6;
					}

					stack_offset[nested_loop_count - 1] = 1;
					nested_loop_count--;
				}
				i++;
				break;
			default:
				i++;
		}
	}

	extend_jit_page();
	memcpy(this->jit_page + this->code_size,JIT_END,6);
	delete[] stack_offset;

	return 0;
}

void BrainFuckVM::run()
{
	void (*jit_run)(char *) = (void (*)(char *))this->jit_page;
	
	jit_run(this->memory);

	this->code_size = 0; // reset
	memset(this->jit_page,0,this->jit_page_size);
}

BrainFuckVM::~BrainFuckVM()
{
	delete this->memory;
	munmap(this->jit_page,this->jit_page_size);
}