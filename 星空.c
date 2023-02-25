#include<acllib.h>
#include<math.h>
#include<stdio.h>
#include<time.h>
#define maxN 4

void timeEvent_auto(int id);
void automove();
void addnumber();
void criterion_fairy();
void criterion_attack();
void criterion_attackc();
void paint_basic();
void paint_fairy();
void paint_attack();
void keyevent_control(int key,int event);
void mouse(int x,int y,int button,int event);
void gameover();

struct fairy_auto
{
	int x;
	int y;
	int fairyw;
	int fairyh;
	int distx;
	int disty;
}fairy[maxN],boss;

struct fairy_control
{
	int x;
	int y;
	int w;
	int h;
	int dist;
}warrior,attack,attackc;

struct rect//碰撞结构 
{
	int x;
	int y;
	int w;
	int h;
}r1,r2;
int collision_fairy(struct rect r1,struct rect r2);
void collision_attack (struct rect r1,struct rect r2);
void collision_attackc(struct rect r1,struct rect r2);

ACL_Image img_fairy;
ACL_Image img_boss;
ACL_Image img_warrior;
ACL_Image img_warriorleft;
ACL_Image img_back;
ACL_Image img_attack;
ACL_Image img_attackc;
ACL_Image menu;

ACL_Sound bgm;
ACL_Sound click;
ACL_Sound peng;
ACL_Sound sattack;
ACL_Sound beaten;
ACL_Sound victory;
ACL_Sound defeat;

const int width=1500,height=1000;
int i,start=0,id1=1,id2=2,id3=3,id4=4,id5=5,judge=1,release=0,releaseright=1,releaseleft=1,scorelife=0;
static int score=0,timer=0,life=3,num=0,convert=0,convert_attack=0,convertc=1,blood=1150,direct=1;

int Setup()
{
    initWindow("游戏", DEFAULT, DEFAULT,width,height);
    const char *i_fairy="fairy.gif";	 loadImage(i_fairy,&img_fairy);
	const char *i_boss="BOSS.gif";       loadImage(i_boss,&img_boss);
	const char *i_warrior="warrior.gif"; loadImage(i_warrior,&img_warrior);
	const char *i_left="warriorleft.gif";loadImage(i_left,&img_warriorleft);
	const char *i_back="background.jpg"; loadImage(i_back,&img_back);
	const char *i_attack="attack.gif";   loadImage(i_attack,&img_attack);
	const char *i_attackc="attackc.gif"; loadImage(i_attackc,&img_attackc);
	const char *i_menu="menu.jpg";      loadImage(i_menu,&menu);

	const char *s_bgm="bgm.mp3"; loadSound(s_bgm,&bgm);
	const char *s_click="click.mp3"; loadSound(s_click,&click);
	const char *s_peng="peng.mp3"; loadSound(s_peng,&peng);
	const char *s_sattack="attack.mp3";loadSound(s_sattack,&sattack);
	const char *s_beaten="beaten.mp3"; loadSound(s_beaten,&beaten);
	const char *s_victory="vict.mp3"; loadSound(s_victory,&victory);
	const char *s_defeat="defeat.mp3"; loadSound(s_defeat,&defeat);
	
	srand((unsigned)time(NULL));
	attack.h=150;    attack.w=150;    attack.dist=9;   attack.y=-attack.h;
	attackc.h=150;   attackc.w=150;   attackc.dist=9;  attackc.y=-attackc.h;
	boss.fairyh=300; boss.fairyw=400; boss.distx=3;    boss.disty=3;    boss.x=-width;
	warrior.h=300;   warrior.w=300;   warrior.dist=20; warrior.x=100; warrior.y=100;
	
	beginPaint();
	putImageScale(&menu,0,0,width,height);
	endPaint();
	playSound(bgm,1);
	registerKeyboardEvent(keyevent_control);
	registerTimerEvent(timeEvent_auto);
	registerMouseEvent(mouse);
	startTimer(id1,20);
    startTimer(id2,1000);
	startTimer(id3,5000);
	startTimer(id4,1000);
	return 0;
}

void paint_basic()
{
	char basic[44];
	if(score<=0) score=0;
	sprintf(basic,"分数:%d 生命值:%d 时间:%d",score*25,life,timer);
	setTextColor(CYAN);
	setTextBkColor(EMPTY);
	setTextSize(50);
	putImageScale(&img_back,0,0,width,height);
	paintText(15,15,basic);
	setPenColor(CYAN);
	setBrushColor(RED);
	paintText(900,15,"BOSS血量：");
	rectangle(1150,15,1480,65);
    setBrushColor(BLACK);
	rectangle(1150,15,blood,65);
}
void paint_fairy()
{
	beginPaint();
	clearDevice();
	paint_basic();
	if(convert==1)
	{
		putImageTransparent(&img_boss,boss.x,boss.y,boss.fairyw,boss.fairyh,WHITE);
		putImageTransparent(&img_attack,attack.x,attack.y,attack.w,attack.h,WHITE);
	}
	if(direct== 1)  putImageTransparent(&img_warrior,warrior.x,warrior.y,warrior.w,warrior.h,WHITE);
	if(direct==-1)  putImageTransparent(&img_warriorleft,warrior.x,warrior.y,warrior.w,warrior.h,WHITE);
	putImageTransparent(&img_attackc,attackc.x,attackc.y,attackc.w,attackc.h,WHITE);
	for(i=0;i<num;i++)
	putImageTransparent(&img_fairy,fairy[i].x,fairy[i].y,fairy[i].fairyw,fairy[i].fairyh,WHITE);
	if(life<=0)
	{
		setTextSize(200);
		paintText(width/2-height/2+75,height/2-200,"GAME OVER!");
		paintText(width/2-height/2+75,height/2-200+200,"按TAB重来");
		playSound(defeat,0);
	}
	if(blood>=1480||score>=5000)
	{
		setTextSize(200);
		paintText(width/2-height/2+75,height/2-200,"VICTORY!");
		paintText(width/2-height/2+75,height/2-200+200,"按TAB重来");
		playSound(victory,0);
	}
	endPaint();
}
void timeEvent_auto(int id)
{
	if(start)
	{
		if(id==id1)
	    {
		    automove();
		    criterion_fairy();
		    gameover();
	    }
	    if(id==id2)
	    {
		    timer++;
		    addnumber();  
	    }
	    if(id==id3)
	    {
	    	convert=1;
	    	boss.x=rand()%(width -boss.fairyw);
	    	boss.y=rand()%(height-boss.fairyh);
	    	cancelTimer(id3);
	    }
	    if(id==id4)
	    {
	    	if(timer>=5)
			{
				criterion_attackc();
			}
	    }
	}
}

void automove() //精灵移动 
{
	for(i=0;i<num;i++)
	{
		fairy[i].x+=fairy[i].distx;
	    fairy[i].y+=fairy[i].disty;
	    if(fairy[i].x>width -fairy[i].fairyw||fairy[i].x<0) fairy[i].distx*=-1;
	    if(fairy[i].y>height-fairy[i].fairyh||fairy[i].y<0) fairy[i].disty*=-1;
	}
	if(convert==1)
	{
		boss.x+=boss.distx;
	    boss.y+=boss.disty;
	    if(boss.x>width -boss.fairyw||boss.x<0) boss.distx*=-1;
	    if(boss.y>height-boss.fairyh||boss.y<0) boss.disty*=-1;
		if(timer%5==0)
		{
			if(blood>1320)
			{
				if(boss.distx>0) boss.distx=4;
				else boss.distx=-4;
				if(boss.disty>0) boss.disty=4;
				else boss.disty=-4;
				if(blood>1400)
				{
					if(boss.distx>0) boss.distx=5;
				    else boss.distx=-5;
					if(boss.disty>0) boss.disty=5;
				    else boss.disty=-5;
				    for(i=0;i<num;i++)
				    {
				    	if(fairy[i].distx>0) fairy[i].distx=4;
				    	else fairy[i].distx=-4;
				    	if(fairy[i].disty>0) fairy[i].disty=4;
				    	else fairy[i].disty=-4;
					}
				}
			}
			attack.x=boss.x-attack.w+100; attack.y=boss.y+100;
		}
		attack.x-=attack.dist;
		if(attack.x<-attack.w) attack.x=-attack.w;
	}
	if(releaseright==1&&judge==0)
	{
		attackc.x=warrior.x+warrior.w-60;
		attackc.y=warrior.y+100;
	}
	else if(releaseleft==1&&judge==0)
	{
		attackc.x=warrior.x-attackc.w+40;
		attackc.y=warrior.y+100;
	}
	judge=1;
	if(release==1&&releaseright==1)attackc.x+=attackc.dist;
	else if(release==1&&releaseleft==1)attackc.x-=attackc.dist;
	if(attackc.x>attackc.w+width||attackc.x<-attackc.w)
	{
		attackc.x=0;
		attackc.y=-height;
		release=0;
	}
	paint_fairy();
}
void addnumber() //精灵数增加
{
	if(num<maxN)
	{
		fairy[num].fairyh=75;  fairy[num].fairyw=100;   fairy[num].distx=2;   fairy[num].disty=2;
		fairy[num].x=rand()%(width-fairy[num].fairyw);  fairy[num].y=rand()%(height-fairy[num].fairyh);
		num++;
	}
}
void criterion_fairy() //精灵碰撞判定
{
	int fact=0;
	r1.x=warrior.x; r1.y=warrior.y; r1.w=warrior.w; r1.h=warrior.h;
	for(i=0;i<num;i++)
	{
		r2.x=fairy[i].x; r2.y=fairy[i].y; r2.w=fairy[i].fairyw; r2.h=fairy[i].fairyh;
		fact=collision_fairy(r1,r2);
		if(fact==1)
		{
		    score--;
			scorelife++;
			fairy[i].x=rand()%(width-fairy[i].fairyw);
		    fairy[i].y=rand()%(height-fairy[i].fairyh);
		    if(scorelife%4==0)  life--; 
		    playSound(beaten,0);
		}
	}
	r1.x=attackc.x; r1.y=attackc.y; r1.w=attackc.w;   r1.h=attackc.h;
	for(i=0;i<num;i++)
	{
		r2.x=fairy[i].x; r2.y=fairy[i].y; r2.w=fairy[i].fairyw; r2.h=fairy[i].fairyh;
		fact=collision_fairy(r1,r2);
		if(fact==1)
		{
		    fairy[i].x=rand()%(width-fairy[num].fairyw);  
			fairy[i].y=rand()%(height-fairy[num].fairyh);
			attackc.x=0; attackc.y=-height;
			release=0;
			playSound(peng,0);
		}
	}
	if(timer>5)
	{
	r2.x=boss.x;    r2.y=boss.y;    r2.w=boss.fairyw; r2.h=boss.fairyh;
	fact=collision_fairy(r1,r2);
	if(fact==1){score+=4;  blood+=10; attackc.x=0; attackc.y=-height; release=0; playSound(peng,0);}
	r2.x=attack.x;  r2.y=attack.y;  r2.w=attack.w;    r2.h=attack.h;
	fact=collision_fairy(r1,r2);
	if(fact==1){score+=4;  attackc.x=0; attackc.y=-height; attack.x=-attack.w; release=0; playSound(peng,0);}
	}
}
void criterion_attackc() //我方攻击碰撞判定 
{
	int fact=0;
	r1.x=warrior.x; r1.y=warrior.y; r1.w=warrior.w;   r1.h=warrior.h;
	r2.x=boss.x;    r2.y=boss.y;    r2.w=boss.fairyw; r2.h=boss.fairyh;
	fact=collision_fairy(r1,r2);
	if(fact==1){ life--; if(blood>=1170) blood-=20; playSound(beaten,0);}
	r2.x=attack.x;  r2.y=attack.y;  r2.w=attack.w;    r2.h=attack.h;
	fact=collision_fairy(r1,r2);
	if(fact==1){ life--; if(blood>=1170) blood-=20; attack.x=-attack.w; playSound(beaten,0);}
}
int collision_fairy(struct rect r1,struct rect r2)//碰撞检测函数 
{
	int fact=1;
	if(r1.x<r2.x&&r1.x+r1.w>r2.x)
	{
		if(r1.y>r2.y&&r1.y<r2.y+r2.h)return fact;
		if(r1.y<r2.y&&r1.y+r1.h>r2.y)return fact;
	}
	else
	{
	 if(r1.x>r2.x&&r2.x+r2.w>r1.x)
	  {
		if(r1.y>r2.y&&r1.y<r2.y+r2.h)return fact;
		if(r1.y<r2.y&&r1.y+r1.h>r2.y)return fact;
	  }
	}
	fact=0;
	return fact;
}
void keyevent_control(int key,int event)
{
	if(event!=KEY_DOWN) return;
	switch(key)
	{
		case VK_RETURN: start=1; playSound(click,0);break;
		case VK_UP:
			if(start==1)
			{
			warrior.y-=warrior.dist;
		    if(warrior.y<=0) warrior.y=0; 
			}
		    break;
		case VK_DOWN:
			if(start==1)
			{
			warrior.y+=warrior.dist;
		    if(warrior.y>=height-warrior.h) warrior.y=height-warrior.h;		
			}
		    break;
		case VK_LEFT:
			if(start==1)
			{
			direct=-1;
		    warrior.x-=warrior.dist;
		    if(warrior.x<=0) warrior.x=0;
			}
		    break;
		case VK_RIGHT:
			if(start==1)
			{
			direct=1;
		    warrior.x+=warrior.dist;
		    if(warrior.x>=width -warrior.w) warrior.x=width -warrior.w;
			}
		    break;
		case VK_SPACE:
			if(start==1)
			if(release==0)
			{
				release=1;
				judge=0;
				if(direct==1 &&releaseright==-1) releaseright*=-1;
				if(direct==1 &&releaseleft ==1)  releaseleft*=-1;
				if(direct==-1&&releaseright==1)  releaseright*=-1;
				if(direct==-1&&releaseleft ==-1) releaseleft*=-1;
				playSound(sattack,0);
		    }
		    break;
		case VK_TAB:
			if(start==0)
			{
			start=1;
			life=3;
			score=0;
			timer=0;
			convert=0;
			boss.x=width; boss.y=-boss.fairyh;
			boss.distx=3; boss.disty=3;
			blood=1150;
			num=0;
			warrior.x=100; warrior.y=100;
			playSound(bgm,1);
			startTimer(id1,20);
            startTimer(id2,1000);
	        startTimer(id3,5000);
	        startTimer(id4,1000);
	        }
		    break;
		default: break;
	}
}
void gameover()
{
	if(life<=0||blood>=1480)
	{
		paint_fairy();
		start=0;
        stopSound(bgm);
	}
}
void mouse(int x,int y,int button,int event)
{
	if(event!=NO_BUTTON) return;
	switch(button)
	{
		case LEFT_BUTTON:
			if(x>535&&x<888)
			if(y>330&&y<425)
			start=1;
			playSound(click,0);
			break;
		default: break;
	}
}
