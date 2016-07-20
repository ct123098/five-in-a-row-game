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
FILE *Log;
#endif

double sqr(double x)
{ return x * x; }

double cube(double x)
{ return x * x * x; }

double biq(double x)
{ return x * x * x * x; }



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
	double Dweight, Aweight;

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
		Dweight = Aweight = 0;
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

							int xx, yy, side = 0;
							xx = i, yy = j, side--;
							while(getmap(xx, yy) == 0 || getmap(xx, yy) == type)
								side++, xx -= dx[k], yy -= dy[k];
							xx = x, yy = y, side--;
							while(getmap(xx, yy) == 0 || getmap(xx, yy) == type)
								side++, xx += dx[k], yy += dy[k];

							if(side + cnt < 5) continue;

							int rank = 0;
							if(getmap(i - dx[k], j - dy[k]) == 0)
								rank += 1;
							if(getmap(x + dx[k], y + dy[k]) == 0)
								rank += 1;
							if(getmap(i - dx[k], j - dy[k]) == 0) if(getmap(i - 2 * dx[k], j - 2 * dy[k]) == type)
								rank += 1;
							if(getmap(x + dx[k], y + dy[k]) == 0) if(getmap(x + 2 * dx[k], y + 2 * dy[k]) == type)
								rank += 1;

							if(rank <= 0) continue;

							if(getmap(i - dx[k], j - dy[k]) == 0)
								val[i - dx[k]][j - dy[k]] += 0.2 * sqr(1 + cnt);
							if(getmap(x + dx[k], y + dy[k]) == 0)
								val[x + dx[k]][y + dy[k]] += 0.2 * sqr(1 + cnt);
							if(getmap(i - dx[k], j - dy[k]) == 0) if(getmap(i - 2 * dx[k], j - 2 * dy[k]) == 0) if(rank + cnt < 5)
								val[i - 2 * dx[k]][j - 2 * dy[k]] += 0.65 * 0.2 * sqr(1 + cnt);
							if(getmap(x + dx[k], y + dy[k]) == 0) if(getmap(x + 2 * dx[k], y + 2 * dy[k]) == 0) if(rank + cnt < 5)
								val[x + 2 * dx[k]][y + 2 * dy[k]] += 0.65 * 0.2 * sqr(1 + cnt);

							v.push_back(Adj(Point(i, j), Point(x, y), k, cnt, rank + cnt));
						}
		sort(v.begin(), v.end(), cmp_rank);
	}

	void getForce(vector<Point> &p, vector<Adj> &a, int type)
	{
		double mxrank = a.size() ? a[0].r : 0;
		if(mxrank < 5) return;
		for(int i = 0; i < a.size(); i++)
		{
			if(a[i].r < mxrank) break;

			bool flag = false;
			if(getmap(a[i].fr.x - dx[a[i].d], a[i].fr.y - dy[a[i].d]) == 0) if(getmap(a[i].fr.x - 2 * dx[a[i].d], a[i].fr.y - 2 * dy[a[i].d]) == type)
				p.push_back(Point(a[i].fr.x - dx[a[i].d], a[i].fr.y - dy[a[i].d])), flag = true;
			if(getmap(a[i].to.x + dx[a[i].d], a[i].to.y + dy[a[i].d]) == 0) if(getmap(a[i].to.x + 2 * dx[a[i].d], a[i].to.y + 2 * dy[a[i].d]) == type)
				p.push_back(Point(a[i].to.x + dx[a[i].d], a[i].to.y + dy[a[i].d])), flag = true;

			if(flag) continue;

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
				if(getmap(i, j) == 0)
					Defv[i][j] = Attv[i][j] = 0.5 - 0.1 * rand() / RAND_MAX - 0.05 * max(abs(7 - i), abs(7 - j));
		getAdj(Def, Defv, 2);
		getAdj(Att, Attv, 1);
		getForce(forceDef, Def, 2);
		getForce(forceAtt, Att, 1);
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

		Dweight = max(0.1, 1.0 - 0.05 * Att.size());
		for(int i = 0; i < Def.size(); i++)
			if(Def[i].r >= 4)
				Dweight += 0.1;
			else if(Def[i].r >= 3)
				Dweight += 0.01;
			else
				break;

		Dweight = 0.5;//min(Dweight, 0.9);
		Aweight = 1.0 - Dweight;

		double best = -1;
		Point p;
		for(int i = 0; i < N; i++)
			for(int j = 0; j < N; j++)
				if(getmap(i, j) == 0 && Dweight * Defv[i][j] + Aweight * Attv[i][j] > best)
					best = Dweight * Defv[i][j] + Aweight * Attv[i][j], p = Point(i, j);
		return p;
	}

};

Board arena;

namespace Yours
{
	void Put(int r, int c)
	{
		if(r < 0 || r >= N || c < 0 || c >= N)
			exit(0);
		arena[r][c] = 2;
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
		arena[r][c] = 1;
		cout << r << " " << c << endl;
	}

	void Put(const Point &p)
	{
		Put(p.x, p.y);
	}

	void Go()
	{
		Point put = arena.makeDecision();

#ifdef Debug
		Log = fopen("log.txt", "a+");

		fprintf(Log, "Defv\n");
		fprintf(Log, "   ");
		for(int i = 0; i < N; i++)
			fprintf(Log, "  %c  ", 'A' + i);
		fprintf(Log, "\n");
		for(int i = 0; i < N; i++)
		{
			fprintf(Log, "%2d ", i + 1);
			for(int j = 0; j < N; j++)
				fprintf(Log, "%.2lf ", arena.Defv[i][j]);
			fprintf(Log, "\n");
		}
		fprintf(Log, "\n");

		fprintf(Log, "Attv\n");
		fprintf(Log, "   ");
		for(int i = 0; i < N; i++)
			fprintf(Log, "  %c  ", 'A' + i);
		fprintf(Log, "\n");
		for(int i = 0; i < N; i++)
		{
			fprintf(Log, "%2d ", i + 1);
			for(int j = 0; j < N; j++)
				fprintf(Log, "%.2lf ", arena.Attv[i][j]);
			fprintf(Log, "\n");
		}
		fprintf(Log, "\n");

		fprintf(Log, "Def\n");
		for(int i = 0; i < arena.Def.size(); i++)
			fprintf(Log, "%d %d to %d %d : rank = %d\n", arena.Def[i].fr.x, arena.Def[i].fr.y, arena.Def[i].to.x, arena.Def[i].to.y, arena.Def[i].r);
		fprintf(Log, "\n");

		fprintf(Log, "Att\n");
		for(int i = 0; i < arena.Att.size(); i++)
			fprintf(Log, "%d %d to %d %d : rank = %d\n", arena.Att[i].fr.x, arena.Att[i].fr.y, arena.Att[i].to.x, arena.Att[i].to.y, arena.Att[i].r);
		fprintf(Log, "\n");

		fprintf(Log, "Dweight = %.3lf\n", arena.Dweight);
		fprintf(Log, "Aweight = %.3lf\n", arena.Aweight);
		fprintf(Log, "\n");

		fclose(Log);
#endif

		Put(put);
	}
}


int main()
{
#ifdef Debug
	Log = fopen("Log.txt", "w");
	fclose(Log);
#endif

	srand(time(0));
	memset(arena.map, 0, sizeof(arena.map));
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
