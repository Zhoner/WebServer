#ifndef __ADDRESS_HPP
#define __ADDRESS_HPP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP_LO "127.0.0.1" 
// loopback
#define IP_DHCP "0.0.0.0"
/* con il servizio DHCP si richiede l'ip con un broadcast  
	a tutti i server del mondo e qualcuno lo rimanderà
*/ 

#define PORT_ZERO 0

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
class Address{
private:
	char*ip;
	int port;
	

public:	
	Address	();
	Address	(const Address &);
	Address	(char* 	, int );
	Address (sockaddr_in &);
	~Address();
  

    char* 	getIp	();
    int  	getPort	();
    sockaddr_in getBinary();
    
    void  	setIp	(const char* );
    void  	setPort	(int );
    void    setBinary(sockaddr_in &);
    char* 	toString();
    
};


Address::Address(){
	this->ip 	 = IP_LO;
	this->port 	 = 0;
}

Address::Address (const Address &a){
	this->ip = strdup( a.ip);
	this->port = a.port;
}

Address::Address(char* ip,int port){
	this->ip   = strdup(ip);
	this->port = port;
}

Address::~Address(){
	free(ip);
}

char* Address::getIp(){
	return ip;
}

int Address::getPort(){
	return port;
}

void Address::setIp(const char* ip){
	this->ip = strdup(ip);
}

void Address::setPort(int port){
	this->port = port;
}

char* Address::toString(){
	// IP:%s , PORT: %d 
	char	str[100];
	sprintf	(str , "%s:%d",ip,port);
	return strdup(str);
}


Address::Address (sockaddr_in & inet_addr){
	this->port = ntohs(inet_addr.sin_port);
	this->ip = strdup( inet_ntoa(inet_addr.sin_addr) ); 
}

sockaddr_in Address::getBinary(){
	sockaddr_in inet_addr;
	
	inet_addr.sin_family = AF_INET;
	inet_addr.sin_port = htons(this->port);
	inet_aton(this->ip,&inet_addr.sin_addr);
	
	for(int i = 0; i< 8; i++)
		inet_addr.sin_zero[i] = '\0';
	
	return inet_addr;
}

void Address::setBinary(sockaddr_in &inet_addr){
	this->port = ntohs(inet_addr.sin_port);
	pthread_mutex_lock( &mtx );
	this->ip = strdup( inet_ntoa(inet_addr.sin_addr) );
	pthread_mutex_unlock( &mtx );
}
#endif /*__ADDRESS_HPP*/
