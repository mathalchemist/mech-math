// =====================================
// Coder
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "BigInt.h"

void prn(BigInt * x)
{ int l = x->numDecimalDigits()+16; char * s = new char[l+1]; x->toString(s, l); printf("%s\n", s); delete s; }

int FileSize(char * fn)
{
FILE * f = fopen(fn, "r"); int d; int size = 0; while (!feof(f)) size += fread(&d, 1, 4, f); fclose(f); return size;
}

int pw(int x, int p)
{
	int r= 1;
	for (int i =1; i <=p; i++) r*=x; 
	return r;
}

int main()
{
    BigInt p, q, m, fi, e, d;
    FILE * f = fopen("numbers.txt", "r"); char numstr[256];
    fscanf(f, "%256s", numstr); p.fromString(numstr);
    fscanf(f, "%256s", numstr); q.fromString(numstr);
    fscanf(f, "%256s", numstr); m.fromString(numstr);
    fscanf(f, "%256s", numstr); fi.fromString(numstr);
    fscanf(f, "%256s", numstr); e.fromString(numstr);
    fscanf(f, "%256s", numstr); d.fromString(numstr);
    fclose(f);

    char sn[256]; char cn[256]; char un[256]; 
    printf("Enter source file name: "); scanf("%20s", sn);
    
    int Size = FileSize(sn);
	printf("filesize =%d bytes.\n", Size);

    strcpy(cn, sn); strcat(cn, ".rsa"); strcpy(un, sn); strcat(un, ".dec"); 
    
    FILE * sf = fopen(sn, "r"); FILE * cf = fopen(cn, "w+");
    if (!sf) { printf("s_error!\n"); return 0; }
    if (!cf) { printf("c_error!\n"); return 0; }  
    
	char ss[8];
memset(ss,0,8);
int sss = Size;
    for(int i = 0; i <8; i++)
	{
//	printf("d[%d] =%d", i,sss%26);
	ss[i] = 65 + sss%26;
		sss/=26;
	}

fwrite(ss, 8, 1, cf); // store original file size
    
    const int k = 16;
    const int kk = 57;
    printf("crypting...\n");
    while (!feof(sf))
    {
		unsigned char Data[k]; 
		memset(Data,0,k);
		fread(Data, 1, k, sf);
		BigInt R(0);
		BigInt Base(256);
		for (int i = 0; i < k; i++)
		{
			BigInt digit(Data[i]);
			R += (digit * Base.power(i));
		}
		BigInt x = R;
		BigInt y = (x.powerMod(e, m) % m); //  crypted number
		unsigned char CryptedData[kk];
		BigInt KBase(26);
		for (int i = 0; i < kk; i++)
		{
			CryptedData[i] = int((y%KBase)) + 65; 
			y /= KBase;
		}    
		fwrite(CryptedData, 1, kk, cf);
    }
    fclose(sf);
    fclose(cf);
    
    printf("uncrypting...\n");
    cf = fopen(cn, "r"); FILE * uf = fopen(un, "w+");
    if (!cf) { printf("c_error!!!\n"); return 0; }
    if (!uf) { printf("u_error!!!\n"); return 0; }
    
    fread(ss, 8, 1, cf);
	sss = 0;
	
    for(int i = 0; i <8; i++)
	{
		sss += (ss[i] - 65)*pw(26,i);
	}

printf("size = %d\n", sss);

int bp = 0;

    while (!feof(cf))
    {
		unsigned char CryptedData[kk]; 
		fread(CryptedData, 1, kk, cf);
		BigInt R(0);
		BigInt KBase(26);
		for (int i = 0; i < kk; i++)
		{
			BigInt digit(int(CryptedData[i]-65));
			R += (digit * KBase.power(i));
		}
		BigInt x = R;
		BigInt y = (x.powerMod(d, m) % m); // decoding
	
		unsigned char UncryptedData[k];
		memset(UncryptedData,0,k);
		BigInt Base(256);
		for (int i = 0; i < k; i++)
		{
			UncryptedData[i] = (y%Base); 
			y /= Base;
		}    
		bp += k;
		//printf("bp=%d\n", bp);
		if (bp > Size) {
		//printf("end, garbage len = %d\n",k-(bp-Size));
		fwrite(UncryptedData, 1, k-(bp-Size), uf); break; } else 
		fwrite(UncryptedData, 1, k, uf);
    }
    fclose(cf);
    fclose(uf);

    printf("done.\n");
    
    return 0;
}

