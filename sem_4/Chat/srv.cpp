#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <list>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SV_PORT 25400

using namespace std;

int CreateSocket(int);
int SendData(int s, string *buffer, sockaddr_in *addr);
int RecvData(int s, string *buffer, sockaddr_in *addr);
int Sock = 0;

#include "extract.h"

class CUser {
public:
  string Name;
  sockaddr_in IP;
};

std::list<CUser> Users;

sockaddr_in cli_addr;

#define IT list<CUser>::iterator

int main() {
  printf("This is a udp chat server.\n");
  if ((Sock = CreateSocket(SV_PORT)) == -1)
    return 0;

  while (true) {
    bool exitflag = false;
    string Buf;
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = SV_PORT; // 27000
    if (RecvData(Sock, &Buf, &address) == 0)
      continue;
    // parse message
    string Msg;
    string UserName;
    int MT = ExtractMsg(&Buf, &UserName, &Msg);
    bool found = false;
    IT it;
    for (it = Users.begin(); it != Users.end(); it++)
      if (it->Name == UserName) {
        found = true;
        break;
      }
    if (!found) {
      cout << "New user came: " << UserName << "\n";
      CUser u;
      u.Name = UserName;
      memmove(&u.IP, &address, sizeof(sockaddr_in));
      Users.push_back(u);
    }
    string BBuf = " " + UserName + "#" + Msg;

    switch (MT) {
    case MT_KILL_TS:
      BBuf[0] = 'k';
      exitflag = true;
      break;
    case MT_QUIT_TS:
      BBuf[0] = 'q';
      for (it = Users.begin(); it != Users.end(); it++) {
        if (it->Name == UserName) {
          cout << "User " << UserName << "died.\n";
          Users.erase(it);
          break;
        }
      }
      break;
    case MT_MESSAGE:
      BBuf[0] = 'B';
      break;
    }
    // if (Users.empty()) break;

    cli_addr.sin_family = AF_INET;

    for (it = Users.begin(); it != Users.end(); it++) {
      cli_addr.sin_addr.s_addr = it->IP.sin_addr.s_addr;
      cli_addr.sin_port = it->IP.sin_port;
      cout << "Debug: bbuf = " << BBuf << "\n";
      SendData(Sock, &BBuf, &cli_addr);
    }
    if (exitflag)
      break;
  }
  close(Sock);
  return 0;
}

int CreateSocket(int port) {
  sockaddr_in address;
  int i = 1;
  int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (sock == -1)
    printf("Unable to create UDP socket\n");
  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i)) == -1)
    printf("Error setting broadcast socket\n");

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = port; // our port

  if (bind(sock, (sockaddr *)&address, sizeof(address)) == -1) {
    printf("Error binding socket\n");
    return -1;
  }
  return sock;
}

int RecvData(int s, string *buffer, sockaddr_in *addr) {
  timeval timeout;
  fd_set fdset;
  int i;
  socklen_t fromlen = sizeof(*addr);

  timeout.tv_sec = 0; // one second, 0 msec
  timeout.tv_usec = 100;

  FD_ZERO(&fdset);
  FD_SET(s, &fdset);

  i = select(s + 1, &fdset, 0, 0, &timeout);

  if (i > 0 && FD_ISSET(s, &fdset)) {
    char *buf = new char[1024];
    int res = recvfrom(s, buf, 1024, 0, (sockaddr *)addr, &fromlen);
    if (res == -1) {
      printf("recvfrom: Error\n");
      return -1;
    }
    (*buffer) = buf;
    delete buf;
    return res; // success: quantity of bytes
  }
  return 0;
}

int SendData(int s, string *buffer, sockaddr_in *addr) {
  socklen_t len = sizeof(*addr);
  int res = sendto(s, buffer->c_str(), buffer->length() + 1, 0,
                   (sockaddr *)addr, len);

  if (res == -1) {
    printf("Send packet error\n");
    return 0;
  }
  return res; // suxxess
}
