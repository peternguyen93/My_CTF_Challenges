/*
*	BrainFuck JIT Compiler
*	Author : peternguyen
*/
#include <stddef.h>

#define MEMORY_MAX 256
#define JIT_PAGE 0x1000
/*
	push rbp
	mov rbp,rsp
	sub rsp,0x20
	mov r14,rdi; ptr pointer
*/
#define JIT_BEGIN "\x55\x48\x89\xe5\x48\x83\xec\x20\x49\x89\xfe"
/*
	add    rsp,0x20
	pop    rbp
	ret    
*/
#define JIT_END "\x48\x83\xc4\x20\x5d\xc3"

class BrainFuckVM
{
	char *memory; // memory religion that store cell of code
	char *jit_page; // page store asm code
	size_t memory_size;
	size_t code_size; // code size
	size_t jit_page_size;

public:
	BrainFuckVM();
	int translate_code(char *bf_code,size_t bf_code_size);
	void extend_jit_page();
	int jit_cmp();
	int jit_call_func(size_t func_addr,size_t argc);
	int jit_sub_value(size_t sub_op_length);
	int jit_add_value(size_t add_op_length);
	int jit_sub_addr(size_t sub_op_length);
	int jit_add_addr(size_t add_op_length);
	void run();

	~BrainFuckVM();
};