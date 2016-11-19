/*
 *  Task 2: Строковый калькулятор
 *
 *  Created by Vitaliy Rakitin in 2016 
 *  BD-11, TechnoSphera
 *
 * Решение в виде дерева.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_SIZE 1
#define PARENT_TREE 0
#define CHILD_TREE 1
#define START_POSITION 0
#define TYPE_PLUS 0
#define TYPE_STRING 2
#define DEFAULT_TIMES 1

/* Один лист нашего дерева */
struct list{
    int type; // TYPE_PLUS, TYPE_STRING
    size_t size; // Real size of a string
    size_t times; // repeat string N times, N = times 
    char *string;
    struct list *right;
    struct list *left;
};

/* очистка памяти от дерева */
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

/* Считывание строки со стандартного входа */
char* read_string(int *errflag, size_t *size){
    char character;
    size_t times = 1; // сколько раз повторили MAX_SIZE в разделе выделения памяти

    char *line = (char*)malloc(MAX_SIZE * sizeof(char));
    if (line == NULL) *errflag = 1;

    /* изначально мы ничего не считали */
    *size = 0;

    while((*errflag == 0)&&((character = fgetc(stdin))!= EOF)){
        if ((character!= 10)&&(character!=0)&&(character!=13)){
            line[*size] = character;
            (*size)++;
        }
        else break; /* если считали \n или \0, тогда пора заканчивать */

        if (*size >= times*MAX_SIZE-1){
            times++;
            line = (char*)realloc(line, (MAX_SIZE*times)*sizeof(char));
            if (line == NULL){
                *errflag = 1;
                break;
            }
        }
    }
    line[*size] = '\0';
    return line;
}

/* Создание одного листа дерева по заданным параметрам */
struct list* create_list(int type, const char *str, int *errflag){
    struct list *str_list;
    
    str_list = (struct list*)malloc(sizeof(struct list));
    if (str_list == NULL){
        *errflag = 1; 
        return 0;
    }

    /* параметры, которые всегда будут определены одинаково */
    str_list->type = type;
    str_list->times = DEFAULT_TIMES;
    str_list->left = NULL;
    str_list->right = NULL;

    /* Параметры, зависящие от типа */
    switch(type){
        case TYPE_STRING:
            str_list->size = strlen(str);

            str_list->string = (char*)malloc((str_list->size  + 2) * sizeof(char));
            if (str_list->string == NULL){
                *errflag = 1; 
                return 0;
            }
            str_list->string[0] = '\0';
            str_list->string = strcat(str_list->string,str);
            break;
        case TYPE_PLUS:
            str_list->size = 0; 
            str_list->string = NULL;
            break;
        default: 
            str_list->string = NULL;
            *errflag = 11; 
            break;
    }
    return str_list;
}

/* функция получения числа из строки с текущей позиции */
size_t getnum(char *str,size_t *counter,  int *errflag){
    size_t num = 0; // наше число
    int check_out = 1;  // пометка для выхода
    char character;
	size_t count;

    for (count = 0; (*errflag == 0)&&(check_out==1); count++){
        character = str[count + *counter];
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
            case ' ': 
                    if (num != 0){
                        check_out = 0; 
                        count -= 1;
                    } 
                    break;  /* пробел должен быть только перед числом */      
            default: 
            	if (num == 0) 
                    *errflag = 4; 
            	check_out = 0; 
                count -= 1; 
                break;
        }
    if (check_out != 1) break;
    }

    *counter+= count;
    return num;
}

/* Создание дерева */
struct list* create_tree(char *str, int in_breakets, size_t size, int *errflag, size_t *global_counter){
    char character; // текуший символ строки
	struct list* head; // корень
	struct list* cur; // текущая позиция на дереве
	int open_quotes = 0; // =1, значит живём в ковычках
	size_t first,final; // адрес первого и последнего элемента в ковычках (подстроки)
	char *subline; // подстрока, вытащенная из ковычек
    size_t local_global_counter = 0; // подсчёт количества символов, пройденных по строке начиная с 0 позиции данного вхождения в функцию 
    size_t num = 0; // число при умножении
    int check_multy=0; // проверка умножения в начале подстроки
    size_t counter = 0;

    /* создадим дерево и подстроку по дефолту */
	head = create_list(TYPE_PLUS, NULL, errflag);
	cur = head;
	subline = (char*)malloc(size*sizeof(char));	
	if (subline == NULL){
        *errflag = 1; 
        return head;
    }

    subline[0] = '\0';

    /* пойдём по строке */
    //printf("%lu %zu\n",strlen(str),size );
	for (counter = 0;(counter < size)&&(*errflag == 0);counter++){
        /* считываем элемент */

        character = str[counter];

        /* если open_quotes == 1, то это означает, что мы живём внутри открытых ковычек */
        if (open_quotes == 0)
            switch(character){
                /* открыли кавычку, сохранили позицию первого элемента внутри */
                case '"': 
                    first = counter+1; 
                    open_quotes = 1; break; 
                case '+': 
                    /* а вдруг перед + было число? */
                    if (num != 0) {*errflag = 8; break;}
                    /* если слева уже есть что-то, то пойдём вправо */
                	if (cur->left != NULL){
                		cur->right = create_list(TYPE_PLUS, NULL, errflag);
              	  		cur = cur->right;}
              	  		else *errflag = 2;
                	break; 
            	case '*':
                    /* если слева уже есть что-то, то мы явно это что-то умножаем */
                    /* поэтому считаем число */
                    if (cur->left != NULL){
                        counter++;
            			cur->left->times *= getnum(str,&counter,errflag);
            		}
            		else{
                        /* а возможно мы только что считали число, а сейчас будет строка */
                        if (num != 0) check_multy = 1;
                        else  *errflag = 5;
                    }
                    break;
                case ' ': break; /* нам нет разницы до пробелов */
            	case '(':
                    /* создадим поддерево, живущее в этих скобках */
            		cur->left = create_tree(str+counter+1, CHILD_TREE, size-counter-1, errflag,&local_global_counter);
                    /* раз мы вышли из рекурсии, значит скобки уже разобраны, перешагнём их */
                    counter += local_global_counter;
                    /* если мы пытались умножить перед скобками */
                    if ((num != 0)&&(*errflag==0)){
                        if (check_multy != 0){ 
                            cur->left->times = num;
                            num = 0;
                            check_multy = 0;
                        }
                        else{ 
                            /* число было, а знака умножения нет :( */ 
                            *errflag = 10;}
                    }
            		break;
            	case ')': 
                    /* закрываем скобки, а мы не в поддереве */
            		if (in_breakets != CHILD_TREE) {*errflag = 6;}
            		else{
                        /* просто выйдем из поддерева */
            			*global_counter += counter + 1;
            			counter = size; /* пора выходить */
            		}
            		break;
                default:
                    if (num == 0){
                        num = getnum(str,&counter,errflag); 
                    }
                    else *errflag = 7;
                    break;
            }
        else
        {   /* тут мы окажемся, если живёмвнутри ковычек
             * пока они открыты нам неважно, что внутри: там просто строка */
            if (character == '"'){
                /* зафиксировали позицию последнего элемента, разность final - first --- длина строки */
                final = counter;
                open_quotes = 0; // закрыли кавычку
                subline = strncpy(subline, str+first, final-first); // скопировали подстроку
                subline[final-first] = '\0';
                cur->left = create_list(TYPE_STRING, subline, errflag);
                /* если перед строкой было умножение */
                if ((num != 0)&&(*errflag==0)){
                    if (check_multy != 0){
                        cur->left->times = num;
                        num = 0;
                        check_multy = 0;
                    }
                    else *errflag = 10;
                }
            }
        }
    }
    /* а вдруг ковычки не закрыты? или что-то осталось неумноженным? */
    if ((open_quotes != 0)||(num!= 0)||(check_multy!=0)) {*errflag = -1;}
    free(subline);
    return head;
}

/* Cоздание итоговой строки без ковычек*/
char* create_out_string_real(struct list* tree, int *errflag){
    size_t size = 0; // размер созданной строки в текущий момент
    size_t times = 1; // сколько раз повторили MAX_SIZE в разделе выделения памяти под строку
    char *string = (char*)malloc(MAX_SIZE * sizeof(char)); // строка 
    char *substring = (char*)malloc(MAX_SIZE * sizeof(char)); // вспомогательная подстрока
    size_t sub_size = 0; // размер подстроки в текущий момент
    struct list* cur; // текущая позиция на дереве
    size_t i = 0;
    string[0] = '\0';
    cur = tree;

    while ((cur != NULL)&&(*errflag==0)){
        /*  если слева "+", значит там поддерево
            рекурсивно считаем результат в подстроку */
        if (cur->left->type == TYPE_PLUS){
            free(substring);
            substring = create_out_string_real(cur->left,errflag);
            /* выделим недостающую память в нашей строке */
            while((strlen(substring) + size >= times*MAX_SIZE-1)&&(*errflag==0)){
                times++;
                string = (char*)realloc(string, (MAX_SIZE*times)*sizeof(char));
                if (string == NULL){
                    *errflag = 1;
                    break;}   
            }
            /* объеденим полученные строки */
            string = strcat(string,substring);
        }
        else{
            /*  если мы тут, значит слева строка */
            /*  определим её размер */
            sub_size = strlen(cur->left->string) * (cur->left->times) + 2;

            /* выделим недостающую память в нашей строке */
            while((sub_size + size >= times*MAX_SIZE-1)&&(*errflag==0)){
                times++;
                string = (char*)realloc(string, (MAX_SIZE*times)*sizeof(char));
                if (string == NULL) {*errflag = 1;}   
            }
            if (*errflag != 0) break;

            /* добавим подстроку нужное количество раз */
            for (i=0;i < cur->left->times;i++) 
                string = strcat(string,cur->left->string);
        }
        size = strlen(string);
        cur = cur->right;
    }
    /* закончились листы, проверим: а вдруг мы в поддереве, 
       которое повторяется несколько раз */
    if ((tree->times > 1)&&(*errflag==0)){
        /* в подстроку запишем всю нашу строку */
        free(substring);
        substring = (char*)malloc((size + 2) * sizeof(char)); 
        if (substring == NULL) {*errflag = 1;} 
        else{
            substring[0] = '\0';
            substring = strcat(substring,string);

            /* выделим недостающую память в нашей строке */
            string = (char*)realloc(string, (size * tree->times + 2)*sizeof(char));
            if (string == NULL) {*errflag = 1;} 
            /* продублируем строку нужное количество раз */ 
            else 
                for (i=1;i<tree->times;i++)
                    string = strcat(string,substring);
        }
    }

    free(substring);
    return string;
}

char* create_out_string(struct list* tree, int *errflag){
    char *string_without;
    char *string=NULL;
    size_t size = 0;
    string_without = create_out_string_real(tree,  errflag);
    if (*errflag == 0){
        size = strlen(string_without);
        string = (char*)malloc((strlen(string_without)+3) * sizeof(char)); // строка 
        if (string == NULL){
            *errflag = 10;
        }
        else{
            string[0] = '"';
            string[1] = '\0';
            string = strcat(string,string_without);
            string[size+1] ='"';
            string[size+2] ='\0';
        }
    }
    free(string_without);
    return string;

}

int main(void){
	int errflag = 0;
	size_t size = 0; // размер строки
	struct list *tree = NULL; // наше дерево
	size_t global_counter = START_POSITION; // количество обработанных символов
    char *string = NULL; // строка

    /* считаем строку */
    string = read_string(&errflag, &size);
    
    if (size <= 0) errflag = 11;
	if (errflag==0){
        /* создадим дерево */
		tree = create_tree(string,PARENT_TREE,size, &errflag, &global_counter);
		if (errflag == 0){    
            /* преобразуем дерево в строку */
            free(string);
            string = create_out_string(tree,&errflag);
            /* если всё хорошо, выведем результат */
            if (errflag == 0)
                printf("%s\n", string);
        }
    }
    if (errflag != 0)
        printf("[error]\n");
    
    /* освободим память от дерева и от строки */
    if (tree != NULL)
	   free_tree(tree);
    if (string != NULL)
        free(string);
	return 0;
}