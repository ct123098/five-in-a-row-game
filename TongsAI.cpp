//#define Debug

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <set>
#include <ctime>
#include <windows.h>


using namespace std;

const int N = 15;
const int dx[] = {1, 1, 1, 0, 0, -1, -1, -1};
const int dy[] = {1, 0, -1, 1, -1, 1, 0, -1};

#ifdef Debug
FILE *log = fopen("log.txt", "w");
#endif

bool in(int x, int y)
{
	return 0 <= x && x < N && 0 <= y && y < N;
}

struct Point
{
	int x, y;

	Point(int _x = 0, int _y = 0) : x(_x), y(_y)
	{ }
};

bool in(const Point &p)
{
	return in(p.x, p.y);
}

struct Adj
{
	Point fr, to;
	int d, c, r;

	Adj(Point _f, Point _t, int _d, int _c, int _r) : fr(_f), to(_t), d(_d), c(_c), r(_r)
	{ }
};

bool cmp_rank(const Adj &a, const Adj &b)
{
	return a.r > b.r;
}

struct Board
{
	int map[N][N];
	vector<Adj> Def, Att;
	vector<Point> forceDef, forceAtt;
	double Defv[N][N], Attv[N][N];

	int *operator [](int x)
	{
		return map[x];
	}

	int getmap(int x, int y)
	{
		if(!in(x, y)) return -1;
		return map[x][y];
	}

	void clear()
	{
		Def.clear(), Att.clear();
		forceDef.clear(), forceAtt.clear();
		for(int i = 0; i < N; i++)
			for(int j = 0; j < N; j++)
				Defv[i][j] = Attv[i][j] = 0;
	}

	void getAdj(vector<Adj> &v, double val[][N], int type)
	{
		for(int i = 0; i < N; i++)
			for(int j = 0; j < N; j++)
				if(map[i][j] == type)
					for(int k = 0; k < 4; k++)
						if(getmap(i - dx[k], j - dy[k]) != type)
						{
							int x = i, y = j, cnt = 1;
							while(getmap(x, y) == type)
							{
								x += dx[k];
								y += dy[k];
								cnt++;
							}
							x -= dx[k], y -= dy[k], cnt--;

							int rank = 0;
							if(getmap(i - dx[k], j - dy[k]) == 0)
								rank += 1;
							if(getmap(x + dx[k], y + dy[k]) == 0)
								rank += 1;
							if(getmap(i - dx[k], j - dy[k]) == 0) if(getmap(i - 2 * dx[k], j - 2 * dy[k]) == type)
								rank += 1;
							if(getmap(x + dx[k], y + dy[k]) == 0) if(getmap(x + 2 * dx[k], y + 2 * dy[k]) == type)
								rank += 1;

							if(getmap(i - dx[k], j - dy[k]) == 0)
								val[i - dx[k]][j - dy[k]] += rank + cnt;
							if(getmap(x + dx[k], y + dy[k]) == 0)
								val[x + dx[k]][y + dy[k]] += rank + cnt;
							if(getmap(i - dx[k], j - dy[k]) == 0) if(getmap(i - 2 * dx[k], j - 2 * dy[k]) == 0) if(rank + cnt < 5)
								val[i - 2 * dx[k]][j - 2 * dy[k]] += 0.6 * (rank + cnt);
							if(getmap(x + dx[k], y + dy[k]) == 0) if(getmap(x + 2 * dx[k], y + 2 * dy[k]) == 0) if(rank + cnt < 5)
								val[x + 2 * dx[k]][y + 2 * dy[k]] += 0.6 * (rank + cnt);

							if(rank <= 0) continue;
							v.push_back(Adj(Point(i, j), Point(x, y), k, cnt, rank + cnt));
						}
		sort(v.begin(), v.end(), cmp_rank);
	}

	void getForce(vector<Point> &p, vector<Adj> &a)
	{
		double mxrank = a.size() ? a[0].r : 0;
		if(mxrank < 5) return;
		for(int i = 0; i < a.size(); i++)
		{
			if(mxrank < mxrank) break;
			if(getmap(a[i].fr.x - dx[a[i].d], a[i].fr.y - dy[a[i].d]) == 0)
				p.push_back(Point(a[i].fr.x - dx[a[i].d], a[i].fr.y - dy[a[i].d]));
			if(getmap(a[i].to.x + dx[a[i].d], a[i].to.y + dy[a[i].d]) == 0)
				p.push_back(Point(a[i].to.x + dx[a[i].d], a[i].to.y + dy[a[i].d]));
		}
	}

	void calcValues()
	{
		clear();
		for(int i = 0; i < N; i++)
			for(int j = 0; j < N; j++)
				Defv[i][j] = Attv[i][j] = 0.5 - 0.1 * rand() / RAND_MAX - 0.05 * max(abs(7 - i), abs(7 - j));
		getAdj(Def, Defv, 2);
		getAdj(Att, Attv, 1);
		getForce(forceDef, Def);
		getForce(forceAtt, Att);
	}

	Point makeDecision()
	{
		calcValues();
		int mxdrank = Def.size() ? Def[0].r : 0;
		int mxarank = Att.size() ? Att[0].r : 0;

		if(mxarank >= 5 && mxdrank <= mxarank && forceAtt.size())
			return forceAtt[0];
		if(mxdrank >= 5 && forceDef.size())
			return forceDef[0];


		double best = -1, dwei = 0.5 + 0.1 * mxdrank, awei = 1.0 - dwei;
		Point p;
		for(int i = 0; i < N; i++)
			for(int j = 0; j < N; j++)
				if(getmap(i, j) == 0 && dwei * Defv[i][j] + awei * Attv[i][j] > best)
					best = dwei * Defv[i][j] + awei * Attv[i][j], p = Point(i, j);
		return p;
	}

};

Board Arena;

namespace Yours
{
	void Put(int r, int c)
	{
		if(r < 0 || r >= N || c < 0 || c >= N)
			exit(0);
		Arena[r][c] = 2;
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
		Arena[r][c] = 1;
		cout << r << " " << c << endl;
	}

	void Put(const Point &p)
	{
		Put(p.x, p.y);
	}

	void Go()
	{
		Point put = Arena.makeDecision();

#ifdef Debug


		fprintf(log, "Defv\n");
		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
				fprintf(log, "%.2lf ", Arena.Defv[i][j]);
			fprintf(log, "\n");
		}
		fprintf(log, "\n");
		fprintf(log, "Attv\n");
		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
				fprintf(log, "%.2lf ", Arena.Attv[i][j]);
			fprintf(log, "\n");
		}
		fprintf(log, "\n");

#endif

		Put(put);
	}
}


int main()
{
	srand(time(0));
	memset(Arena.map, 0, sizeof(Arena.map));
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
