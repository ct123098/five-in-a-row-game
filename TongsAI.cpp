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
int Round = 0;

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
	{}
	
	bool operator <(const Point &a) const
	{
		if(x != a.x)
			return x < a.x;
		return y < a.y;
	}
};

bool in(const Point &p)
{
	return in(p.x, p.y);
}

struct Adj
{
	Point fr, to;
	int d, c, r;
	vector<Point> e;
	
	Adj(Point _f, Point _t, int _d, int _c, int _r, vector<Point> _e)
			: fr(_f), to(_t), d(_d), c(_c), r(_r), e(_e)
	{}
};

bool cmp_rank(const Adj &a, const Adj &b)
{
	return a.r > b.r;
}

/*
bool cmp_val(const Point &a, const Point &b)
{
	return true;
}
*/
struct Board
{
	int map[N][N];
	vector<Point> forceDef, forceAtt;
	double Defv[N][N], Attv[N][N];
	double Dweight, Aweight;
	int mxDRank, mxARank;
	
	int *operator [](int x)
	{
		return map[x];
	}
	
	int getmap(int x, int y) const
	{
		if(!in(x, y))
			return -1;
		return map[x][y];
	}
	
	double getval(int x, int y) const
	{
		return Dweight * Defv[x][y] + Aweight * Attv[x][y];
	}

	void clear()
	{
		forceDef.clear(), forceAtt.clear();
		for(int i = 0; i < N; i++)
			for(int j = 0; j < N; j++)
				Defv[i][j] = Attv[i][j] = 0;
		Dweight = Aweight = 0;
		mxDRank = mxARank = 0;
	}
	
	void getAdj(vector<Point> &p, double val[][N], int &mx, int type)
	{
		
		for(int i = 0; i < N; i++)
			for(int j = 0; j < N; j++)
				if(map[i][j] == 0)
					for(int k = 0; k < 4; k++)
					{
						int cnt = 1, side = 0, rank = 0;
						int x1 = i, y1 = j;
						while(getmap(x1 - dx[k], y1 - dy[k]) == type)
							x1 -= dx[k], y1 -= dy[k], cnt++;
						int x2 = i, y2 = j;
						while(getmap(x2 + dx[k], y2 + dy[k]) == type)
							x2 += dx[k], y2 += dy[k], cnt++;
						if(cnt <= 1)
							continue;
						
						int xx, yy;
						xx = x1, yy = y1;
						while(getmap(xx - dx[k], yy - dy[k]) == type || getmap(xx - dx[k], yy - dy[k]) == 0)
							xx -= dx[k], yy -= dy[k], side++;
						xx = x2, yy = y2;
						while(getmap(xx + dx[k], yy + dy[k]) == type || getmap(xx + dx[k], yy + dy[k]) == 0)
							xx += dx[k], yy += dy[k], side++;
						
						if(side + cnt < 5)
							continue;
						
						rank += 2 * cnt;
						if(getmap(x1 - dx[k], y1 - dy[k]) == 0)
							rank += 1;
						if(getmap(x2 + dx[k], y2 + dy[k]) == 0)
							rank += 1;
						if(cnt >= 5)
							rank += 2;
						
						mx = max(mx, rank);
						
						if(rank >= 10)
							p.push_back(Point(i, j));
						
						val[i][j] += 0.5 * sqr(rank / 2.0);
					}
		
		for(int i = 0; i < N; i++)
			for(int j = 0; j < N; j++)
				if(map[i][j] == 0)
					for(int k = 0; k < 8; k++)
						if(getmap(i + dx[k], j + dy[k]) == 0)
						{
							int cnt = 0, side = 0, rank = 0;
							int x1 = i + dx[k], y1 = j + dy[k];
							while(getmap(x1 + dx[k], y1 + dy[k]) == type)
								x1 += dx[k], y1 += dy[k], cnt++;
							if(cnt <= 0)
								continue;
							
							int xx, yy;
							xx = i, yy = j;
							while(getmap(xx - dx[k], yy - dy[k]) == type || getmap(xx - dx[k], yy - dy[k]) == 0)
								xx -= dx[k], yy -= dy[k], side++;
							xx = x1, yy = y1;
							while(getmap(xx + dx[k], yy + dy[k]) == type || getmap(xx + dx[k], yy + dy[k]) == 0)
								xx += dx[k], yy += dy[k], side++;
							
							if(side + cnt + 2 < 5)
								continue;
							
							rank += 2 * cnt;
							rank += 1;
							if(getmap(x1 + dx[k], y1 + dy[k]) == 0)
								rank += 1;
							
							val[i][j] += 0.25 * sqr(rank / 2.0);
						}
	}
	
	void getForce(vector<Point> &p, int type)
	{
		vector<pair<double, Point> > tmp;
		for(int i = 0; i < p.size(); i++)
			tmp.push_back(make_pair(getval(p[i].x, p[i].y), p[i]));
		sort(tmp.begin(), tmp.end(), greater<pair<double, Point> >());
		p.clear();
		for(int i = 0; i < tmp.size(); i++)
			p.push_back(tmp[i].second);
	}
	
	void calcValues()
	{
		clear();
		for(int i = 0; i < N; i++)
			for(int j = 0; j < N; j++)
				if(getmap(i, j) == 0)
					Defv[i][j] = Attv[i][j] = 0.5 - 0.1 * rand() / RAND_MAX - 0.05 * max(abs(7 - i), abs(7 - j));
		
		getAdj(forceDef, Defv, mxDRank, 2);
		getAdj(forceAtt, Attv, mxARank, 1);
		
		Dweight = 0.5;
		if(mxARank > mxDRank)
			Dweight -= 0.2;
		if(mxARank < mxDRank)
			Dweight += 0.2;
		Aweight = 1.0 - Dweight;
		
		getForce(forceDef, 2);
		getForce(forceAtt, 1);
	}
	
	Point makeDecision()
	{
		calcValues();
		
		if(mxARank >= 8 && mxDRank <= mxARank && forceAtt.size())
			return forceAtt[0];
		if(mxDRank >= 8 && forceDef.size())
			return forceDef[0];
		
		
		double best = -1;
		Point p;
		for(int i = 0; i < N; i++)
			for(int j = 0; j < N; j++)
				if(getmap(i, j) == 0 && getval(i, j) > best)
					best = getval(i, j), p = Point(i, j);
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
		Round++;
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
		Round++;
		Point put = arena.makeDecision();

#ifdef Debug
		Log = fopen("log.txt", "a+");
		
		
		fprintf(Log, "Round %d\n", Round);
		
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
/*
		fprintf(Log, "Def\n");
		for(int i = 0; i < arena.Def.size(); i++)
			fprintf(Log, "%d %d to %d %d : cnt = %d rank = %d\n", arena.Def[i].fr.x, arena.Def[i].fr.y, arena.Def[i].to.x, arena.Def[i].to.y, arena.Def[i].c, arena.Def[i].r);
		fprintf(Log, "\n");
		
		fprintf(Log, "Att\n");
		for(int i = 0; i < arena.Att.size(); i++)
			fprintf(Log, "%d %d to %d %d : cnt = %d rank = %d\n", arena.Att[i].fr.x, arena.Att[i].fr.y, arena.Att[i].to.x, arena.Att[i].to.y, arena.Att[i].c, arena.Att[i].r);
		fprintf(Log, "\n");
*/
		
		fprintf(Log, "ForceDef\n");
		for(int i = 0; i < arena.forceDef.size(); i++)
			fprintf(Log, "%d %d\n", arena.forceDef[i].x, arena.forceDef[i].y);
		fprintf(Log, "ForceAtt\n");
		for(int i = 0; i < arena.forceAtt.size(); i++)
			fprintf(Log, "%d %d\n", arena.forceAtt[i].x, arena.forceAtt[i].y);
		
		
		fprintf(Log, "mxDRank = %d\n", arena.mxDRank);
		fprintf(Log, "mxARank = %d\n", arena.mxARank);
		
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
