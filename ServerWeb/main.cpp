#include "SocketTCP.hpp"
#include "VCGI.h"
#include <pthread.h>

struct Params{
	ServerConn* connection;
	ServerTCP* myself;
};

void* request(void* connection);

int main(int argc, char const *argv[]) {

	if(argc != 2){
		printf("USAGE: %s PORT\n",*argv);
		return -1;
	}

    int port = atoi(argv[1]);
    ServerTCP* server= new ServerTCP(port);

    while (1) {
      ServerConn* conn = server->accetta();
      
      Params* params = (Params*)malloc(sizeof(Params) );
      params->myself = server;
      params->connection = conn;

      pthread_t thread_id;
      pthread_create(&thread_id,NULL,request,(void*)params);
    }

    delete(server);
  return 0;
}

void* request(void* params){
  Params* p = (Params*) params;
  
  ServerConn* conn = (ServerConn*) p->connection;
  ServerTCP* myself = (ServerTCP*) p->myself;

  char* request = conn->ricevi();
  printf("%s\n\n",request);
  //http://192.168.6.72:8686/filename.html?filename=index.html
  char* fname = src_file(request);
  
  
  if(fname != NULL){
  	printf("invia html\n");
  	// creates a file that has the data table getted from the database
  	char* fhtml = VCGI::exe_tags(fname);
  	if(fhtml != NULL){
	  	int lenAnsw = 0;
	  	char* answHTTP = conn->addHTTPHeader(fhtml,&lenAnsw); 
	  	conn->inviaRow(answHTTP,lenAnsw);
	  	printf("invia finito\n");
		
		free(fhtml);
	  	free(answHTTP);
  	}else{
  		printf("Errore SQL o qualcos'altro");
  	}
  }
  myself->disconnetti(conn);
  free(fname);
  free(p);
  
  pthread_exit(NULL);
}
