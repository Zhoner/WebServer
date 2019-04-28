#ifndef SOCKET_TCP_HPP
#define SOCKET_TCP_HPP

#include "Address.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iterator>
#include <sys/types.h>
#include "nstl.h"

#include <list>
using namespace std;

#define ERROR_SOCKET -101
#define ERROR_BIND -102
#define ERROR_CLOSE -103
#define ERROR_REC 0
#define ERROR_BC -104
#define ERROR_LISTEN -105
#define MAX 10000

#define TCP_MAX 65535


#define HEADER_HTTP "HTTP/1.1 200 OK\n\
Date: Wed, 19 Apr 2017 16:34:20 GMT\n\
Content-Type: text/html\n\ 
charset=UTF-8\n\
\n"
/*----------------------------------------------*/
/*SocketTCP  
  api used:   socket() close() setsockopt	   -
  funtions : setBroadcast() 					*/
/*----------------------------------............*/
class SocketTCP{
protected:
	int sock_id;
	
public:
	SocketTCP(){ sock_id = socket(AF_INET, SOCK_STREAM,0); }
	
	void setBroadcast(bool activeBroadcast){
		int is_bc = activeBroadcast ? 1 : 0;
		if (setsockopt(sock_id, SOL_SOCKET, SO_BROADCAST, (void *) &is_bc,sizeof(is_bc)) < 0)
        errore(ERROR_BC,"setsockopt()");
	}
	
	~SocketTCP(){ if(close (sock_id) ) errore("close()",ERROR_CLOSE); }
};




/*----------------------------------*/
/*Connessione	
  api used:  recv() send()        
  functions: setHost(), get Host() ,
  invia() , ricevi() */
/*----------------------------------*/
class Connessione{
protected:
	int conID;
private:
	Address* host;
	
public:
	Connessione(int conID){
		host = nullptr;
		if(conID > 0 ) this-> conID = conID;
	};
	
	~Connessione(){
		if(host != nullptr) 
			delete host;
	}
	
	void setHost(Address* host){
		if(host != nullptr) 
			this->host = host;
	}
	
	Address* getHost(){
		Address* ret ;
		memcpy(ret, host, sizeof(ret));
		return ret;
	}
	
	bool invia(char* msg){
		return inviaRow(msg, strlen(msg));
	}

	bool inviaHTML(char* path){
		int tlen = 0;
		char* text = readFile(path,&tlen);

		int blen = 0;
	    char* buffer = addHTTPHeader(text, &blen);

	    inviaRow(buffer, blen);
	    printf("%s",buffer);

	    free(buffer);
	    free(text);

		return false;
	}

	char* addHTTPHeader(char* text, int* len){
		*len = strlen(HEADER_HTTP) + strlen(text) + 1;
		char* buffer = (char*) malloc(*len );
		strcat(buffer,HEADER_HTTP);
		strcat(buffer,text);
		buffer[*len] = '\0';
		return buffer;
	}


	
	char* ricevi(){
		int length;
		char* buf = (char*)riceviRow(&length); 
		buf[length] = '\0';
		return buf;
	}

	void* riceviRow(int* len){
		/*receving the string*/
		char buffer[MAX+1];
		int length = recv(conID, buffer , MAX ,0);
		if(length < 0) errore("receviRow()",-5);
		*len = length;
		/*copying into new array*/
		char* ret = new char[length];
		memcpy(ret, buffer, length);
		return ret;
	}

	/** invia qualsiasi tipologia di dati*/
	bool inviaRow(char* msg, int length){
		
		int ret = send(conID, msg, length,0);
		if( ret != length )
			errore("inviaRow()",-4);
		return ret;	
	}
};
	
		

/*----------------------------------*/
/*ServConn	 
  api used:		 shutdown()	        */
/*----------------------------------*/
class ServerConn : public Connessione{
public:
	ServerConn(int conID): Connessione(conID){}
	~ServerConn(){shutdown(conID,SHUT_RDWR);}
};

/*----------------------------------*/
/*ClientConn 
  api used: connect()	
  funtions: connetti()   	        */
/*----------------------------------*/
class ClientConn : public Connessione{
public:
	ClientConn(int conID): Connessione(conID) {}

	bool connetti(Address* server){
		sockaddr_in addr = server->getBinary();
		int ret = connect(conID,(struct sockaddr*) &addr,  sizeof( struct sockaddr_in)  );
		if(ret == -1) errore("connect()",-3);
		setHost(server);
	}	
};





/*------------------------------------------------------*/
/*ServerTCP	   		    						 		-
 api used:   listen() bind() accept()		     	   -*/
/*functions: accetta() , disconetti(),invia(),ricevi()  */
/*------------------------------------------------------*/
class ServerTCP:public SocketTCP{
private:
	list<ServerConn*> serverConns;
	
	list<ServerConn*>::iterator getIterator(ServerConn* srvCon){
		if(srvCon == nullptr) return serverConns.end();
		for(list<ServerConn*>::iterator it = serverConns.begin() ; it != serverConns.end(); it++)
			if( srvCon == *it ) return it;
		return serverConns.end();
	}

public:
	ServerTCP(int port , int nlisten = 5,bool loopback = false){
		/*bind() with address myself */
		char* ip = strdup( loopback ? IP_LO : IP_DHCP) ;
		if(sock_id == -1) errore("socket()",ERROR_SOCKET);
	
		if(port != 0 ){
			Address myself(ip,port);
			sockaddr_in myself_addr = myself.getBinary();
			int ret_bind = bind(sock_id,(sockaddr*)&myself_addr,sizeof(struct sockaddr_in));
			if(ret_bind == -1) errore(ERROR_BIND,"bind()");
		}
		
		/*listen() and init the array of serverConn*/
		if(nlisten > 1) listen(sock_id,nlisten);
		else errore("listen()",ERROR_LISTEN);
	}
	
	ServerConn* accetta(){
		/*getting the client address*/
		sockaddr_in client_address;
		int len = sizeof(struct sockaddr_in);
		int connID = accept(sock_id,(sockaddr*)&client_address, (socklen_t*) &len);
		if(connID == -1) errore("accept()",-3);
		Address* host = new Address();
		host->setBinary(client_address);
		
		/*adding a serverConn*/
		ServerConn* srvCon = new ServerConn(connID);
		srvCon->setHost(host);
		serverConns.push_back(srvCon);
		return srvCon;
	}
	
	bool disconnetti(ServerConn* srvCon){
		if(srvCon == nullptr) return true;
		delete srvCon;
		serverConns.remove(srvCon);
	}

	bool disconnettiTutti(){
		while( !serverConns.empty() )
			disconnetti( serverConns.front() );
	}
	
	~ServerTCP(){
		disconnettiTutti();
	}

	bool inviaRowTutti(char* msg,int leng){
		bool ret = true;
		for(list<ServerConn*>::iterator it = serverConns.begin(); it != serverConns.end(); it++)
			ret = ret && (*it)->inviaRow(msg,leng);
		
		return ret;
	}

	bool inviaTutti(char* msg){
		return inviaRowTutti( msg, strlen(msg));
	}
};

/*------------------------------------------*/
/*ClientTCP 			     	  			 /
  api used: none							 /
  functions: connetti(), invia(), ricevi()  */
/*----------------------------------		*/
class ClientTCP: public SocketTCP{
private:
	ClientConn* clientConn;
public:
	ClientTCP(){
		clientConn = new ClientConn(sock_id);
	}
	
	bool connetti(Address* server){
		if(server != nullptr && clientConn != nullptr)
		  return clientConn->connetti(server);
		return true;
	}
	
	bool invia(char* msg){
		if(clientConn != nullptr) 
			return clientConn->invia(msg);
		else return true;
	}
	char* ricevi(){
		if(clientConn != nullptr)
			return clientConn->ricevi();
		else return 0;
	}

	bool disconettiti(){
		if(clientConn != nullptr){
			delete clientConn;
			return false;
		}return true;
	}
	
	~ClientTCP(){
		disconettiti();
	}
};



#endif
