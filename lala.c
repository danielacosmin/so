#include "so_stdio.h"
#include<stdio.h>
#include <string.h>


int main()
{
	SO_FILE *f = so_fopen("testmare", "r");
	
	char *ptr =malloc(16000);
	so_fread(ptr, 1, 16000, f);
	//for(int i = 0 ; i < 16000; i++){
	//	c = so_fputc(97,f);
	//
	//}
	free(ptr);
	so_fclose(f);
	return 0;
}
