#ifndef __SOCKET_UDP
#define __SOCKET_UDP

#include "Address.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "nstl.h"




#define UDP_MAX 65507
#define BROADCAST_PORT 6363

#define ERROR_SOCKET -101
#define ERROR_BIND -102
#define ERROR_CLOSE -103
#define ERROR_REC 0
#define ERROR_BC -104

class SocketUDP{
private:
	int socketId;
	
public:
	SocketUDP(int port = PORT_ZERO , bool loopback = false);
	~SocketUDP();

	bool invia( char* msg, const Address dest);
	char* ricevi(Address& mit);
	
	bool invia_row(void* msg,  Address dest,int size_msg);
	void* ricevi_row(Address &mit,int &size);
	
	bool inviaFile(char* path, Address *dest);
	int riceviFile(char* filename);
	
	bool setBroadcast(bool activeBroadcast);
};



bool SocketUDP::setBroadcast(bool activeBroadcast){
	int is_bc = activeBroadcast ? 1 : 0;
	if (setsockopt(socketId, SOL_SOCKET, SO_BROADCAST, (void *) &is_bc,sizeof(is_bc)) < 0)
        errore(ERROR_BC,"setsockopt()");
    
    return false;
}

SocketUDP::SocketUDP(int port,bool loopback){
	socketId = socket(AF_INET, SOCK_DGRAM,0);
	char* ip = loopback ? strdup(IP_LO) : strdup(IP_DHCP) ;
	
	if(socketId == -1) errore(ERROR_SOCKET,"socket()");
	
	if(port != 0 ){
		Address myself(ip,port);
		sockaddr_in myself_addr = myself.getBinary();
		int ret_bind = bind(socketId,(sockaddr*)&myself_addr,sizeof(struct sockaddr_in));
		if(ret_bind == -1) errore(ERROR_BIND,"bind()");
	}
}

SocketUDP::~SocketUDP(){
	int ret = close(socketId);
	if(ret) errore(ERROR_CLOSE,"close()");
}

bool SocketUDP::invia( char* msg, const Address dest){
	return invia_row((void*)msg,dest,(int)strlen(msg));
}

char* SocketUDP::ricevi(Address& mit){
	int size;
	char* row_buff = (char*)ricevi_row(mit,size);
	char* ret = (char*) malloc(sizeof(char) * (size+1));
	memcpy(ret,row_buff,size);
	ret[size] = '\0';
	return ret;
}


bool SocketUDP::invia_row( void* msg,  Address dest,int size_msg){
	sockaddr_in mittente = dest.getBinary();
	
	int ret_send = sendto(
		socketId,
		msg,
		sizeof(char) * (size_msg),
		0, 
		(sockaddr*)&mittente, 
		(socklen_t)sizeof(sockaddr_in));
	
	if(ret_send !=  sizeof(char) * (size_msg))return true;
	return false;
}

void* SocketUDP::ricevi_row(Address &mit,int &size){
	unsigned char buffer[UDP_MAX+1];
	struct sockaddr_in mittente;
	int size_packet = sizeof(struct sockaddr_in);

	int ret_recv = recvfrom(
		socketId,
		buffer,
		UDP_MAX,
		0,
		(sockaddr*)&mittente,
		(socklen_t*)&size_packet
	);
	if(ret_recv < 1)return ERROR_REC;
	mit.setBinary(mittente);
	
	return dupBuff(buffer,ret_recv);	
}


bool SocketUDP::inviaFile(char* path, Address *dest){
	FILE* fp = fopen(path,"rb");
	if(fp == NULL){
		int n_packet = 0;
		invia_row((void*)&n_packet,*dest,(int)sizeof(int));
		return true;
	}	
	fseek(fp, 0L, SEEK_END);
	int fsize = ftell(fp); // file size
	int packet_size;
	rewind(fp);
	int n_packet = ( (fsize %UDP_MAX) == 0) ? fsize/UDP_MAX : (fsize/UDP_MAX) +1 ;	
	invia_row((void*)&n_packet,*dest,(int)sizeof(int));
	
	int tmp_size = fsize;
	for(int i= 0; i<n_packet; i++){
	    unsigned char buffer[UDP_MAX];
	    packet_size= ( (tmp_size%UDP_MAX) == 0) ? UDP_MAX : (fsize%UDP_MAX);
	    fread(buffer,packet_size,1,fp);
		tmp_size -= packet_size;
	    invia_row(buffer,*dest,packet_size);
	}

	fclose(fp);
	return false;
}
int SocketUDP::riceviFile(char* filename){
	FILE* fp = fopen(filename,"wb");
	Address mit;
	int size;
	int size_tot=0;
	int* n_packet = (int*)ricevi_row(mit,size);
	for(int i= 0; i< *n_packet; i++){
	    char* buffer = (char*) ricevi_row(mit,size);
		size_tot+= size;
		fwrite(buffer,size,1,fp);
	}
	fclose(fp);
	return size_tot;
}


#endif /*__SOCKET_UDP*/
