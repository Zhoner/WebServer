#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include "nstl.h"

int main(int argc, char* argv[])
{
    char* text = argv[1];
    //RGX_SQL
    char* regex = RGX_SQL;
    int n_matches=0;
    printf("text:%s, regex:%s\n",text,regex);

    char** matches = regex_match(text,regex,&n_matches);

    printf("n_matches:%d\n",n_matches);
   	if(matches != NULL)
   		printf("first match:%s\n",matches[0]);
    else 
    	printf("it doesnt match\n");
    return 0;
}