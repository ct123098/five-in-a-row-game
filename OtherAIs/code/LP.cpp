#include<bits/stdc++.h>

using namespace std;

const int dr[]={-1, 1, 0, 0,-1, 1,-1, 1};
const int dc[]={ 0, 0,-1, 1,-1, 1, 1,-1};
const int pos[15][15]={
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
{0,1,2,2,2,2,2,2,2,2,2,2,2,1,0},
{0,1,2,3,3,3,3,3,3,3,3,3,2,1,0},
{0,1,2,3,4,4,4,4,4,4,4,3,2,1,0},
{0,1,2,3,4,5,5,5,5,5,4,3,2,1,0},
{0,1,2,3,4,5,6,6,6,5,4,3,2,1,0},
{0,1,2,3,4,5,6,7,6,5,4,3,2,1,0},
{0,1,2,3,4,5,6,6,6,5,4,3,2,1,0},
{0,1,2,3,4,5,5,5,5,5,4,3,2,1,0},
{0,1,2,3,4,4,4,4,4,4,4,3,2,1,0},
{0,1,2,3,3,3,3,3,3,3,3,3,2,1,0},
{0,1,2,2,2,2,2,2,2,2,2,2,2,1,0},
{0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
const int val[]={0,0, 0,0, 5,3, 200,50, 10000,500};//基本棋型对应的分值 

int board[20][20];
//-2：不搜该位置；-1：搜索该位置；0：己方棋子；1：对方棋子

bool InBoard(int r,int c)
{
	if(r>=0&&r<15&&c>=0&&c<15)	return true;
	return false;
}

int col(int r,int c)
{
	if(!InBoard(r,c))	return -1;
	return max(board[r][c],-1);
}

int Score(int r,int c)
{
	int ret=pos[r][c]*2;
	for(int h=0;h<2;h++)
	{
		board[r][c]=h;
		int l[4]={1,1,1,1},ob[4]={0};
		for(int i=0;i<8;i++)
		{
			int j;
			for(j=1;col(r+dr[i]*j,c+dc[i]*j)==h;j++)	l[i>>1]++;
			if(col(r+dr[i]*j,c+dc[i]*j)==(h^1))	ob[i>>1]++;
		}
		int mx=0;
		for(int i=0;i<4;i++)
		{
			if(l[i]>=5)	return 1e9;
			if(ob[i]<2)	mx=max(mx,val[(l[i]<<1)+ob[i]]);
		}
		ret+=mx;
		for(int i=0;i<4;i++)	
		board[r][c]=-1;
	}
	return ret;
}

void Put(int r,int c,int h)
{
	board[r][c]=h;
	for(int i=0;i<8;i++)
		for(int j=1;j<=2;j++)
			if(InBoard(r+dr[i]*j,c+dc[i]*j))
				board[r+dr[i]*j][c+dc[i]*j]=max(board[r+dr[i]*j][c+dc[i]*j],-1);
}

pair<int,int> Search()
{
	pair<int,int> ret;
	int mx=0;
	for(int i=0;i<15;i++)
		for(int j=0;j<15;j++)
			if(board[i][j]==-1)
			{
				int tmp=Score(i,j);
				if(tmp>mx)	{mx=tmp;ret.first=i;ret.second=j;}
			}
	return ret;
}

void MyGo()
{
	pair<int,int> step=Search();
	cout<<step.first<<" "<<step.second<<endl;
	Put(step.first,step.second,0);
}

void YouGo()
{
	int r,c;
	cin>>r>>c;
	Put(r,c,1);
}

void Init()
{
	for(int i=0;i<15;i++)
		for(int j=0;j<15;j++)
			board[i][j]=-2;
	board[7][7]=-1;
}

void Print()
{
	for(int i=0;i<15;i++,cout<<endl)
		for(int j=0;j<15;j++)
			printf("%2d ",board[i][j]);
}

int main()
{
	Init();
	int first;
	cin>>first;
	if(first)	MyGo();
	while(true)
	{
		YouGo();
		MyGo();
	}
	return 0;
}
