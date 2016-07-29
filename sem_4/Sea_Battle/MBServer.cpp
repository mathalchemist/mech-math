// =====================================
// servak
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <X11/keysym.h>
#include "XLib.h"

#include "Common.h"

int s1 = 0; // Socket

bool MyTurn = true;

// MAIN ==================================
int main(int argc, char **argv) {
  printf("Welcome to the game 'Sea battle'. This is a SERVER.\n");
  printf("Command line syntax: MBSrv [port number]\n");
  printf("Controls: WASD --- cursor movement,\nO --- fire,\nSPACE --- "
         "set/remove ship.\n");
  printf("Press 'R' key when ready...\n");

  if (signal(SIGPIPE, &SigHandler) == SIG_ERR) {
    printf("Error: cannot set signal handler!\n");
    return 0;
  }
  int Port = 1234; // def port
  if (argc > 1)
    Port = atoi(argv[1]);
  printf("Using port #%d.\n", Port);

  // Create a socket
  int s0 = socket(AF_INET, SOCK_STREAM, 0);
  if (s0 < 0) {
    printf("Error: cannot create a socket.\n");
    return 0;
  }

  sockaddr_in MyAddr;
  memset(&MyAddr, 0, sizeof(sockaddr_in));
  MyAddr.sin_family = AF_INET;
  MyAddr.sin_port = htons(Port);
  MyAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  // Bind a socket to the address
  if (bind(s0, (sockaddr *)&MyAddr, sizeof(MyAddr)) < 0) {
    printf("Error: cannot bind a socket.\n");
    close(s0);
    return 0;
  }
  printf("Waiting for the client...\n");
  linger linger_opt = {1, 0};
  setsockopt(s0, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));
  if (listen(s0, 1) < 0) {
    printf("Error: cannot listen on the socket.\n");
    close(s0);
    return 0;
  }

  sockaddr_in CliAddr;
  socklen_t AddrLen;
  s1 = accept(s0, (sockaddr *)&CliAddr, &AddrLen);
  if (s1 < 0) {
    printf("Error: cannot accept connection.\n");
    close(s0);
    return 0;
  }

  printf("Accepted connection on port %d\n", ntohs(CliAddr.sin_port));

  close(s0); // Close the listen socket

  try {
    char *m6 = (char *)"Sea battle: SERVER";
    XWin = new TXWin(m6, 100, 100, XWW, XWH, RGB_565);
    while (Working) {
      int EType;
      if (XWin->GetEvent(&EType, &KInfo, &MInfo)) {
        if (EType == ET_EXPOSE)
          ExposeHandler();
        if (EType == ET_KEYPRESS && KeyHandler(s1) == 1)
          break;
      } else
        NetworkHandler(s1);
    }
    delete XWin;
  } catch (const TXWin::Ex &e) {
    printf("Error: %s.\n", e.Msg);
    return 1;
  }
  return 0;
}

#include "Handlers.h"
