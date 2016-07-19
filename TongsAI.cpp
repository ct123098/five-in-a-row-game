#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <set>
#include <ctime>

using namespace std;

const int MAXN = 15;
int map[100][100];

namespace Yours
{
	void Put(int r, int c)
	{
		if(r < 0 || r >= MAXN || c < 0 || c >= MAXN)
			exit(0);
		map[r][c] = 2;
	}

	void Go()
	{
		int r, c;
		cin >> r >> c;
		Put(r, c);
	}
}

namespace Mine
{
	void Put(int r, int c)
	{
		map[r][c] = 1;
		cout << r << " " << c << endl;
	}

	void Go()
	{
		while(true)
		{
			int r = rand() % MAXN;
			int c = rand() % MAXN;
			if(map[r][c] == 0)
			{
				Put(r, c);
				return;
			}
		}
	}
}


int main()
{
	srand(time(0));
	memset(map, 0, sizeof(map));
	int first;
	cin >> first;
	if(first)
		Mine::Go();
	while(true)
	{
		Yours::Go();
		Mine::Go();
	}
	return 0;
}
