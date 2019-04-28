#ifndef VCGI_HEADER
#define VCGI_HEADER

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sqlite3.h"
#include "nstl.h"
//VCGI -> Wang CGI

#define TMP 4968

int callback(void *ret, int argc, char **argv, char **azColName) {
	  char* string = (char*) ret;
	  char buf[TMP] = "";

	  if(strlen(string) == 0){
	  	strcat(string,"<table>\n");
	  	strcat(string,"<tr>\n");
	  	for (int i = 0; i < argc; i++) { 
	    	sprintf(buf,"<th>%s</th>\n",  azColName[i]);
	  		strcat(string,buf);
	  	}

	  	strcat(string,"</tr>\n");
	  }

	  //scorre la riga

  	  strcat(string,"<tr>\n");
	  for (int i = 0; i < argc; i++) { 
	    sprintf(buf,"<td>%s</td>\n",  argv[i] ? argv[i] : "NULL");
	  	strcat(string,buf);
	  }
	  strcat(string,"</tr>\n");
	  
	  printf("------------------------\n");
	  printf("string:%s\n",string);
	  printf("------------------------\n");

	  return 0;
}

class VCGI{
private:

	
public:
	VCGI(){
	}



	~VCGI(){
	}	

	static char* elabFile(char* fname){
		// scrivere su un file non va bene.... devi memorizzarlo in una stringa
		// e concatenarla con un https header e poi spedirlo, quindi devi
		// modificare il inviaFile del socketTCP.
		long tid = (long)pthread_self();
		const int n = snprintf(NULL, 0, "%lu", tid);

		// -5 per .html
		int flen = strlen(fname)-5;
		char* file_name =(char*) malloc(flen+1);
		strncpy(file_name, fname, flen+1);
		file_name[flen] = '\0';

		// +5 per .html
		char* out = (char*) malloc(sizeof(char)*(n+1+flen+5 ) ); ;
		snprintf(out, n+1+flen+5, "%s%lu.html", file_name,tid);

		free(file_name);

		printf("-----------------------------\n");
		printf("out:%s, file_name:%s\n",out,fname);
  		printf("-----------------------------\n");

     	FILE * fin = fopen(fname, "r");
	    FILE * fout = fopen(out,"w");
	    char * line = NULL;
	    size_t len = 0;
	    ssize_t read;
	    int n_matches=0;

	    if (fin == NULL)
	        exit(EXIT_FAILURE);

	    while ((read = getline(&line, &len, fin)) != -1) {
	        printf("Retrieved line of length %zu:\n", read);
	        printf("%s", line);
	        
	    	char** ret = regex_match(line,RGX_SQL,&n_matches);
	    	if(ret != NULL){
	    		for(int i = 0; i< n_matches; i++){
	    			printf("%s",ret[i]);
	    			free(ret[i]);
	    		}
	    		free(ret);
	    	}else {
	    		printf("written line:%s\n",line);
	    		fprintf(fout,"%s",line);
	    	}

	    }

	    free(line);
	    fclose(fin);
	    fclose(fout);
		return out;
	}

	#define MAX 50000
	static char* exe_tags(char* fname){
     	FILE * fin = fopen(fname, "r");
     	char ret[MAX] ="";
	    char * line = NULL;
	    size_t len = 0;
	    ssize_t read;
	    int n_matches=0;
	    

	    char** matches;

	    if (fin == NULL)
	        exit(EXIT_FAILURE);

	    while ((read = getline(&line, &len, fin)) != -1) {
	        printf("Retrieved line of length %zu:\n", read);
	        printf("%s", line);
	        
	    	matches = regex_match(line,RGX_SQL,&n_matches);
	    	if(matches != NULL){
	    		char* tab = VCGI::exe_sql(matches[1],matches[2]);
	    		if(tab == NULL)return NULL;

	    		strcat(ret,tab);
	    		printf("----------------\n");
	    		printf("ret:%s",ret);

	    		printf("----------------\n");
    			free(matches[1]);
    			free(matches[2]);
	    		free(matches);
	    	}else {
	    		strcat(ret,line);
	    	}

	    }

	    free(line);
	    fclose(fin);
    	return strdup(ret);
    }


    static char* exe_sql(char* dbname,char* query){
      printf("------------------------------\n");
      printf("executing query from db %s:%s\n",dbname,query);
      printf("------------------------------\n");
      char ret[MAX]="";
	  sqlite3 *db;
	  char *err_msg = 0;

	  int rc = sqlite3_open(dbname, &db);

	  if (rc != SQLITE_OK) {

	    printf("Cannot open database: %s\n", sqlite3_errmsg(db));
	    sqlite3_close(db);

	    return NULL;
	  }

	  char *sql = query; //query


	  rc = sqlite3_exec(db, sql, callback, ret, &err_msg); //gli passo la funzione di callback

	  if (rc != SQLITE_OK ) {

	    printf("Failed to select data\n");
	    printf("SQL error: %s\n", err_msg);

	    sqlite3_free(err_msg);
	    sqlite3_close(db);

	    return NULL;
	  }
	  sqlite3_close(db);

	  strcat(ret,"</table>\n");
	  return strdup(ret);
    }
	


};





#endif // STRING_HEADER