/*
 *  Task 2 
 *
 *  Created by Vitaliy Rakitin in 2016 
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_SIZE 5 


struct list{
	int type; // 0 --- +, 1 --- * breakets, 2 --- строка
	size_t size;
	int times;
	char *string;
	struct list *right;
	struct list *left;
};

struct list* create_list_str(const char *str,  int times, int *errflag){
	struct list *str_list;
	
	str_list = (struct list*)malloc(sizeof(struct list));
	if (str_list == NULL){*errflag = 1; return 0;}

	str_list->type = 2;
	str_list->size = strlen(str);
	str_list->times = 1;
	
	str_list->string = (char*)malloc((str_list->size * times + 2) * sizeof(char));
	if (str_list->string == NULL){*errflag = 1; return 0;}

	str_list->string[0] = '\0';
	for (int i=0;i<times;i++)
		str_list->string = strcat(str_list->string,str);
	
	str_list->left = NULL;
	str_list->right = NULL;
	return str_list;
}

struct list* create_list_plus(struct list* left_tree, int *errflag){
	struct list *str_list;
	str_list = (struct list*)malloc(sizeof(struct list));
	if (str_list == NULL){*errflag = 1; return 0;}
	str_list->type = 0;
	str_list->size = 0;
	str_list->times = 0;	
	str_list->string = NULL;
	str_list->left = left_tree;
	str_list->right = NULL;
	return str_list;
}

struct list* create_list_multy(struct list* left_tree,  int times, int *errflag){
	struct list *str_list;
	str_list = (struct list*)malloc(sizeof(struct list));
	if (str_list == NULL){*errflag = 1; return 0;}
	str_list->type = 1;
	str_list->size = 0;
	str_list->times = times;
	str_list->string = NULL;
	str_list->left = left_tree;
	str_list->right = NULL;
	return str_list;
}


void print_list(struct list *str_list){
	if (str_list->string!=NULL)
		printf("%s\n%d\n", str_list->string,str_list->times);
}

void print_tree(struct list *tree){
	if (tree->left != NULL)
		print_tree(tree->left);
	if (tree->right!=NULL)
		print_tree(tree->right);
	print_list(tree);
}

void free_list(struct list*str_list){
	if (str_list->string != NULL)
		free(str_list->string);
	if (str_list != NULL)
		free(str_list);
}

void free_tree(struct list *tree){
	if (tree->left != NULL)
		free_tree(tree->left);
	if (tree->right!=NULL)
		free_tree(tree->right);
	free_list(tree);
}

char* read_string(int *errflag, size_t *size){
	char a;
	int times = 1;
    char *line = (char*)malloc(MAX_SIZE * sizeof(char));
    if (line == NULL) *errflag = 1;	
    *size = 0;

	while((*errflag == 0)&&((a = fgetc(stdin))!= EOF)){
        if ((a == 10)||(a==0)){ // \n или \0            
        	line[*size] = '\0';
        	break;
        }
        else{
        line[*size] = a;
        (*size)++;
        }

        if (*size >= times*MAX_SIZE-1){
            times++;
            line = (char*)realloc(line, (MAX_SIZE*times)*sizeof(char));
            if (line == NULL) {*errflag = 1;break;}
        }
    }
    return line;
}


/* функция получения числа из строки с текущей позиции */
int getnum(char *str,int *counter,  int *errflag){
    int num = 0; // наше число
    int check_out = 1;  // пометка для выхода
    char character;
	size_t count;
    for (count = 0; (sscanf(str + count + *counter + 1,"%c",&character)==1)&&(*errflag == 0)&&(check_out==1); count++)
        switch(character){
            case '0': num = num*10;     break;
            case '1': num = num*10 + 1; break;
            case '2': num = num*10 + 2; break;
            case '3': num = num*10 + 3; break;     
            case '4': num = num*10 + 4; break;
            case '5': num = num*10 + 5; break;
            case '6': num = num*10 + 6; break;
            case '7': num = num*10 + 7; break;
            case '8': num = num*10 + 8; break;
            case '9': num = num*10 + 9; break;
            case ' ': if (num != 0)  check_out = 0; break;  /* пробел должен быть только перед числом */      
            default: 
            	if (num == 0) *errflag = 4; 
            	check_out = 0; break;
        }
   
    *counter+= count;
    return num;
}

struct list* create_tree(char *str, size_t size, int *errflag){
	char character;
	struct list* head;
	struct list* cur;
	int open_quotes = 0;
	size_t first,final;
	char *subline;

	head = create_list_plus(NULL, errflag);
	cur = head;
	subline = (char*)malloc(size*sizeof(char));
	if ((subline == NULL)||(*errflag!=0)) {*errflag = 1; return head;}

	subline[0] = '\0';

	for (int counter = 0;(counter < size)&&(*errflag == 0);counter++){
        /* считываем элемент */
        sscanf(str+counter,"%c",&character);

        /* если open_quotes == 1, то это означает, что мы живём внутри открытых ковычек */
        if (open_quotes == 0)
            switch(character){
                case '"': first = counter+1; open_quotes = 1; break; // открыли кавычку, сохранили позицию первого элемента внутри
                case '+': 
                	if (cur->left != NULL){
                		cur->right = create_list_plus(NULL, errflag);
              	  		cur = cur->right;}
              	  		else *errflag = 2;
                	break; /* если скобка закрылась перед "+", то можно про неё забыть */
            	case '*':
            		if (cur->left != NULL){
            			cur->left->times = getnum(str,&counter,errflag);
            		}
            		else *errflag = 5;
                case ' ': break; /* нам нет разницы до пробелов */
                default:
                    *errflag = -1;
                    //counter--; /* мы только что считали число, а нам нужно учесть его, то есть считать ещё раз в getnum */
                    //num = getnum(&counter, str); // если тут пришла цифра, то считаем начнём считывать число с позиции этой цифры
                    //if (num < 0) errflag = num;
                    break;
            }
        else
        {   /* тут мы окажемся, если живёмвнутри ковычек
             * пока они открыты нам неважно, что внутри: там просто строка */
            if (character == '"'){
                final = counter; // зафиксировали позицию последнего элемента, разность final - first --- длина строки */
                open_quotes = 0; // закрыли кавычку
                subline = strncpy(subline, str+first, final-first);
                subline[final-first] = '\0';
                cur->left = create_list_str(subline,  1, errflag);
            }
        }
    }
    /* а вдруг ковычки не закрыты? */
    if (open_quotes != 0) *errflag = -1;
    free(subline);
    return head;
}



int main(void){
	int errflag = 0;
	size_t size = 0;
	struct list *tree;


	char *str = read_string(&errflag, &size);
	if (errflag==0){
		tree = create_tree(str,size, &errflag);
		if (errflag != 0){
			printf("[error] %d\n",errflag);
		}
		else
			print_tree(tree);
	}
	//else print_list(tree);

	free_tree(tree);

	if (str != NULL)
		free(str);
	return 0;
}