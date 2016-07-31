//#define Debug

//version = 0.2.1
//author = ChenTong

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

double cub(double x)
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

struct Change
{
	vector<Point> v;
	
	Change();
	
	Change(const Change &c);
	
	void add(const Point &p);
	
	void add(int x, int y);
	
	void pop();
};

struct State
{
	int map[N][N];
	
	State();
	
	State(const State &s);
	
	int *operator [](int x);
	
	int getmap(int x, int y) const;
	
	double value() const;
	
	double value(const Change &c) const;
};


struct Board : State
{
	vector<Point> bestPoint;
	double Defv[N][N], Attv[N][N];
	double Dweight, Aweight;
	int mxDRank, mxARank;
	
	double getval(int x, int y) const;
	
	void clear();
	
	void getAdj(double val[][N], int &mx, int type);
	
	void getBest();
	
	void calcValues();
	
	Point makeDecision();
	
};

Board arena;

namespace DecisionTree
{
	Point makeDecision(const Board &b);
	
	double Minvalue(const Board &b, State &s, Change &c, double alpha, double beta, int h);
	
	double Maxvalue(const Board &b, State &s, Change &c, double alpha, double beta, int h);
};

Change::Change()
{
	v.clear();
}

Change::Change(const Change &c)
{
	v = c.v;
}

void Change::add(const Point &p)
{
	v.push_back(p);
}

void Change::add(int x, int y)
{
	Change::add(Point(x, y));
}

void Change::pop()
{
	v.resize(v.size() - 1);
}

State::State()
{
	memset(map, 0, sizeof(map));
}

State::State(const State &s)
{
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			map[i][j] = s.map[i][j];
}


int *State::operator [](int x)
{
	return map[x];
}

int State::getmap(int x, int y) const
{
	if(!in(x, y))
		return -1;
	return map[x][y];
}


double State::value() const
{
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			if(getmap(i, j) != 0)
				for(int k = 0; k < 4; k++)
					if(getmap(i, j) != getmap(i - dx[k], j - dy[k]))
					{
						int x = i, y = j, t = getmap(i, j), cnt = 0;
						while(getmap(x, y) == t)
							cnt++, x += dx[k], y += dy[k];
						if(cnt >= 5)
							return t == 1 ? 1 : -1;
					}
	return 0;
}

double State::value(const Change &c) const
{
	
	for(int i = 0; i < c.v.size(); i++)
	{
		int x = c.v[i].x, y = c.v[i].y;
		for(int j = 0; j < 4; j++)
		{
			int x1 = x, y1 = y, x2 = x, y2 = y, cnt = 1, type = getmap(x, y);
			while(getmap(x1 - dx[j], y1 - dy[j]) == type)
				x1 -= dx[j], y1 -= dy[j], cnt++;
			while(getmap(x2 + dx[j], y2 + dy[j]) == type)
				x2 += dx[j], y2 += dy[j], cnt++;
			if(cnt >= 5)
				return type == 1 ? 1 : -1;
		}
	}
	return 0;
}


double Board::getval(int x, int y) const
{
	return Dweight * Defv[x][y] + Aweight * Attv[x][y];
}

void Board::clear()
{
	bestPoint.clear();
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			Defv[i][j] = Attv[i][j] = 0;
	Dweight = Aweight = 0;
	mxDRank = mxARank = 0;
}

void Board::getAdj(double val[][N], int &mx, int type)
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
					if(rank >= 10)
						rank += 10;
					
					mx = max(mx, rank);
					
					val[i][j] += 1.0 * sqr(rank / 3.0);
				}
	
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			if(map[i][j] == 0)
				for(int k = 0; k < 8; k++)
					if(getmap(i + dx[k], j + dy[k]) == 0)
					{
						int cnt = 1, side = 0, rank = 0;
						int x1 = i, y1 = j;
						int x2 = i + dx[k], y2 = j + dy[k];
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
						
						if(side + cnt + 1 < 5)
							continue;
						
						rank += 2 * cnt;
						if(getmap(x1 - dx[k], y1 - dy[k]) == 0)
							rank += 1;
						if(getmap(x2 + dx[k], y2 + dy[k]) == 0)
							rank += 1;
						
						val[i][j] += 0.8 * sqr(rank / 3.0);
					}
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			if(map[i][j] == 0)
				for(int k = 0; k < 8; k++)
					if(getmap(i + dx[k], j + dy[k]) == 0 && getmap(i + 2 * dx[k], j + 2 * dy[k]) == 0)
					{
						int cnt = 1, side = 0, rank = 0;
						int x1 = i, y1 = j;
						int x2 = i + 2 * dx[k], y2 = j + 2 * dy[k];
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
						
						if(side + cnt + 2 < 5)
							continue;
						
						rank += 2 * cnt;
						if(getmap(x1 - dx[k], y1 - dy[k]) == 0)
							rank += 1;
						if(getmap(x2 + dx[k], y2 + dy[k]) == 0)
							rank += 1;
						
						val[i][j] += 0.6 * sqr(rank / 3.0);
					}
}

void Board::getBest()
{
	vector<pair<double, Point> > tmp;
	for(int i = 1; i < N; i++)
		for(int j = 1; j < N; j++)
			if(getmap(i, j) == 0)
				tmp.push_back(make_pair(getval(i, j), Point(i, j)));
	sort(tmp.begin(), tmp.end(), greater<pair<double, Point> >());
	for(int i = 0; i < min(12, (int) tmp.size()); i++)
		bestPoint.push_back(tmp[i].second);
}

void Board::calcValues()
{
	clear();
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			if(getmap(i, j) == 0)
				Defv[i][j] = Attv[i][j] = 0.5 - 0.1 * rand() / RAND_MAX - 0.05 * max(abs(7 - i), abs(7 - j));
	
	getAdj(Defv, mxDRank, 2);
	getAdj(Attv, mxARank, 1);
	
	Dweight = 0.5;
	if(mxARank > mxDRank)
		Dweight -= 0.2;
	if(mxARank < mxDRank)
		Dweight += 0.2;
	Aweight = 1.0 - Dweight;
	
	getBest();
}

Point Board::makeDecision()
{
	calcValues();
	
	return DecisionTree::makeDecision(*this);
}

Point DecisionTree::makeDecision(const Board &b)
{
	double best = -1e100;
	Point ret;
	
	Change c;
	State s(b);
	for(int i = 0; i < b.bestPoint.size(); i++)
	{
		int x = b.bestPoint[i].x, y = b.bestPoint[i].y;
		s.map[x][y] = 1;
		c.add(x, y);
		double tmp = DecisionTree::Minvalue(b, s, c, best, 1e100, 4);
		s.map[x][y] = 0;
		c.pop();
		
		if(tmp > best)
			best = tmp, ret = Point(x, y);
		if(best > 0)
			return ret;
	}
	
	
	best = -1;
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			if(b.getmap(i, j) == 0 && b.getval(i, j) > best)
				best = b.getval(i, j), ret = Point(i, j);
	return ret;
}

double DecisionTree::Minvalue(const Board &b, State &s, Change &c, double alpha, double beta, int h)
{
	double cur = s.value(c);
	if(cur != 0) return cur;
	if(h <= 0) return cur;
	
	double v = 1e100;
	
	for(int i = 0; i < b.bestPoint.size(); i++)
	{
		int x = b.bestPoint[i].x, y = b.bestPoint[i].y;
		if(s.getmap(x, y) != 0) continue;
		s.map[x][y] = 2;
		c.add(x, y);
		v = min(v, DecisionTree::Maxvalue(b, s, c, alpha, beta, h - 1));
		s.map[x][y] = 0;
		c.pop();
		
		if(v <= alpha)
			return v;
		beta = min(beta, v);
	}
	return v;
}

double DecisionTree::Maxvalue(const Board &b, State &s, Change &c, double alpha, double beta, int h)
{
	double cur = s.value(c);
	if(cur != 0) return cur;
	if(h <= 0) return cur;
	
	double v = -1e100;
	State to(s);
	
	for(int i = 0; i < b.bestPoint.size(); i++)
	{
		int x = b.bestPoint[i].x, y = b.bestPoint[i].y;
		if(s.getmap(x, y) != 0) continue;
		s.map[x][y] = 1;
		c.add(x, y);
		v = max(v, DecisionTree::Minvalue(b, s, c, alpha, beta, h - 1));
		s.map[x][y] = 0;
		c.pop();
		
		if(v >= beta)
			return v;
		alpha = max(alpha, v);
	}
	return v;
}

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

#ifdef Debug
		Log = fopen("log.txt", "a+");
		fprintf(Log, "Round %d\n", Round);
		fclose(Log);
#endif
		
		Point put = arena.makeDecision();

#ifdef Debug
		Log = fopen("log.txt", "a+");
		fprintf(Log, "(%d, %d)\n", put.x, put.y);
		fprintf(Log, "Best\n");
		for(int i = 0; i < arena.bestPoint.size(); i++)
			fprintf(Log, "%d %d\n", arena.bestPoint[i].x, arena.bestPoint[i].y);
		
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

		
		fprintf(Log, "ForceDef\n");
		for(int i = 0; i < arena.forceDef.size(); i++)
			fprintf(Log, "%d %d\n", arena.forceDef[i].x, arena.forceDef[i].y);
		fprintf(Log, "ForceAtt\n");
		for(int i = 0; i < arena.forceAtt.size(); i++)
			fprintf(Log, "%d %d\n", arena.forceAtt[i].x, arena.forceAtt[i].y);
*/
		
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
