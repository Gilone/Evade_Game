//173号码图作业，实现鼠标操控的精灵游戏，应用类，派生，多态等知识点

#include"acllib.h"
#include"time.h"
#include<iostream>
#include<windows.h>
using namespace std;
const int mWindowWidth = 1152, mWindowHeight = 648;
const int maxNum = 20;
const int maxNum2 = 15;
const int maxNum3 = 15;
const int maxNum4 = 2;
const int maxNum5 = 4;
const int sabaruWidth = 70, sabaruHeight = 140;
const int bossWidth = 50, bossHeight = 70;
int num = 0, num2 = 0, num3 = 0, num4 = 0,num5=0;
const char *name = "d1.jpg";
const char *name2 = "d2.jpg";
const char *name3 = "d3.jpg";
const char *name4 = "d4.jpg";
const char *name5 = "d5.jpg";
int score = 0;
int sum = maxNum+maxNum2+maxNum3+maxNum4;
int life = 100;
ACL_Sound pop;
ACL_Sound background;
ACL_Image backp;
ACL_Image backp2;
clock_t stc;
void timerEvent(int id);
void keyevent(int key, int event);


class animal {                                                         //基本的动物类
protected:
	ACL_Image img;
	int x, y;
	int width, height;
	char name[50];
public:	
	animal(const char *name, int w, int h, int ax, int ay) {
		strcpy_s(this->name, name);
		loadImage(name, &img);
		width = w;
		height = h;
		x = ax;
		y = ay;
	}

		void drawImageScale()
		{
			putImageScale(&img, x, y, width, height);
		}	
};
class hpc :public animal {                                            //固定轨迹移动的电脑，派生类
private:
	int dx, dy;
public:
	hpc(const char *name, int w, int h, int x, int y,int dxx,int dyy) :animal(name, w, h, x, y) {
		dx = dxx;
		dy = dyy;
	}
	void Move(){
		x += dx;
		y += dy;
		if (x >= mWindowWidth - width || x <= 0)dx *= -1;
		if (y >= mWindowHeight - height || y <= 0)dy *= -1;
	}

	friend bool collision(int i,hpc*boss[]);
};

class pc :public animal {                                          //随机位置生成的自由移动电脑，派生类
private:
	int dx, dy;
public:
	pc(const char *name, int w, int h, int x, int y) :animal(name, w, h, x, y) {
		srand((unsigned)time(NULL));
		this->x = rand() % (mWindowWidth-bossWidth);
		this->y = rand() % (mWindowHeight-bossHeight);
		dx = rand() % 10 + 3;
		dy = rand() % 10 + 3;
	}

	void Move() {
		x += dx;
		y += dy;
		if (x >= mWindowWidth - width || x <= 0)dx *= -1;
		if (y >= mWindowHeight - height || y <= 0)dy *= -1;
	}

	friend bool collision(int i, pc* boss[]);
};


class gpc :public animal {                                            //跟踪 躲避的电脑，派生类
private:
	int dx, dy; //目标方向，用友函数赋值
	int bx, by;//步长
public:
	gpc(const char *name, int w, int h, int x, int y) :animal(name, w, h, x, y) {
		bx = 1; by = 1;
	}
	void hiding() {
	if ((dx + sabaruWidth + 40) > x&&dx + sabaruWidth<x&&dy + sabaruHeight + 40>y&&dy + sabaruHeight < y) {	bx = 5; by = 5;}  //分析八个方向 3号位
		else if (dx + sabaruWidth > x&&dx<x&&dy + sabaruHeight + 40>y&&dy + sabaruHeight < y) { bx = 0; by = 5; }//2号位
		else if (dx - 40 < x+bossWidth&&dx > x&&dy + sabaruHeight + 40 > y&&dy + sabaruHeight < y) { bx = -5; by = 5; }//1号位
		else if ((dx + sabaruWidth + 40) > x&&dx + sabaruWidth < x&&dy<y&&dy + sabaruHeight>y) { by = 5; bx = 5; }//6号位
		else if ((dx + sabaruWidth + 40) > x&&dx + sabaruWidth < x&&dy - 40 < y+bossHeight&&dy > y) { bx = 5; by = -5; }//9号位
		else if (dy - 40 < y+bossHeight&&dy > y&&dx - 40 < x+bossWidth&&dx > x) { bx = -5; by = -5; }//7 
		else if (dx - 40 < x+bossWidth&&dx > x&&dy<y&&dy + sabaruHeight>y) { bx = -5; by = 0; }//4
		else if (dx + sabaruWidth > x&&dx < x&&dy - 40 < y+bossHeight&&dy > y) { bx = 0; by = -5; }//8  
		if (x >= mWindowWidth - width || x <= 0)bx *= -1;
		if (y >= mWindowHeight - height || y <= 0)by *= -1;
		x += bx;
		y += by;
	}
	void tracking  () {
		if (dx > x&&dy > y) { bx = 1; by = 1; }
		else if (dx > x&&dy < y) { bx = 1; dy = -1; }
		else if (dx<x&&dy>y) { bx = -1; by = 1; }
		else if (dx < x&&dy < y) { bx = -1; by = -1; }
		if (x >= mWindowWidth - width || x <= 0)bx *= -1;
		if (y >= mWindowHeight - height || y <= 0)by *= -1;
		x += bx;
		y += by;
	}
	friend bool collision(int i, gpc*boss[]);
	friend void setp(int i, gpc*gboss[]);
};

class player :public animal {                      //鼠标操控的玩家，派生类
private:
	int xa, ya;
public:
	int guiji(int xa, int ya, int x, int y, int dx);
	void move();
	player(int xa, int ya, const char *name, int w, int h, int x, int y) :animal(name, w, h, x, y) {
		this->xa = xa;
		this->ya = ya;
	}

	friend bool collision(int i, hpc* boss[]);
	friend bool collision(int i, pc* boss[]);
	friend bool collision(int i, gpc* boss[]);
	friend void mouseevent(int x, int y, int button, int event);
	friend void setp(int i, gpc*gboss[]);
};


pc *boss[maxNum] = { NULL };
hpc *hboss[maxNum2] = { NULL };
hpc *wboss[maxNum3] = { NULL };
gpc *gboss[maxNum4] = { NULL };
gpc*sboss[maxNum5] = { NULL };
player *sabaru = NULL;

int Setup()
{
	srand((unsigned)time(NULL));
	initWindow("demo", 0, 0, mWindowWidth, mWindowHeight);
	sabaru=new player(30, 30, "c.jpg", sabaruWidth, sabaruHeight, bossWidth, bossHeight);
	loadSound("spop.wav", &pop);
	loadSound("backgrounds.wav", &background);
	loadImage("backp.bmp", &backp);
	loadImage("backp2.bmp", &backp2);
	registerTimerEvent(timerEvent);
	registerMouseEvent(mouseevent);
	registerKeyboardEvent(keyevent);	
	playSound(background, 1);
	beginPaint();
	clearDevice();
	putImageScale(&backp,0, 0, mWindowWidth, mWindowHeight);
	endPaint();

	return 0;
}

void keyevent(int key, int event) {
	if (event != KEY_DOWN)return;
	switch (key)
	{
	case VK_SPACE:
		beginPaint();
		clearDevice();
		endPaint();
		startTimer(0, 2000);      //每两秒生成精灵
		startTimer(2, 10);
		startTimer(1, 10);
	break;
	case VK_ESCAPE:		
		cancelTimer(0);
		cancelTimer(1);
		cancelTimer(2);
		beginPaint();
		putImageScale(&backp2, 0, 40, mWindowWidth, mWindowHeight);
		endPaint();
		break;
	default:
		break;
	}
}


void timerEvent(int id)
{
	int i; char s[20], st[20],lf[20];
	char scor[20] = {"时间"};
	char fenshu[20] = { "分数" };
	char shengming[20] = { "生命" };
		switch (id)
		{
		    case 0:
			if (num < maxNum)
			{
				boss[num] = new pc(name, bossWidth, bossHeight, 30, 30);
				if (boss[num] == NULL)
				{
					cancelTimer(0);
					return;
				}
				++num;
			}	
			if (num2 < maxNum2)
			{
				hboss[num2] = new hpc(name2, bossWidth+20, bossHeight, rand() % (mWindowWidth - bossWidth), rand() % (mWindowHeight - bossHeight),10,0);
				if (hboss[num2] == NULL)
				{
					cancelTimer(0);
					return;
				}
				++num2;
			}
			if (num3 < maxNum3)
			{
				wboss[num3] = new hpc(name3, bossWidth + 20, bossHeight, rand() % (mWindowWidth - bossWidth), rand() % (mWindowHeight - bossHeight), 0, 10);
				if (wboss[num3] == NULL)
				{
					cancelTimer(0);
					return;
				}
				++num3;
			}
			if (num4 < maxNum4)
			{
				gboss[num4] = new gpc(name4, bossWidth + 20, bossHeight, rand() % (mWindowWidth - bossWidth), rand()%(mWindowHeight-bossHeight));
				if (gboss[num4] == NULL)
				{
					cancelTimer(0);
					return;
				}
				++num4;
			}
			if (num5 < maxNum5)
			{
				sboss[num5] = new gpc(name5, bossWidth + 20, bossHeight, rand() % (mWindowWidth - bossWidth), rand() % (mWindowHeight - bossHeight));
				if (sboss[num5] == NULL)
				{
					cancelTimer(0);
					return;
				}
				++num5;
			}
			break;	   
			case 1:
			for (i = 0; i < num; ++i)
				if (boss[i]) {
					boss[i]->Move();
				}

			for (i = 0; i < num2; ++i)
				if (hboss[i]) {
					hboss[i]->Move();
				}
			for (i = 0; i < num3; ++i)
				if (wboss[i]) {
					wboss[i]->Move();
				}
			for (i = 0; i < num4; ++i)
				if (gboss[i]) {
					setp(i, gboss);
					gboss[i]->hiding();
				}
			for (i = 0; i < num5; ++i)
				if (sboss[i]) {
					setp(i, sboss);
					sboss[i]->tracking();
				}
			break;
		   case 2:
			sabaru->move();
			break;
		}
			beginPaint();
		    clearDevice();
			setPenColor(BLACK);
			setBrushColor(WHITE);
			for (i = 0; i < num; ++i)
			{
				if (!boss[i])continue;
				boss[i]->drawImageScale();
				if (collision(i,boss) == true) { 
				playSound(pop, 0);
				++score; 
				delete boss[i];
				boss[i] = NULL;	
				--sum;
				}
			}
			for (i = 0; i < num2; ++i)
			{
				if (!hboss[i])continue;
				hboss[i]->drawImageScale();
				if (collision(i, hboss) == true) {
					playSound(pop, 0);
					++score;
					delete hboss[i];
					hboss[i] = NULL;
					--sum;
				}
			}
			for (i = 0; i < num3; ++i)
			{
				if (!wboss[i])continue;
				wboss[i]->drawImageScale();
				if (collision(i, wboss) == true) {
					playSound(pop, 0);
					++score;
					delete wboss[i];
					wboss[i] = NULL;
					--sum;
				}
			}
			for (i = 0; i < num4; ++i)
			{
				if (!gboss[i])continue;
				gboss[i]->drawImageScale();
				if (collision(i, gboss) == true) {
					playSound(pop, 0);
					++score;
					delete gboss[i];
					gboss[i] = NULL;
					--sum;
				}
			}
			for (i = 0; i < num5; ++i)
			{
				if (!sboss[i])continue;
				sboss[i]->drawImageScale();
				if (collision(i, sboss) == true) {
					playSound(pop, 0);
					life=life-25;
					delete sboss[i];
					sboss[i] = NULL;
				
				}
			}
			stc = clock();
		    sabaru->drawImageScale();
			sprintf_s(s, "%d", score);
			sprintf_s(st, "%d", ((int)stc/1000));
			sprintf_s(lf, "%d", life);
			paintText(220, 20, s);
			paintText(420, 20, st);
			paintText(320, 20, scor);
			paintText(120, 20, fenshu);
			paintText(520, 20, shengming);
			paintText(620, 20, lf);
		    endPaint();
			if (sum == 0||life==0)
			{
				cancelTimer(0);
				cancelTimer(1);
				cancelTimer(2);
				beginPaint();
				putImageScale(&backp2, 0, 40, mWindowWidth , mWindowHeight);
				endPaint();
			}
	
}

void mouseevent(int kx, int ky, int button, int event) {
	if ( event == BUTTON_DOWN && button== LEFT_BUTTON){
		sabaru->xa = kx;
		sabaru->ya = ky;

		return;
	}
	else return;
}

int player::guiji(int xa, int ya, int x,int y,int dx) {  //计算鼠标轨迹的函数
	if ((ya - y) > 0) {
		if (xa - x == 0) return dx;
		else if(xa-x<0)
		return -(int)(((ya - y) / (xa - x))*dx);
		else 
			return (int)(((ya - y) / (xa - x))*dx);
	}
	else if ((ya - y) < 0) {
		if (xa - x == 0) return -dx;
		else if (xa - x<0)
			return -(int)(((ya - y) / (xa - x))*dx);
		else
			return  (int)(((ya - y) / (xa - x))*dx);
	}
	else return 0;
}


void player::move() {
	if (ya > mWindowHeight - sabaruHeight) ya= mWindowHeight - sabaruHeight;
	else if( xa >= mWindowWidth - sabaruWidth)xa = mWindowWidth - sabaruWidth;
	
		y += guiji(xa, ya, x, y, 1);
		if ((xa - x) > 0 && (xa - x) % 4 == 0) x += 4;
		else if ((xa - x < 0) && (xa - x) % 4 == 0) x += -4;
		else if (xa - x > 0)x += 1;
		else if (xa - x < 0)x -= 1;
	
}


bool collision(int i,pc* boss[])//碰撞函数，重载
{
	bool c = true;
	if (sabaru->x <boss[i]->x && sabaru->x + sabaruWidth >boss[i]->x) {
		if (sabaru->y > boss[i]->y && sabaru->y<boss[i]->y + bossHeight)return c;
		if (sabaru->y <boss[i]->y && sabaru->y + sabaruHeight >boss[i]->y)return c;
	}
	else {
		if (sabaru->x > boss[i]->x && boss[i]->x + bossWidth > sabaru->x)
		{	
			if (sabaru->y > boss[i]->y && sabaru->y<boss[i]->y + bossHeight)return c;
			if (sabaru->y <boss[i]->y && sabaru->y + sabaruHeight >boss[i]->y)return c;
		}
	}
	c = false;
	return c;
}
bool collision(int i, hpc* boss[])
{
	bool c = true;
	if (sabaru->x <boss[i]->x && sabaru->x + sabaruWidth >boss[i]->x) {
		if (sabaru->y > boss[i]->y && sabaru->y<boss[i]->y + bossHeight)return c;
		if (sabaru->y <boss[i]->y && sabaru->y + sabaruHeight >boss[i]->y)return c;
	}
	else {
		if (sabaru->x > boss[i]->x && boss[i]->x + bossWidth > sabaru->x)
		{
			if (sabaru->y > boss[i]->y && sabaru->y<boss[i]->y + bossHeight)return c;
			if (sabaru->y <boss[i]->y && sabaru->y + sabaruHeight >boss[i]->y)return c;
		}
	}
	c = false;
	return c;
}
bool collision(int i, gpc* boss[])
{
	bool c = true;
	if (sabaru->x <boss[i]->x && sabaru->x + sabaruWidth >boss[i]->x) {
		if (sabaru->y > boss[i]->y && sabaru->y<boss[i]->y + bossHeight)return c;
		if (sabaru->y <boss[i]->y && sabaru->y + sabaruHeight >boss[i]->y)return c;
	}
	else {
		if (sabaru->x > boss[i]->x && boss[i]->x + bossWidth > sabaru->x)
		{
			if (sabaru->y > boss[i]->y && sabaru->y<boss[i]->y + bossHeight)return c;
			if (sabaru->y <boss[i]->y && sabaru->y + sabaruHeight >boss[i]->y)return c;
		}	
	}
	c = false;
	return c;
}


void setp(int i,gpc*gboss[]) {
	gboss[i]->dx = sabaru->x;
	gboss[i]->dy = sabaru->y;
}