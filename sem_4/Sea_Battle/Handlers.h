// =====================================
// Different handlers: net, kbd, redraw
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

// Handlers.h
void NetworkHandler(int Socket) {
  // network processing
  if (!WaitMessage(Socket, RecvBuf))
    return;

  if (RecvBuf[0] == 1024 && RecvBuf[1] == 1023) // end signal
  {
    GameState = GS_END;
    EndedByOtherSide = true;
    return;
  }
  switch (GameState) {
  case GS_INPUT:
    if (RecvBuf[101] == 0x00FD) {
      memmove(HisField, RecvBuf, 100 * sizeof(int));
      HeIsReady = true;
    }
    break;
  case GS_READY:
    if (RecvBuf[101] == 0x00FD) {
      memmove(HisField, RecvBuf, 100 * sizeof(int));
      HeIsReady = true;
      GameState = GS_GAME;
    }
    break;
  case GS_GAME:
    if (MyField[RecvBuf[0]][RecvBuf[1]] == CS_SHIP) {
      MyField[RecvBuf[0]][RecvBuf[1]] = CS_HIT;
      HisHits++;
    } else {
      MyField[RecvBuf[0]][RecvBuf[1]] = CS_MISS;
      MyTurn = true;
    }
    if (HisHits == 20) {
      GameState = GS_END;
      Victory = false;
      close(Socket);
    }
    break;
  case GS_END:
    break;
  }
  ExposeHandler();
}

void DrawCursor(int Shift) {
  XWin->DrawRect(Shift + 20 + CursorX * SIZE, 20 + CursorY * SIZE,
                 Shift + 20 + (CursorX + 1) * SIZE, 20 + (CursorY + 1) * SIZE,
                 RGB(255, 0, 0));
}

void DrawGrid(int Shift) {
  for (int x = 0; x < 10; x++)
    for (int y = 0; y < 10; y++)
      XWin->DrawRect(Shift + 20 + x * SIZE, 20 + y * SIZE,
                     Shift + 20 + (x + 1) * SIZE, 20 + (y + 1) * SIZE,
                     RGB(128, 128, 128));
}

void DrawField(int *Field, int Shift) {
  DrawGrid(Shift);
  for (int x = 0; x < 10; x++)
    for (int y = 0; y < 10; y++)
      switch (Field[10 * x + y]) {
      case CS_SHIP:
        if (Shift != 0)
          break;
        XWin->DrawRect(Shift + 20 + x * SIZE, 20 + y * SIZE,
                       Shift + 20 + (x + 1) * SIZE, 20 + (y + 1) * SIZE,
                       RGB(255, 255, 255));
        XWin->DrawRect(Shift + 20 + x * SIZE + 1, 20 + y * SIZE + 1,
                       Shift + 20 + (x + 1) * SIZE - 1, 20 + (y + 1) * SIZE - 1,
                       RGB(255, 255, 255));
        break;
      case CS_HIT:
        XWin->DrawLine(Shift + 20 + x * SIZE, 20 + y * SIZE,
                       Shift + 20 + (x + 1) * SIZE, 20 + (y + 1) * SIZE,
                       RGB(255, 255, 255));
        XWin->DrawLine(Shift + 20 + (x + 1) * SIZE, 20 + y * SIZE,
                       Shift + 20 + x * SIZE, 20 + (y + 1) * SIZE,
                       RGB(255, 255, 255));
        break;
      case CS_MISS:
        XWin->DrawLine(Shift + 20 + x * SIZE + D, 20 + y * SIZE + D,
                       Shift + 20 + (x + 1) * SIZE - D, 20 + (y + 1) * SIZE - D,
                       RGB(255, 255, 255));
        XWin->DrawLine(Shift + 20 + (x + 1) * SIZE - D, 20 + y * SIZE + D,
                       Shift + 20 + x * SIZE + D, 20 + (y + 1) * SIZE - D,
                       RGB(255, 255, 255));
        break;
      }
}

void ExposeHandler() {
  XWin->FillRect(0, 0, XWW, XWH);
  if (GameState == GS_INPUT) {
    char *m2 =
        (char *)"Set your ships: WASD -- cursor move, SPACE -- set/erase";
    XWin->Write(20, 10, m2);
    DrawField((int *)MyField, 0);
    DrawCursor(0);
    char *m1 = (char *)"Hurry, your opponent is ready!";
    if (HeIsReady)
      XWin->Write(20, 20 + 10 * SIZE + 10, m1);
  } else if (GameState == GS_READY) {
    char *m = (char *)"Waiting for opponent...";
    XWin->Write(200, 145, m);
  } else if (GameState == GS_GAME) {
    DrawField((int *)MyField, 0);
    DrawField((int *)HisField, 200);

    DrawCursor(200);

    char str[256];
    if (MyTurn)
      sprintf(str, "YOUR TURN!     Your hits: %02d  his hits: %02d", MyHits,
              HisHits);
    else
      sprintf(str, "PLEASE WAIT    Your hits: %02d  his hits: %02d", MyHits,
              HisHits);
    XWin->Write(20, 10, str);
  } else

      if (GameState == GS_END) {
    if (EndedByOtherSide == true) {
      char *m10 = (char *)"Game ended by another player";
      XWin->Write(20, 20, m10);

      return;
    }
    if (EarlyEnd == true) {
      char *m10 = (char *)"Game interrupted.";
      XWin->Write(20, 20, m10);
      return;
    }
    if (Victory)
      for (int i = 0; i < 200; i++) {
        XWin->SetColor(RGB(random() % 256, random() % 256, random() % 256));
        char *m5 = (char *)"victory!";
        XWin->Write(random() % XWW, random() % XWH, m5);
      }
    else {
      char *m4 = (char *)"YOU ARE DEFEATED!";
      XWin->Write(200, 145, m4);
    }
  }
}

bool uf(int i, int j) {
  if (j > 0)
    if (MyField[j - 1][i] == CS_SHIP)
      return false;
  return true;
}

bool df(int i, int j) {
  if (j < 9)
    if (MyField[j + 1][i] == CS_SHIP)
      return false;
  return true;
}

bool rf(int i, int j) {
  if (i < 9)
    if (MyField[j][i + 1] == CS_SHIP)
      return false;
  return true;
}

bool lf(int i, int j) {
  if (i > 0)
    if (MyField[j][i - 1] == CS_SHIP)
      return false;
  return true;
}

bool CheckField() {
  int one = 0;
  int two = 0;
  int three = 0;
  int four = 0;
  // count 1-cell
  for (int x = 0; x < 10; x++)
    for (int y = 0; y < 10; y++)
      if (MyField[x][y] == CS_SHIP) {
        if (lf(x, y) && rf(x, y) && uf(x, y) && df(x, y))
          one++;
      }
  if (one != 4)
    return false;

  // count 2-cell
  // horz
  for (int y = 0; y < 10; y++)
    for (int x = 0; x < 9; x++)
      if (MyField[x][y] == CS_SHIP) {
        if (lf(x, y) && !rf(x, y) && uf(x, y) && df(x, y) && rf(x + 1, y))
          two++;
      }
  for (int y = 0; y < 9; y++)
    for (int x = 0; x < 10; x++)
      if (MyField[x][y] == CS_SHIP) {
        if (lf(x, y) && rf(x, y) && uf(x, y) && !df(x, y) && df(x, y + 1))
          two++;
      }

  if (two != 3)
    return false;

  // count 3-cell
  // horz
  for (int y = 0; y < 10; y++)
    for (int x = 0; x < 8; x++)
      if (MyField[x][y] == CS_SHIP) {
        if (lf(x, y) && !rf(x, y) & !rf(x + 1, y) && uf(x, y) && df(x, y) &&
            rf(x + 2, y))
          three++;
      }
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 10; x++)
      if (MyField[x][y] == CS_SHIP) {
        if (lf(x, y) && rf(x, y) && uf(x, y) && !df(x, y) && !df(x, y + 1) &&
            df(x, y + 2))
          three++;
      }

  if (three != 2)
    return false;
  // count 2-cell
  // horz
  for (int y = 0; y < 10; y++)
    for (int x = 0; x < 7; x++)
      if (MyField[x][y] == CS_SHIP) {
        if (lf(x, y) && !rf(x, y) & !rf(x + 1, y) && !rf(x + 2, y) &&
            uf(x, y) && df(x, y) && rf(x + 3, y))
          four++;
      }
  for (int y = 0; y < 7; y++)
    for (int x = 0; x < 10; x++)
      if (MyField[x][y] == CS_SHIP) {
        if (lf(x, y) && rf(x, y) && uf(x, y) && !df(x, y) && !df(x, y + 1) &&
            !df(x, y + 2) && df(x, y + 3))
          four++;
      }

  if (four != 1)
    return false;
  // ufff... all ok
  return true;
}

int KeyHandler(int Socket) {
  // if (KInfo.KeyCode == XK_Q) return 1; // quit
  if (KInfo.KeyCode == XK_Q) {
    if (GameState == GS_END) {
      Working = false;
      return 1;
    }
    SendBuf[0] = 1024;
    SendBuf[1] = 1023;
    write(Socket, SendBuf, 3 * sizeof(int));
    Victory = false;
    EarlyEnd = true;
    GameState = GS_END;
  }
  if (GameState == GS_INPUT || (GameState == GS_GAME && MyTurn)) {
    if (KInfo.KeyCode == XK_w && CursorY > 0)
      CursorY--;
    if (KInfo.KeyCode == XK_s && CursorY < 9)
      CursorY++;
    if (KInfo.KeyCode == XK_a && CursorX > 0)
      CursorX--;
    if (KInfo.KeyCode == XK_d && CursorX < 9)
      CursorX++;
  }

  if (GameState == GS_INPUT && KInfo.KeyCode == XK_space)
    if (MyField[CursorX][CursorY] == CS_EMPTY)
      MyField[CursorX][CursorY] = CS_SHIP;
    else
      MyField[CursorX][CursorY] = CS_EMPTY;

  if (KInfo.KeyCode == XK_R && GameState == GS_INPUT) {
    int NumFilled = 0;
    for (int x = 0; x < 10; x++)
      for (int y = 0; y < 10; y++)
        if (MyField[x][y] == CS_SHIP)
          NumFilled++;
    if (NumFilled != 20) {
      printf("Wrong number of ships. Check your position.\n");
      return 0;
    }
    if (!CheckField()) {
      printf("Wrong position!. Check your position.\n");
      return 0;
    }
    // we think that field is ok.
    memmove(SendBuf, MyField, 100 * sizeof(int));
    SendBuf[101] = 0x00FD;
    write(Socket, SendBuf, 102 * sizeof(int));
    if (HeIsReady) // skip ready state
      GameState = GS_GAME;
    else
      GameState = GS_READY;
  }
  if (MyTurn && KInfo.KeyCode == XK_o && GameState == GS_GAME &&
      HisField[CursorX][CursorY] != CS_HIT &&
      HisField[CursorX][CursorY] != CS_MISS) {
    // send fire command
    SendBuf[0] = CursorX;
    SendBuf[1] = CursorY;

    if (HisField[CursorX][CursorY] == CS_SHIP) {
      HisField[CursorX][CursorY] = CS_HIT;
      MyHits++;
    } else {
      HisField[CursorX][CursorY] = CS_MISS;
      MyTurn = false; // pass turn to him
    }
    write(Socket, SendBuf, 3 * sizeof(int));
    if (MyHits == 20) {
      Victory = true;
      GameState = GS_END;
      close(Socket);
    }
  }
  ExposeHandler();

  return 0;
}

bool WaitMessage(int Socket, int *Buffer) {
  fd_set DSet;
  FD_ZERO(&DSet);
  FD_SET(Socket, &DSet);
  timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 100000;
  if (select(Socket + 1, &DSet, 0, 0, &tv) > 0) {
    read(Socket, Buffer, 1024);
    return true;
  } else
    return false;
}

void SigHandler(int SigID) {
  printf("sighandler(): Broken pipe. SID: %d", SigID);
}
