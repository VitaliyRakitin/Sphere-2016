/*
 *  Task 1: Сообщения
 *
 *  Created by Vitaliy Rakitin in 2016 
 *  BD-11, TechnoSphera
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 5 
#define AMOUNT_OF_KEYS 4
const char* keys[] = {"Date:\0", "From:\0", "To:\0", "Subject:\0"};

/* отбор ключевых строк: 1 -- да, 0 -- нет */
int filter(char* line, size_t in_length, int *errflag){
char *subline;
int result = 0;
size_t key_len; // размер ключа

for(size_t j = 0; j<AMOUNT_OF_KEYS; j++){
    /* будем сравнивать строки и ключи по 1ому элементу */
    /* если длина строки меньше длины ключа -- нет смысла это рассматривать */
    key_len = strlen(keys[j]);
    if (( line[0] == keys[j][0] )&&( key_len <= in_length )){
    	subline = (char*)malloc((key_len + 2) * sizeof(char));
    	if (subline == NULL) {*errflag = -1; break;}

    	/* запишем в subline кусок строки размера подошедшего ключа */
    	subline = strncpy(subline,line,key_len);
    	subline[key_len] = '\0';

    	if (strcmp(subline, keys[j]) == 0) result = 1;
    	free(subline);
    }

    if (result == 1) break;
    }
    return result; 
}



int main(void){
    char a; // текущий считанный символ
    int str_size = 0;  // количество строк  
    size_t size = 0;  // размер текущей строки
    size_t line_times = 1;  // сколько раз повторили MAX_SIZE в разделе выделения памяти под строку
    size_t str_times = 1;  // сколько раз повторили MAX_SIZE в разделе выделения памяти под строки
    int errflag = 0;

    char** str =(char**)malloc(MAX_SIZE * sizeof(char*)); // подходящие строки
    if (str== NULL) errflag = -1;
    char* line = (char*)malloc(MAX_SIZE * sizeof(char)); // текущая строка
    if (line == NULL) errflag = -1;

    while((errflag == 0)&&((a = fgetc(stdin))!= EOF)){
        if ((a == 10)||(a==0)){ // \n или \0

        	/* считали пустую строку? пора заканчивать */
            if (size == 0) break; 

            /* проверим, а вдруг это то, что нам нужно */
            line[size] = '\0';
            if (filter(line, size, &errflag)){
                /* если да, то добавим в str */
				str[str_size]= (char*)malloc((size+2)* sizeof(char));
				if (str[str_size] == NULL) {errflag = -1;break;}
            	str[str_size]=strcpy(str[str_size],line);
				str_size++;
			}

			/* почистим за собой */	
            free(line);
            line = (char*)malloc(MAX_SIZE * sizeof(char));
            if (line == NULL) {errflag = -1; break;}
            size = 0;
            line_times = 1;
        }
        else{
        line[size] = a;
        size++;
        }

        if (size >= line_times*MAX_SIZE-1){
            line_times++;
            line = (char*)realloc(line, (MAX_SIZE*line_times)*sizeof(char));
            if (line == NULL) {errflag = -1;break;}
        }     

        if (str_size >= str_times*MAX_SIZE-1){
            str_times++;
            str = (char**)realloc(str, (MAX_SIZE*str_times)*sizeof(char*));
            if (str == NULL) {errflag = -1;break;}
        }
    }
    free(line);

    if (errflag != 0){
        printf("[error]");
        return 0;
    }

    for (int i=0;i<str_size;i++) {printf("%s\n",str[i]);}

    for (int i=0;i<str_size;i++) {free(str[i]);}
    free(str);
    fclose(stdin);
    
	return 0;
}



