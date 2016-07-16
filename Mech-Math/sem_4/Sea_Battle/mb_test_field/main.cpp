bool uf(int i,int j)
{
	if (j > 0)
		if (MyField[j-1][i] == CS_SHIP) return false;
	return true;
}

bool df(int i,int j)
{
	if (j < 9)
		if (MyField[j+1][i] == CS_SHIP) return false;
	return true;
}

bool rf(int i,int j)
{
	if (i < 9)
		if (MyField[j][i+1] == CS_SHIP) return false;
	return true;
}

bool lf(int i,int j)
{
	if (i > 0)
		if (MyField[j][i-1] == CS_SHIP) return false;
	return true;
}

bool CheckField()
{
	int one = 0;
	int two = 0;
	int three = 0;
	int four = 0;
	
	// count 1-cell
	for (int x =0; x < 10; x++)
	for(int y =0 ;y <10;y++)
	if (MyField[y][x] == CS_SHIP)
	{
		if (lf(x,y)&&rf(x,y)&&uf(x,y)&&df(x,y)) one++;
	}
	
	printf("one=%d", one);
	if (one!= 4) return false;

	// count 2-cell
	//horz
	for(int y =0 ;y <10;y++)
	for (int x =0; x < 9; x++)
	if (MyField[y][x] == CS_SHIP)
	{
		if (lf(x,y)&& !rf(x,y) && uf(x,y)&&df(x,y) && rf(x+1,y)) two++;
	}
	for(int y =0 ;y <9;y++)
	for (int x =0; x < 10; x++)
	if (MyField[y][x] == CS_SHIP)
	{
		if (lf(x,y)&& rf(x,y) && uf(x,y)&& !df(x,y) && df(x,y+1)) two++;
	}
	
	printf("two=%d", two);
	if (two != 3) return false;

	// count 3-cell
	//horz
	for(int y =0 ;y <10;y++)
	for (int x =0; x < 8; x++)
	if (MyField[y][x] == CS_SHIP)
	{
		if (lf(x,y)&& !rf(x,y) & !rf(x+1,y) && uf(x,y)&&df(x,y) && rf(x+2,y)) three++;
	}
	for(int y =0 ;y <8;y++)
	for (int x =0; x < 10; x++)
	if (MyField[y][x] == CS_SHIP)
	{
		if (lf(x,y)&& rf(x,y) && uf(x,y)&& !df(x,y) && !df(x,y+1) && df(x,y+2)) three++;
	}
	
	printf("three=%d", three);
	if (three != 2) return false;
	// count 2-cell
	//horz
	for(int y =0 ;y <10;y++)
	for (int x =0; x < 7; x++)
	if (MyField[y][x] == CS_SHIP)
	{
		if (lf(x,y)&& !rf(x,y) & !rf(x+1,y) && !rf(x+2,y) && uf(x,y)&&df(x,y) && rf(x+3,y)) four++;
	}
	for(int y =0 ;y <7;y++)
	for (int x =0; x < 10; x++)
	if (MyField[y][x] == CS_SHIP)
	{
		if (lf(x,y)&& rf(x,y) && uf(x,y)&& !df(x,y) && !df(x,y+1) && !df(x,y+2) && df(x,y+3)) four++;
	}
	
	printf("four=%d", four);
	if (four != 1) return false;
	printf("%d, %d, %d, %d\n", one, two, three, four);
}
