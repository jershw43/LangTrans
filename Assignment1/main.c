#include <stdio.h>
#include <stdlib.h>
#include "file_util.c"

int main(){
    const int SIZE = 255;

    char str[SIZE];
    FILE* fp;
    fp = fopen("input.txt","r");
    
    if (fp == NULL)
    {
        printf("The file was not opened");
    }
    while(fgets(str,SIZE,fp) != NULL){
        printf("%s",str);
    }
    return 0;
}