// =====================================
// Network Game Bulls & Cows
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

const int MAX = 64;

void SigHandler(int);
bool WaitMessage(int Socket, int *Buffer);
bool GetAnswer(int N, int GN, int *B, int *C);

void ExtractIP(char *A, char *IP) {
  for (int i = 0; i < 4; i++)
    IP[i] = A[i];
}

int main(int argc, char *argv[]) {
  printf("Welcome to the game 'Bulls & Cows'. This is a CLIENT.\n");
  printf("Command line syntax: BCC [host address] [port number]\n");
  if (signal(SIGPIPE, &SigHandler) == SIG_ERR) {
    printf("Error: cannot set signal handler!\n");
    return 0;
  }

  // Create a socket
  int s0 = socket(AF_INET, SOCK_STREAM, 0);
  if (s0 < 0) {
    printf("Error: cannot create a socket.\n");
    return 0;
  }

  // Fill in the address of server
  sockaddr_in SrvAddr;
  memset(&SrvAddr, 0, sizeof(SrvAddr));

  char *HostName = "localhost";
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

  char IP[4];
  ExtractIP(Host->h_addr_list[0], IP);
  printf("You are connected to server at IP %d.%d.%d.%d on port %d.\n", IP[0],
         IP[1], IP[2], IP[3], Port);

  int MyNumber; // Number of server
  int MyGuess;  // Guessed number of client

  do {
    printf("Enter number:");
    scanf("%d", &MyNumber);
  } while (MyNumber < 1023 || MyNumber > 9876);

  // program cycle
  int SendBuffer[MAX];
  int RecvBuffer[MAX];
  int TN = 0;
  while (true) {
    printf("=============================\nGuess opponent's number: ");
    scanf("%d", &MyGuess);
    MyGuess = MyGuess % 10000;
    if (!MyGuess)
      break;
    TN++;
    SendBuffer[0] = MyNumber; // Our RIGHT number (Server)
    SendBuffer[1] = MyGuess;  // Guessed number of client
    write(s0, SendBuffer, MAX * sizeof(int));

    printf("Waiting for opponent's reply...\n");
    if (!WaitMessage(s0, RecvBuffer))
      break;

    int HisNumber = RecvBuffer[0]; // Right client's number
    int HisGuess = RecvBuffer[1];
    int B = 0;
    int C = 0;
    if (!GetAnswer(HisNumber, MyGuess, &B, &C)) {
      printf("Wrong number!!!\n");
      break;
    }
    printf("Opponent's guess: %d. Your try: %d bulls, %d cows.\n", HisGuess, B,
           C);
    if (HisGuess == MyNumber)
      printf("GAME OVER: Your opponent wins at the %d-th turn.", TN);
    if (HisNumber == MyGuess)
      printf("CONGRATULATIONS: You win wins at the %d-th turn!.", TN);
  }
  close(s0);
  return 0;
}

bool WaitMessage(int Socket, int *Buffer) {
  fd_set DSet;
  FD_ZERO(&DSet);
  FD_SET(Socket, &DSet);
  timeval tv;
  tv.tv_sec = 100;
  tv.tv_usec = 0;
  if (select(Socket + 1, &DSet, 0, 0, &tv) > 0) {
    read(Socket, Buffer, 1024);
    return true;
  } else {
    printf("Timeout or server fell asleep...\n");
    return false;
  }
}

bool GetAnswer(int N, int GN, int *B, int *C) {
  int d0 = N % 10;
  N /= 10;
  int d1 = N % 10;
  N /= 10;
  int d2 = N % 10;
  N /= 10;
  int d3 = N % 10;

  if (d0 == d1 || d0 == d2 || d0 == d3 || d1 == d2 || d1 == d3 || d2 == d3)
    return false;

  int gd0 = GN % 10;
  GN /= 10;
  int gd1 = GN % 10;
  GN /= 10;
  int gd2 = GN % 10;
  GN /= 10;
  int gd3 = GN % 10;

  if (gd0 == gd1 || gd0 == gd2 || gd0 == gd3 || gd1 == gd2 || gd1 == gd3 ||
      d2 == d3)
    return false;

  int b = 0;
  int c = 0;
  if (gd0 == d0)
    b++;
  if (gd1 == d1)
    b++;
  if (gd2 == d2)
    b++;
  if (gd3 == d3)
    b++;
  if ((gd0 != d0) && (gd0 == d1 || gd0 == d2 || gd0 == d3))
    c++;
  if ((gd1 != d1) && (gd1 == d0 || gd1 == d2 || gd1 == d3))
    c++;
  if ((gd2 != d2) && (gd2 == d0 || gd2 == d1 || gd2 == d3))
    c++;
  if ((gd3 != d3) && (gd3 == d0 || gd3 == d1 || gd3 == d2))
    c++;
  *B = b;
  *C = c;
  return true;
}

void SigHandler(int SigID) { printf("Broken pipe. SID: %d", SigID); }
