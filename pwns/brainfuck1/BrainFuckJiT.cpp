#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "BrainFuckVM.h"

using namespace std;

size_t read_str(char **buf)
{
	char b;
	size_t i = 0;
	size_t buff_size = 256;

	*buf = new char[buff_size + 1];

	while(1)
	{
		if(i + 16 >= buff_size){
			buff_size *= 2;
			char *tmp = new char[buff_size + 1];
			memcpy(tmp,*buf,i);
			delete *buf;
			*buf = tmp;
		}

		read(0,&b,1);

		if(b == '\n'){
			break;
		}

		(*buf)[i++] = b;
	}

	return i + 1;
}

int main()
{
	BrainFuckVM *vm = new BrainFuckVM();
	char *buf;
	size_t str_len;

	setvbuf(stdin , NULL , _IONBF , 0 );
	setvbuf(stdout , NULL , _IONBF , 0 );

	cout << "* Peter's BrainFuck JIT VM *\n";

	while(1)
	{
		cout << ">> ";
		str_len = read_str(&buf);

		if(memcmp(buf,"exit",4) == 0){
			break;
		}

		vm->translate_code(buf,str_len);
		vm->run();

		delete buf;
	}

	cout << "Bye." << endl;
	delete vm;

	return 0;
}