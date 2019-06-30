//6 attackers, fake defend
#include "Entry.h"
#include "Logic.h"

#include <iostream>
#include <cstdlib>
#include <algorithm>
#define sd Soldier
#define ms CONST::MapSize
using namespace std;
struct Soldier;
bool build(int x, int y, int type, bool real);
bool attack(int fx, int fy, int tx, int ty);
Soldier Map(int x, int y);
bool move(int fx, int fy, int tx, int ty);
bool base(int x, int y, int id);
int hp(int id);
struct Point
{
    Point(int x_,int y_): x(x_),y(y_){}
    int x,y;
};
Point convert(int &x,int &y);
bool inside(int x,int y) { return x>=0&&y>=0&&x<ms&&y<ms;}
int roundID=0,ID;
class SoldierSaver
{
public:
    SoldierSaver();
    enum Type
    {
        Attacker,Defender,Rival,Supporter,Size
    };
    unsigned int size(Type a) const{ return soldier[a].size(); }
    Point s(Type a,int b) const{ return soldier[a][b]; }
private:
    //void soldierTypeArrange();
    int a[ms][ms];
    vector<Point> soldier[Size];
} *ssr;

SoldierSaver::SoldierSaver()
{
	for (int i=ms-1;i>=0;i--)
        for (int j=0;j<ms;j++)
        {
            sd s=Map(i,j);
            if(s.ownerID==ID)
            {
                if (s.fake)
                {
                	if (( !base(i,j,ID) || ((j==2||j==22)&&roundID==4) )
                		 &&i==2)
                	{
                		soldier[Supporter].push_back(Point(i,j));
                		printf("Supporter %d %d\n",i,j);
                	}
                	else
                	{
                    	soldier[Defender].push_back(Point(i,j));
                		printf("Defender %d %d\n",i,j);
                	}
                }
                else
                {
                	if (roundID>5&&base(i,j,ID))
                		soldier[Defender].push_back(Point(i,j));
                	else
                    	soldier[Attacker].push_back(Point(i,j));
                    printf("Attacker %d %d\n",i,j);
                }
            }else if (s.ownerID==(ID^1)) {
                a[i][j]=-1;
                soldier[Rival].push_back(Point(i,j));
            }
        }
    printf("LoadOk\n");
}

namespace mapSaver
{
	bool last[ms][ms];
	bool now[ms][ms];
	bool beenAttacked=0;
	void save(bool s[ms][ms])
	{
		memset(s,0,sizeof(now));
		for (int i=0;i<ms;i++)
			for (int j=0;j<ms;j++)
				if (Map(i,j).ownerID==ID)
					s[i][j]=1;
	}
	bool attacked()
	{
		if (roundID==1) return 0;
		if (beenAttacked) return 1;
		for (int i=0;i<ms;i++)
			for (int j=0;j<ms;j++)
				if (now[i][j]!=last[i][j])
					beenAttacked=1;
		return beenAttacked;
	}
}

namespace AttackArranger
{
    void buildSoldier()
    {
        build(2,1,rand()%3+1,1);
        build(2,12,rand()%3+1,1);
        build(2,23,rand()%3+1,1);
    }
    int dist[ms][ms];
    int danger[ms][ms];
    int lx[5]={-1,1,0,0,0};
    int ly[5]={0,0,-1,1,0};
    inline pair<int,int> value(int x,int y) { return make_pair(-dist[x][y],-danger[x][y]); }

    int attackHP(const sd &f,const sd &g)
    {
        if (f.fake) return 0;
        if (g.fake) return 10000;
        double rate=1;
        switch (f.kind)
        {
        case 1:
            if (g.kind==2||g.kind==4) rate=2;
            if (g.kind==3) rate=0.5;
            break;
        case 2:
            if (g.kind==3||g.kind==4) rate=2;
            if (g.kind==1) rate=0.5;
            break;
        case 3:
            if (g.kind==1||g.kind==4) rate=2;
            if (g.kind==2) rate=0.5;
            break;
        case 4:
            if (g.kind==1||g.kind==2||g.kind==3) rate=0.4;
            break;
        }
        return f.atn*f.HP*rate/f.maxHP;
    }

    bool winInOneAttack(int fx,int fy,int gx,int gy)
    {
    	if (!inside(fx,fy)) return 0;
        if (!inside(gx,gy)) return 1;
        sd f=Map(fx,fy),g=Map(gx,gy);
        if (g.ownerID==ID) return 0;
        if (g.ownerID==-1) return 1;
        if (attackHP(f,g)>=g.HP) return 1;
        return 0;
    }
/*
    bool gotoBase(int x,int y,int &dx,int &dy)
    {
        if (base(x,y,ID^1)) return 0;
        if (base(x+1,y,ID^1)) { dx=1; dy=0; return 1;}
        if (base(x,y+1,ID^1)) { dx=0; dy=1; return 1;}
        if (base(x,y-1,ID^1)) { dx=0; dy=-1; return 1;}
        if (x==20&&base(x,y,))
        base(x,y,x+1,y)
        return ((x==21&&(winInOneAttack(x,y,x+1,y)||getdist(x,y)>100))
              ||(x==20&&Map(x+1,y).ownerID==(ID^1)&&Map(x+1,y).fake));
    }
*/
    bool better(Point x,Point y)
    {
    	if (!inside(x.x,x.y)||!inside(y.x,y.y)) return inside(x.x,x.y)>inside(y.x,y.x);
        if (x.x<=20||y.x<=20) return x.x>y.x||
                                     x.x==y.x&&dist[x.x][x.y]<dist[y.x][y.y]||
                                     x.x==y.x&&dist[x.x][x.y]==dist[y.x][y.y]&&danger[x.x][x.y]<danger[y.x][y.y];
        else return dist[x.x][y.x]<dist[y.x][y.y]||
                    dist[x.x][y.x]==dist[y.x][y.y]&&danger[x.x][x.y]<danger[y.x][y.y];
    }

	void valueInitialize()
    {
        memset(danger,0,sizeof(danger));
        for (int i=0; i<ms; i++)
            for (int j=0; j<ms; j++)
            {
                int id=Map(i,j).ownerID;
                if (id!=-1) dist[i][j]=1000;
                    else dist[i][j]=0;
                if (id==(ID^1))
                {
                    for (int dx=-3;dx<=3;dx++)
                        for (int dy=-(3-abs(dx));dy<=(3-abs(dx));dy++)
                            if (inside(i+dx,j+dy)) danger[i+dx][j+dy]++;
                }
            }
        for (int i=0;i<ms;i++)
            for (int j=0;j<ms;j++)
                if (dist[i][j]!=-1&&base(i,j,ID^1))
                    dist[i][j]=1;
        for (int round=1;;round++)
        {
            int count=0;
            for (int i=0;i<ms;i++)
                for (int j=0;j<ms;j++)
                    if (dist[i][j]==round)
                    {
                        for (int k=0;k<4;k++)
                        {
                            int x=i+lx[k],y=j+ly[k];
                            if (inside(x,y)&&dist[x][y]==0)
                            {
                                dist[x][y]=round+1;
                                count++;
                            }
                        }
                    }
            if (count==0) break;
        }
        for (int i=0;i<ms;i++)
            for (int j=0;j<ms;j++)
                if (dist[i][j]==0) dist[i][j]=1000;
    }

    bool baseBetter(Point a, Point b)
    {
    	bool b1=base(a.x,a.y,ID^1),b2=base(b.x,b.y,ID^1);
    	if (b1!=b2) return b1>b2; else return a.x>b.x;
    }

    bool gotoBase(const Point &p,int* d)
    {
    	int e[3];
    	Point best=Point(0,0);
    	for (int i=1;i<=4;i++)
    		for (int j=1;j<=4;j++)
    			for (int k=1;k<=4;k++)
    			{
    				e[0]=i; e[1]=j; e[2]=k;
    				int x=p.x,y=p.y;
    				for (int w=0;w<3;w++)
    				{
    					x=x+lx[e[w]]; y=y+ly[e[w]];
    					if (!inside(x,y)) {x=-1; y=-1; break;}
    					if (Map(x,y).ownerID==-1) continue;
    					if (Map(x,y).ownerID==(ID^1)&&(winInOneAttack(p.x,p.y,x,y))) { e[w]=-e[w]; break;}
    					x=-1; y=-1; break;
    				}
    				if (baseBetter(Point(x,y),best))
    				{
    					memcpy(d,e,sizeof(e));
    					best=Point(x,y);
    				}
    			}	
    	if (base(best.x,best.y,ID^1)) return 1;
    }

    bool meetFake(const Point &p, int *d)
    {
    	int e[3];
    	Point best=Point(0,0);
    	for (int i=1;i<=4;i++)
    		for (int j=1;j<=4;j++)
    			for (int k=1;k<=4;k++)
    			{
    				e[0]=i; e[1]=j; e[2]=k;
    				int x=p.x,y=p.y;
    				for (int w=0;w<3;w++)
    				{
    					x=x+lx[e[w]]; y=y+ly[e[w]];
    					if (!inside(x,y)) { x=-1; y=-1; break;}
    					if (Map(x,y).ownerID==-1) continue;
    					if (Map(x,y).ownerID==(ID^1)) { e[w]=-e[w]; break;}
    					x=-1; y=-1; break;
    				}
    				if (Map(x,y).ownerID==(ID^1))
    				{
    					memcpy(d,e,sizeof(e));
    					return 1;
    				}
    			}
    }

    void go(const Point &p,int *d)
    {
    	int x=p.x,y=p.y;
    	for (int i=0;i<3;i++)
    	{
    		if (d[i]<0)
    		{
    			int tx=x+lx[-d[i]],ty=y+ly[-d[i]];
    			attack(x,y,tx,ty); break;
    		}
    		else
    		{
    			int tx=x+lx[d[i]],ty=y+ly[d[i]];
    			move(x,y,tx,ty); x=tx; y=ty;
    		}
    	}
    }

    void moveForward(const Point &p)
    {
    	int d[3];
    	if (gotoBase(p,d))
    		go(p,d);
    	else if (roundID==4&&ID==1&&meetFake(p,d))
    		go(p,d);
    	else
    	{
    		int x=p.x,y=p.y,energy=3;
    		while (energy)
    		{
    			valueInitialize();
	        	Point p[3]={Point(x+1,y),Point(x,y+1),Point(x,y-1)};
            	if (better(p[2],p[1])) swap(p[2],p[1]);
           		if (better(p[1],p[0])) swap(p[1],p[0]);
            	if (better(p[2],p[1])) swap(p[2],p[1]);
            	if (x<21)
            	{
                	if (move(x,y,x+1,y)) { x++; energy--; continue; }
                	if (x==20&&winInOneAttack(x,y,x+1,y)) { if (attack(x,y,x+1,y)) { energy=0; continue;}  }
               		int d=better(Point(x,y+1),Point(x,y-1))?+1:-1;
                	if (move(x,y,x,y+d)) { y+=d; energy--; continue; }
                	if (move(x,y,x,y-d)) { y-=d; energy--; continue; }
            	}
            	else if (x==21)
            	{
            		if (move(x,y,x+1,y)) { x++; energy--; continue; }
                	if (Map(x+1,y).ownerID==(ID^1)&&!winInOneAttack(x+1,y,x,y)) { if (attack(x,y,x+1,y)) { energy=0; continue;}}
                	bool moved=0;
                    for (int i=0;i<3;i++)
                        if (move(x,y,p[i].x,p[i].y)) { x=p[i].x; y=p[i].y; energy--; moved=1; break;}
                    if (moved) continue;
            	}
            	else
            	{
            		bool moved=0;
	                for (int i=0;i<3;i++)
	                    if (move(x,y,p[i].x,p[i].y)) { x=p[i].x; y=p[i].y; energy--; break;}
	                if (moved) continue;
	            }
	            break;
            }
            
        }
        
    }

    void moveForward()
    {
        for (int i=0; i<ssr->size(ssr->Attacker); i++)
        {
            int x=ssr->s(ssr->Attacker,i).x,y=ssr->s(ssr->Attacker,i).y;
            moveForward(Point(x,y));
        }
    }

    void solveAttack()
    {
        moveForward();
        if (roundID<=2)
            buildSoldier();
        //ssr->update();
    }
}

namespace DefendArranger
{
    static const int top=5,below=5,middle=9;
    const int topy[top]={22,24,23,22,22};
    const int topx[top]={2,2,2,1,0};
    const int belowy[below]={2,0,1,2,2};
    const int belowx[top]={2,2,2,1,0};
    const int midy[middle]={12,14,10,13,11,14,10,14,10};
    const int midx[middle]={2,2,2,2,2,1,1,0,0};

    int moveX(int x,int y)
    {
        if (x==2&&(y<2||(y>10&&y<14)||(y>23)))
            return x+1;
        else return x;
    }

    bool exist(int x,int y)
    {
        return Map(x,y).ownerID==ID||Map(moveX(x,y),y).ownerID==ID;
    }

    int getBestType(int x)
    {
    	if (x==1) return 3;
    	if (x==2) return 1;
    	if (x==3) return 2;
    	if (x==4) return rand()%3+1;
    }

    void buildDefender()
    {
        for (int i=0;i<top;i++)
        {
            if (!exist(topx[i],topy[i]))
            {
            	if (Map(topx[i]+1,topy[i]).ownerID==(ID^1)&&!base(topx[i]+1,topy[i],ID)&&mapSaver::attacked())
            		build(topx[i],topy[i],getBestType(Map(topx[i]+1,topy[i]).kind),1);
            	build(topx[i],topy[i],rand()%3+1,0);
            }
        }
        for (int i=0;i<below;i++)
             if (!exist(belowx[i],belowy[i]))
            {
            	if (Map(belowx[i]+1,belowy[i]).ownerID==(ID^1)&&!base(belowx[i]+1,belowy[i],ID)&&mapSaver::attacked())
            		build(belowx[i],belowy[i],getBestType(Map(belowx[i]+1,belowy[i]).kind),1);
            	build(belowx[i],belowy[i],rand()%3+1,0);
            }
        for (int i=0;i<middle;i++)
        {
        	if (roundID==4||roundID==5)
        	{
        		if (i==1||i==2) continue;
        	}
            if (!exist(midx[i],midy[i]))
            {
            	if (Map(midx[i]+1,midy[i]).ownerID==(ID^1)&&!base(midx[i]+1,midy[i],ID)&&mapSaver::attacked())
            		build(midx[i],midy[i],getBestType(Map(midx[i]+1,midy[i]).kind),1);
            	build(midx[i],midy[i],rand()%3+1,0);
            }
        }
    }

    void adjust()
    {
        for (int i=0;i<ssr->size(ssr->Defender);i++)
        {
            int x=ssr->s(ssr->Defender,i).x,y=ssr->s(ssr->Defender,i).y;
            if (mapSaver::attacked())
            {
            	if (base(x,y,ID)&&!base(x+1,y,ID))
            	{
            		if (Map(x+1,y).ownerID==(ID^1)||Map(x+2,y).ownerID==(ID^1)||Map(x+3,y).ownerID==(ID^1))
            		{
            			move(x,y,x+1,y);
            			continue;
            		}
            	}
            }
            move(x,y,moveX(x,y),y);
        }
    }

    void solveDefend()
    {
        if (roundID>=3)
            buildDefender();
        adjust();
    }
}

namespace SupportArranger
{
	void solveSupport()
	{
		for (int i=0;i<ssr->size(ssr->Supporter);i++)
		{
			int x=ssr->s(ssr->Supporter,i).x,y=ssr->s(ssr->Supporter,i).y;
			printf("solveSupport %d\n",y);
			if (y<12)
			{
				for (int j=0;j<3;j++)
				{
					move(x,y,x,y+1);
					y++;
					if (base(x,y,ID)) break;
				}

			}
			else
			{
				for (int j=0;j<3;j++)
				{
					move(x,y,x,y-1);
					y--;
					if (base(x,y,ID)) break;
				}
			}
		}
	}
}

void playerAI()
{
    ID=Logic::Instance()->GetId();
    roundID++;
    mapSaver::save(mapSaver::now);
    ssr = new SoldierSaver;
    AttackArranger::solveAttack();
    SupportArranger::solveSupport();
    DefendArranger::solveDefend();
    mapSaver::save(mapSaver::last);
    delete ssr;
}



Point convert(int &x,int &y)
{
    if (Logic::Instance()->GetId() == 1)
    {
        x = 24 - x;
        y = 24 - y;
    }
    return Point(x,y);
}

/* ----- 以下操作当且仅当成功时返回 true ----- */

bool build(int x, int y, int type, bool real)
{
    convert(x,y);
	return Logic::Instance()->Build(x, y, type, real);
}
// 在 (fx, fy) 建立一个你的单位，type表示类型，克制关系为 1 > 2 > 3 > 1，此外123相同。4为弓箭手。real为假表示幻象。一个回合只能建立一次。只能建立在基地中。

bool attack(int fx, int fy, int tx, int ty)
{
    convert(fx,fy);
	convert(tx,ty);
	return Logic::Instance()->Attack(fx, fy, tx, ty);
}
// 让 (fx, fy) 点的有行动力的你的单位攻击 (tx, ty) 的地方单位，消耗所有行动力

bool move(int fx, int fy, int tx, int ty)
{
    //printf("move %d %d %d %d\n",fx,fy,tx,ty);
	convert(fx,fy);
	convert(tx,ty);
	return Logic::Instance()->Move(fx, fy, tx, ty);
}
// 将 (fx, fy) 点的有剩余行动力的你的单位移动到空地 (tx, ty) 点，消耗1行动力

bool hidechange(int x, int y)
{
	convert(x,y);
	return Logic::Instance()->Hidechange(x, y);
}
// 更改位于 (fx, fy) 单位的潜行状态 （潜行变现形、现形变潜行）

/* ----- 以上操作当且仅当成功时返回 true ----- */


int hp(int id)
{
	return Logic::Instance()->GetHP(id);
}
// 返回编号为 id 的玩家剩余的生命值（初始为5）
// 不合法则返回 -1

int money()
{
	return Logic::Instance()->GetMoney();
}
// 返回你当前的剩余金钱

Soldier Map(int x, int y)
{
    convert(x,y);
	return Logic::Instance()->GetMap(x, y);
}
// 返回地图上一个位置的单位信息

state mine(int x, int y)
{
    convert(x,y);
	return Logic::Instance()->GetMine(x, y);
}
// 返回地图上一个点的矿物信息，

bool base(int x, int y, int id)
{
    convert(x,y);
	return Logic::Instance()->InBase(x, y, id);
}
// 返回一个点是否是 id 号玩家的基地


/*Point p[3]={Point(x+1,y),Point(x,y+1),Point(x,y-1));
            if (better(p[2],p[1])) swap(p[2],p[1]);
            if (better(p[1],p[0])) swap(p[1],p[0]);
            if (better(p[2],p[1])) swap(p[2],p[1]);
            if (x<21)
            {
                for (int i=0;i<3;i++)
                    if (move(x,y,p[i].x,p[i].y)) { x=p[i].x; y=p[i].y; energy--; break;}
            }
            else if (x==21)
            {
                if (energy<=2&&Map(x+1,y).ownerID==(ID^1)&&Map(x+1,y).fake) attack(x,y,x+1,y);
                else
                {
                    for (int i=0;i<3;i++)
                        if (move(x,y,p[i].x,p[i].y)) { x=p[i].x; y=p[i].y; energy--; break;}
                }
            }
            else
            {
                for (int i=0;i<3;i++)
                    if (getdist(>dist[p[i].x][p[i].y]move(x,y,p[i].x,p[i].y)) { x=p[i].x; y=p[i].y; energy--; break;}
            }*/