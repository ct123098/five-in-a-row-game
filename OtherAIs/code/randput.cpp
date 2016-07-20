#include <iostream>
#include <cstring>
#include <cstdlib>
#include "time.h"

using namespace std;

const int MAXN = 15;
int map[100][100];

void myPut(int r, int c)
{
	map[r][c] = 1;
	cout << r << " " << c << endl;
}

void youPut(int r, int c)
{
	if (r<0 || r>=MAXN || c<0 || c>=MAXN)
		exit(0);
	map[r][c] = 2;
}

void myGo()
{
	/*
	for (int i = 0; i < MAXN; ++i)
		for (int j = 0; j < MAXN; ++j)
			if (map[i][j] == 0)
			{
				myPut(i, j);
				return ;
			}
	*/
	while (true)
	{
		int r = rand() % MAXN;
		int c = rand() % MAXN;
		if (map[r][c] == 0)
		{
			myPut(r, c);
			return ;
		}
	}
}

void youGo()
{
	int r, c;
	cin >> r >> c;
	youPut(r, c);
}

int main()
{
	srand(time(0));
	memset(map, 0, sizeof(map));
	int first;
	cin >> first;
	if (first)
		myGo();
	while (true)
	{
		youGo();
		myGo();
	}
	return 0;
}
