#include "so_stdio.h"
#include<stdio.h>


int main(){

	SO_FILE *f = so_fopen("laba","r");
	
	for(int i = 0 ; i < 100; i++){
		printf("%d ",so_fgetc(f));
	}
	//so_fputc(98,f);
	for(int i = 0 ; i < 100; i++){
	//	printf("%d", so_fgetc(f));
		
	}
	return 0;

}
