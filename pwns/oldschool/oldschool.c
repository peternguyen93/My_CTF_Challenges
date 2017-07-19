#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <unistd.h>

#define MAX 16

typedef unsigned int uint32_t;

typedef struct Book
{
	char name[32];
	char author[32];
	uint32_t descr_length;
	char descr[];
}Book;

int read_int()
{
	char buf[16] = {0};
	fgets(buf,15,stdin);
	return atoi(buf);
}

int show_menu()
{
	char buf[16] = {0};

	puts("-------- BookStore --------");
	puts("1.\t Add Book.");
	puts("2.\t Edit Book.");
	puts("3.\t Delete Book.");
	puts("4.\t Show Books.");
	puts("5.\t Exit.");
	printf("Choice:");
	
	fgets(buf,15,stdin);

	return atoi(buf);
}

void show_books(Book **books,uint32_t size)
{
	int i = 0;
	for(i = 0; i < size; i++){
		printf("------------ %d ------------\n", i + 1);
		printf("Name: %s\n", books[i]->name);
		printf("Author: %s\n", books[i]->author);
		printf("Description: %s\n", books[i]->descr);
	}
}

void read_str(char *_str, uint32_t size)
{
	int i = 0;
	char c;

	for(i = 0; i < size; i++){
		if(read(0,&c,1) < 0){
			puts("Error read()");
			exit(-1);
		}
		if(c == 10) break;
		_str[i] = c;
	}
}


int main()
{
	Book *books[MAX] = {0};
	uint32_t book_size = 0;
	char _break = 0;
	int length;
	int idx,i;
	char buf1[32] = {0};
	char buf2[32] = {0};

	setvbuf(stdin , NULL , _IONBF , 0 );
	setvbuf(stdout , NULL , _IONBF , 0 );

	alarm(60);

	while(!_break){
		int op = show_menu();
		switch(op){
			case 1:
				if(book_size < MAX){
					printf("Book name:"); read_str(buf1,32);
					printf("Author:"); read_str(buf2,32);
					printf("Length of Description:"); length = read_int();
					if(length < 0 || length > 256){
						puts("Integer Overflow");
						exit(-1);
					}

					Book *book = alloca(sizeof(Book) + length);
					book->descr_length = length;

					printf("Description:"); read_str(book->descr,length);
					strcpy(book->name,buf1);
					strcpy(book->author,buf2);
					book->descr[length] = '\x00';
					
					books[book_size++] = book;
				}else{
					puts("Can't add more books.");
				}
				break;
			case 2:
				if(book_size > 0){
					show_books(books,book_size);
					printf("Which book do you want to edit ?"); idx = read_int() - 1;
					if(idx < 0 || idx > book_size){
						puts("Integer Overflow");
						exit(-1);
					}
					printf("Book name:");
					read_str(books[idx]->name,strlen(books[idx]->name));
					printf("Author:");
					read_str(books[idx]->author,strlen(books[idx]->author));

					length = books[idx]->descr_length;
					printf("Description:"); read_str(books[idx]->descr,length);
				}else{
					puts("Add more books pls.");
				}
				break;
			case 3:
				if(book_size > 0){
					show_books(books,book_size);
					printf("Which book do you want to delete ?"); idx = read_int() - 1;
					if(idx < 0 || idx > book_size){
						puts("Integer Overflow");
						exit(-1);
					}

					for(i = idx; i < book_size - 1; i++){
						books[i] = books[i + 1];
					}
					book_size--;
				}else{
					puts("You have no book to delete.");
				}
				break;
			case 4:
				show_books(books,book_size);
				break;
			default:
				_break = 1;
				break;

		}
	}

	return 0;
}