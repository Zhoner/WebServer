#ifndef STRING_HEADER
#define STRING_HEADER
#define MAX_STR 4960

#include <errno.h>
#include <string.h>
#include <regex.h>



#define TAG_SQL "<?sql"
#define RGX_SQL "<sql\\s*db=([a-zA-Z\\s<>=.]*);\\s+query=(.*);\\s*/>"
// <sql\s+db=(.+)\s+query=(.+;)\s*\/>
//regcomp(...) --> crea regex
//regexec(...) -> utilizza regex
/*
-----------------------------
STRING
-----------------------------
*/


void outStr(char *s);
int cmpStr(char *s1, char *s2);
int lengthStr(char *s);
char* dupStr(char *s);
char * inputStr();
char * findChar(char *s,char c);
char** dupVett(char **v);
char** split(char *s, char c);
void free_vect(char**);
char* int2str(int num);
void revStr(char *s);
void errore(int n,char*s);
void errore(char* s,int n);
char* inputStr();
void* dupBuff(void* buf, int size);
char* readFile(char* fname);



void* dupBuff(void* buf, int size){
	char* ret = (char*) malloc(size);
	memcpy(ret,buf,size);
	return (void*)ret;
}

char* inputStr(){
    char buffer[MAX_STR + 1];
    int i;
    for(i=0;(buffer[i] = getchar()) != '\n'; i++){ }
    
    if(i != 0)buffer[i] = '\0';
    return dupStr(buffer);
}

void errore(char* s, int n){
	printf("errore bloccante... %d{%s}, durante %s, ret con = %d\n",errno,strerror(errno),s,n);
	exit(n);
}

void errore(int n, char* s){
	printf("errore bloccante... %d{%s}, durante %s, ret con = %d\n",errno,strerror(errno),s,n);
	exit(n);
}



void free_vect(char** v){
	char ** tmp;
	for (tmp = v; *tmp; tmp++)
		free(tmp);	
	free(v);
}

void outStr(char *s){
    for(;*s;s++)
        putchar(*s);
}

int cmpStr(char *s1, char *s2){
    if(*s1>*s2)return -1;
    if(*s1<*s2)return +1;
    if(!*s1)return 0 ;
    return cmpStr(++s1,++s2);
}

int lengthStr(char *s){
    int ret;
    for(ret = 0 ; *(s+ret);ret++){}
    return ret;
}

char* dupStr(char *s){
    char * ret;
	int i = 0;

    ret = (char*)malloc(sizeof(char)*(lengthStr(s)+1));
    
    for(i=0;*(ret+i)=*(s+i);i++){}
    return ret;
}


char* findChar(char *s,char c){
	int i = 0;	    
	for(i = 0;*(s+i);i++)
	        if(*(s+i)==c)return s+i;
	
    return NULL;
}

char** dupVett(char **v){
    char **ret;
    int length = 0 ;
    int i ;
    for(;*(v+length);length++){}
    ret = (char**) malloc (sizeof(char*)*(length+1));
    for(i=0; *(ret+i) = *(v+i) ;i++){}
    return ret;
}



char** split(char *z, char c){
    char *buffer[MAX_STR+1] ;
    char *p ;
	  char* s = dupStr(z);
    int j = 0;
	
    for(p=s;p=findChar(s,c);){
        *p = '\0';
        buffer[j++] = dupStr(s);
	      s=p+1;
    }
	  
	  if(s!= NULL) buffer[j++] = dupStr(s);
	  buffer[j] = NULL;
    return dupVett(buffer);
}


char* int2str(int num){
	char buffer[MAX_STR+1];
	int i = 0;

	if(num == 0)buffer[i++] = num%10+'0';
	else {
		while(num>0){
		    buffer[i++] = num%10+'0';  
			  num = num/10;
		}
	}
  	buffer[i] = '\0';
	revStr(buffer);
	return dupStr(buffer);
}

void revStr(char *s){
	char* begin = s;
	char* end = s+lengthStr(s)-1;
	char tmp;
	for(;begin!=end; begin++,end--){
		tmp = *begin;			
		*begin = *end;
		*end = tmp;	
	}
}


// copia una substring in una stringa
char* instrstr(char* str, int start, int end){
	int len = end - start;
	
	char* ret = (char*) malloc(sizeof(char)*(len+1) );
	memcpy(ret,str+start, len);
	ret[len] = '\0';

	return ret;
}


char* readFile(char* path, int* len){
	FILE* fp = fopen(path,"rb");
		
	if(fp == NULL)
		return NULL;

	fseek(fp, 0L, SEEK_END);
	int fsize = ftell(fp); // file size
	rewind(fp);

	char* text = (char*) malloc(fsize+1);
	fread(text,fsize,1,fp);

	*len = fsize+1;
	text[fsize] = '\0';
    fclose(fp);
    return text;
}

/*
----------------------------------------------------
REGEX - ritorna le parti che matchano con il REGEX e 
SUBREGEX e il numero di matches
---------------------------------------------------
*/

char** regex_match(char* text, char* pattern,int* n_matches){
	regex_t rex;
    int rc;
    char** ret = NULL;


    if ((rc = regcomp(&rex, pattern, REG_EXTENDED))) 
        return NULL;
    
    //char errore[100];

    *n_matches = rex.re_nsub+1;
    regmatch_t* matches =(regmatch_t*) malloc(sizeof(regex_t) * (rex.re_nsub + 1));
    ret = (char**) malloc(sizeof(char*)*(rex.re_nsub + 1) );

    if ((rc = regexec(&rex, text, rex.re_nsub + 1, matches, 0))){
        //regerror(rc, &rex, errore, 100);
		//printf("Error in regexec(): %s\n", errore);
		return NULL;
        /* error or no match */
    } else {
        for(int i = 0; i < rex.re_nsub+1; i++){
            ret[i] = instrstr( text, matches[i].rm_so, matches[i].rm_eo);
        	printf("ret[%d]:%s\n",i,ret[i]);
    	}
    }

    free(matches);
    regfree(&rex);
    return ret;
}



/*
-------------------------------------
SQL
-------------------------------------
*/

char* containsTag(char* html,char* tag){
	char* ret = strstr( html, tag );
	return (ret);
}

#define HTTP "HTTP"
#define SEPERATOR "&"
#define START_PARAM "?"
#define EQUALS "="

char* src_file(char* request){

	char* ptr_s = strstr(request,START_PARAM);
	
	if(ptr_s == NULL)
		return NULL;
	
	char* ptr_f = strstr(request,HTTP);
	if(ptr_f == NULL)
		return NULL;


	int length = ptr_f - ptr_s; // -2 caused by blank space and "?" and +1 for \0s
	printf("length:%d\n",length);
	char* line = (char*) malloc(length*sizeof(char)-1);
	
	int i = 0;
	for(i =0 ; i < length; i++)
		line[i] = *(ptr_s+i+1); // salta "?"
	
	line[length-2] = '\0';

	printf("----------------------------\nparams:%s\n----------------------------------\n",line);
	
	char** params;
	int k = 0;

	/* */
	

	char* token = strtok( line, SEPERATOR);
	char* fname = strdup(strstr(token,EQUALS)+1);
	printf("--------------\nfname %s\n------------\n",fname);
	free(line);
	return fname;
}


#endif // STRING_HEADER
