#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SV_PORT 25400

using namespace std;

int CreateSocket(int);
int SendData(int s, string* buffer, sockaddr_in * addr);int RecvData(int s, string* buffer, sockaddr_in * addr);
int Sock = 0;

bool alive = true; // true if program is working

int Port;

#include "extract.h"

void * RT(void *)
{
    while (alive & Sock != 0)
    {
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = Port; // server port
	string Buf = ""; // receive buffer
	if (RecvData(Sock, &Buf, &address) > 0)
	{
	    string UserName;
	    string Msg;
	    int MT = ExtractMsg(&Buf, &UserName, &Msg);
//	    cout << "Debug: username = " << UserName << "\n";
	    switch (MT)
	    {
		case MT_BROADCAST:
		    cout << "Message from " << UserName << ": " << Msg << "\n";
		    break;
		case MT_QUIT_FS:
		    cout << "User " << UserName << " had left the chat.\n";
		    break;
		case MT_KILL_FS:
		    cout << "Angry user " << UserName << " killed the server. Press ENTER to exit.\n";
		    alive = false;
		    break;
	    }
	}
    }
    return 0;    
}

string getString(const string& s)
{
    printf("%s", s.c_str());
    string m;
    while (alive)
    {
        char c = fgetc(stdin);
	if (c == '\n') break;
	m += c;
    }
    return m;
}

int main()
{
    Port = rand() & 0xFFFF;
    Sock = CreateSocket(Port);
    string name = getString("Enter nickname:\n");
    pthread_t tid;
    pthread_create(&tid, 0, &RT, 0);
    while (alive)
    {	
	bool exitflag = false;
	string msg = getString("");
	
	string SendBuf = "M" + name + "#" + msg;
	if (msg == "kill") { SendBuf[0] = 'K'; exitflag = true; }
	if (msg == "quit") { SendBuf[0] = 'Q'; exitflag = true; }
	
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = SV_PORT; // server port
	string Buf; // receive buffer
	
	SendData(Sock, &SendBuf, &address);
	if (exitflag) break;
    }
    close(Sock);
    Sock = 0;
    alive = false;
    
    return 0;
}

int CreateSocket(int port)
{
    sockaddr_in address;
    int i = 1;
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) printf("Unable to create UDP socket\n");
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i)) == -1)
    	printf("Error setting broadcast socket\n");
	
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = port; // our port

    bind(sock, (sockaddr*)&address, sizeof(address));
    return sock;
}

int RecvData(int s, string * buffer, sockaddr_in *addr)
{
	timeval	timeout;
	fd_set	fdset;
	int i;
	socklen_t fromlen = sizeof(*addr);

	timeout.tv_sec = 0;
	timeout.tv_usec = 100;
	
	FD_ZERO(&fdset);
	FD_SET(s, &fdset);

	i = select(s+1, &fdset, 0, 0, &timeout);
	
	if (i > 0 && FD_ISSET (s, &fdset)) 
	{
	    char * buf = new char[1024];
	    int res = recvfrom(s, buf, 1024, 0, (sockaddr*)addr, &fromlen);
	    if (res == -1) { printf("recvfrom: Error\n"); return -1; }
	    (*buffer) = buf;
	    delete buf;
	    return res; // success: quantity of bytes
	}
	return 0;
}

int SendData(int s, string * buffer, sockaddr_in *addr)
{
	socklen_t len = sizeof(*addr);
	int res = sendto(s, buffer->c_str(), buffer->length()+1, 0, (sockaddr*)addr, len);

	if (res == -1)
	{
	    printf("Send packet error\n"); return 0;
	}
	return res; // suxxess
}
