// =====================================
// client
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
#include <netdb.h>
#include <X11/keysym.h>
#include "XLib.h"

#include "Common.h"

int s0 = 0; // Socket
bool MyTurn = false;

// MAIN ==================================
int main(int argc, char **argv) {
  printf("Welcome to the game 'Sea battle'. This is a CLIENT.\n");
  printf("Command line syntax: MBCli [host address] [port number]\n");
  printf("CONTROLS:\nShift+Q -- quit,\nWASD --- cursor movement,\nO --- "
         "fire,\nSPACE --- set/remove ship.\n");
  printf("Press 'Shift+R' when ready...\n");

  if (signal(SIGPIPE, &SigHandler) == SIG_ERR) {
    printf("Error: cannot set signal handler!\n");
    return 0;
  }

  // Create a socket
  s0 = socket(AF_INET, SOCK_STREAM, 0);
  if (s0 < 0) {
    printf("Error: cannot create a socket.\n");
    return 0;
  }

  // Fill in the address of server
  sockaddr_in SrvAddr;
  memset(&SrvAddr, 0, sizeof(SrvAddr));

  const char *HostName = "localhost";
  if (argc >= 2)
    HostName = argv[1];

  // Resolve the server address (convert from symbolic name to IP number)
  hostent *Host = gethostbyname(HostName);
  if (!Host) {
    printf("Error: cannot get host address.\n");
    close(s0);
    return 0;
  }

  SrvAddr.sin_family = AF_INET;
  int Port = 1234;
  if (argc >= 3)
    Port = atoi(argv[2]);
  SrvAddr.sin_port = htons(Port);

  memmove(&(SrvAddr.sin_addr.s_addr), Host->h_addr_list[0], 4);
  printf("Connecting to %s...\n", HostName);

  // Connect to a remote server
  if (connect(s0, (struct sockaddr *)&SrvAddr, sizeof(SrvAddr)) < 0) {
    printf("Error: cannot connect to remote host.\n");
    close(0);
    return 0;
  }

  try {
    char *m7 = (char *)"Sea battle: CLIENT";
    XWin = new TXWin(m7, 100, 100, XWW, XWH, RGB_565);
    while (Working) {
      int EType;
      if (XWin->GetEvent(&EType, &KInfo,
                         &MInfo)) // message from window, mouse, kbd
      {
        if (EType == ET_EXPOSE)
          ExposeHandler();
        if (EType == ET_KEYPRESS && KeyHandler(s0) == 1)
          break;
      } else
        NetworkHandler(s0); // network event processing
    }
    delete XWin; // kill window
  } catch (const TXWin::Ex &e) // bug report
  {
    printf("Error: %s.\n", e.Msg);
    return 1;
  }
  return 0;
}

#include "Handlers.h"
