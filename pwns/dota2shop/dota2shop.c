#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define INT_OVERFLOW(num,start,end) \
	if(num < start || num > end) {printf("Nah, Integer Overflow ?"); exit(-1);}

typedef struct my_cart_item
{
	int id;
	int item_id;
	int use_item;
	int damage;
	float rate;
	char *descr;
	int (*use)(struct my_cart_item *);
	void (*show)(struct my_cart_item *);
}my_cart_item;

char *dota2items[] = {
	"Blade of Alacariy",
	"Echo Sabre",
	"Dagon",
	"Shadow Blade",
	"Blade Mail",
	"Battle Furry",
	"Armlet",
	"Butterfly",
	"Monkey King Bar",
	"Divine Rapier",
	NULL
};
float prices[] = {1000,2200,2720,2700,2200,4500,2370,5525,5200,6000};
int items_damage[] = {10,40,80,20,100,30,110,120,150,200};

my_cart_item **my_carts = NULL;
int my_carts_count = 0;
float my_money = 1337.37;

char *creeps[] = {
	"Natural Creep",
	"Centaurs",
	"Ursa",
	"Chim",
	"ML",
	"Rosan",
	NULL
};
int creeps_hp[] = {40,80,100,140,200,10000};

int use_item(my_cart_item *this)
{
	return this->use_item--;
}

void show_item(my_cart_item *this)
{
	printf("+ %d : '%s'\n",this->id + 1,dota2items[this->item_id]);
	printf("\t -> Description: %s\n",this->descr);
}

char* read_str(int size)
{
	char *buf;
	char c;
	int i = 0;
	
	buf = (char *)malloc(size);
	memset(buf,0,size);

	while(i < size){
		if(read(0,&c,1) < 0){ fprintf(stderr,"read error\n"); exit(-1);}
		if(c == 10) break;

		buf[i++] = c;
	}

	return buf;
}

int read_int()
{
	char *input;
	int i;

	loop:
	input = read_str(16);
	for(i = 0; i < strlen(input); i++){
		if(input[i] > '9' || input[i] < '0'){
			printf("Input number pls\n");
			free(input);
			goto loop;
		}
	}

	int num = atoi(input);
	free(input);
	return num;
}

void view_items()
{
	char **p;
	int c = 0;

	puts("============ Dota 2 Items ============");

	for(p = dota2items; *p != NULL; p++,c++){
		printf("+ %d.\t'%s' - %.2f\n",c + 1,*p,prices[c]);
	}
	puts("======================================");
}

void view_my_carts()
{
	int i = 0;

	puts("============ My Carts ============");
	for(i = 0; i < my_carts_count; i++){
		if(my_carts[i]) my_carts[i]->show(my_carts[i]);
	}	
}

void add_to_carts()
{
	my_carts = (my_cart_item **)realloc(my_carts,(my_carts_count + 1)*sizeof(my_cart_item *));
	my_carts_count += 1;

	view_items();
	printf("Your money : %.2f\n",my_money);
	printf("Which item do you want to buy ?");
	int op = read_int();

	INT_OVERFLOW(op,1,9)
	// printf("%f - %f\n",my_money,prices[op - 1]);
	if(my_money < prices[op - 1]){
		printf("You don't have money to buy this item\n");
		return;
	}else{
		my_money -= prices[op - 1];
	}

	my_carts[my_carts_count - 1] = (my_cart_item *)malloc(sizeof(my_cart_item));
	my_carts[my_carts_count - 1]->id = my_carts_count - 1;
	my_carts[my_carts_count - 1]->item_id = op - 1;
	my_carts[my_carts_count - 1]->damage = items_damage[op - 1];
	printf("Do you want to save a short description about this item? (y/n) ");
	char *buf;
	do{
		buf = read_str(16);
	}while(buf[0] != 'y');
	free(buf);

	printf("How many bytes do you want to send? ");
	int send_bytes = read_int();

	char *descr = read_str(send_bytes);
	my_carts[my_carts_count - 1]->descr = descr;

	my_carts[my_carts_count - 1]->show = show_item;
	my_carts[my_carts_count - 1]->use = use_item;

	switch(op)
	{
		case 1:
			my_carts[my_carts_count - 1]->rate = 4.1;
			my_carts[my_carts_count - 1]->use_item = 4;
			break;
		case 2:
			my_carts[my_carts_count - 1]->rate = 4.2;
			my_carts[my_carts_count - 1]->use_item = 8;
			break;
		case 3:
			my_carts[my_carts_count - 1]->rate = 12.5;
			my_carts[my_carts_count - 1]->use_item = 6;
			break;
		case 4:
			my_carts[my_carts_count - 1]->rate = 14.8;
			my_carts[my_carts_count - 1]->use_item = 12;
			break;
		case 5:
			my_carts[my_carts_count - 1]->rate = 16.8;
			my_carts[my_carts_count - 1]->use_item = 10;
			break;
		case 6:
			my_carts[my_carts_count - 1]->rate = 18.1;
			my_carts[my_carts_count - 1]->use_item = 20;
			break;
		case 7:
			my_carts[my_carts_count - 1]->rate = 20.9;
			my_carts[my_carts_count - 1]->use_item = 100;
			break;
	}

	puts("Done.");
}

void sell_item()
{
	int i;
	view_my_carts();

	printf("Which item do you want to sell ?");
	int id = read_int();

	INT_OVERFLOW(id,1,my_carts_count)

	float price = prices[my_carts[id - 1]->item_id];
	float rate = my_carts[id - 1]->rate;

	my_money += round(price - (price * rate)/100 + (my_carts[id - 1]->use_item * rate)/100);
	printf("Your money : %.2f\n",my_money);

	free(my_carts[id - 1]->descr);
	free(my_carts[id - 1]);

	for(i = id - 1; i < my_carts_count; i++){
		my_carts[i] = my_carts[i + 1];
	}
	my_carts_count -= 1;
	puts("Done.");
}

void earn_money()
{
	view_my_carts();
	puts("Which item do you want to use ?");
	int op = read_int();

	INT_OVERFLOW(op,1,my_carts_count)

	my_cart_item *p = my_carts[op - 1];
	if(p->use_item < 0){
		puts("Your item is broken");
		return;
	}

	srand(time(0));
	int idx = rand() % 6;
	
	printf("Your enemy '%s' - HP : %d\n",creeps[idx],creeps_hp[idx]);
	int creep_hp = creeps_hp[idx];
	while(p->use(p)){
		creep_hp -= p->damage;
	}

	if(creep_hp < 0){
		my_money += round(p->rate * creeps_hp[idx]);
	}else{
		my_money += round(p->rate * (creeps_hp[idx] - creep_hp));
	}
	printf("Your money: %.2f\n",my_money);
}


int main()
{
	setvbuf(stdin , NULL , _IONBF , 0 );
	setvbuf(stdout , NULL , _IONBF , 0 );

	alarm(80);

	while(1)
	{
		puts("++++++++ Wellcome to My Dota2 Shop ++++++++");
		printf("Your money: %.2f\n",my_money);
		puts("1. View our items.");
		puts("2. View your carts");
		puts("3. Buy our items.");
		puts("4. Sell to us.");
		puts("5. Earn money.");
		puts("6. Exit");

		int op = read_int();
		if(op == 6) break;

		switch(op)
		{
			case 1:
				view_items();
				break;
			case 2:
				if(my_carts_count == 0) printf("You have no item in your carts\n");
				else
					view_my_carts();
				break;
			case 3:
				add_to_carts();
				break;
			case 4:
				if(my_carts_count == 0) printf("Buy my items pls.\n");
				else
					sell_item();
				break;
			case 5:
				if(my_carts_count == 0) printf("Buy my items pls.\n");
				else
					earn_money();
				break;
		}
	}
	return 0;
}