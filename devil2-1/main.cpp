#include <stdio.h>
#include <math.h>

#include "donglib.h"
#include "data.h"

#define PLAY_SOUND(x)	_Play(Sound[x])
#define CLICK_DELAY		10

bool window_mode=false;
bool game_lbutton;

void SetCara(int num, int state, int frame, int hieght, int center1, int center2,
			 int center3, int center4, int center5, int width1, int width2, int width3, 
			 int width4, int width5)
{
	//����, ����
	cara_data[num].frame[state]=frame;
	cara_data[num].hieght[state]=hieght;

	//�ȴ� ����
	cara_data[num].center[state][0]=center1;
	cara_data[num].center[state][1]=center2;
	cara_data[num].center[state][2]=center3;
	cara_data[num].center[state][3]=center4;
	cara_data[num].center[state][4]=center5;

	cara_data[num].width[state][0]=width1;
	cara_data[num].width[state][1]=width2;
	cara_data[num].width[state][2]=width3;
	cara_data[num].width[state][3]=width4;
	cara_data[num].width[state][4]=width5;

	//��¥ �߽���ǥ ã��
	for(int i=1; i<5; i++)
		for(int j=0; j<i; j++)
			cara_data[num].center[state][i]-=cara_data[num].width[state][j];

	//x��ǥ
	int cumulation=0;
	for(int i=0; i<5; i++)
	{
		cara_data[num].x[state][i]=cumulation;
		cumulation+=cara_data[num].width[state][i];
	}

	//y��ǥ
	cumulation=0;
	for(int i=0; i<state; i++)
	{
		cumulation+=cara_data[num].hieght[i];
	}
	cara_data[num].y[state]=cumulation;
}

void InitCara()
{
	//�氥
	SetCara(0,0,5,113,10,131,253,377,502,122,122,120,124,125);
	SetCara(0,1,5,108,11,130,241,345,449,119,111,104,100,105);
	//����
	SetCara(1,0,3,150,0,95,190,0,0,95,95,95,0,0);
	//ū����
	SetCara(2,0,5,162,0,88,176,264,352,88,88,88,88,88);
	SetCara(2,1,4,167,0,116,236,356,0,104,115,120,121,0);
	//������
	SetCara(3,0,5,200,0,157,297,423,563,157,140,126,140,166);
	SetCara(3,1,5,250,0,148,297,460,660,148,149,150,163,199);
	//���ߵ��
	SetCara(4,0,1,121,4,0,0,0,0,64,0,0,0,0);
	//DHB
	SetCara(5,0,5,116,13,135,252,362,470,125,121,113,109,108);
	//����
	SetCara(6,0,5,152,120,227,326,432,559,121,107,99,106,127);
	SetCara(6,1,5,185,107,214,326,438,565,108,108,111,126,160);
	//���
	SetCara(7,0,5,150,118,224,321,426,550,119,106,97,105,124);
	SetCara(7,1,5,185,107,214,326,438,565,108,108,111,126,160);
	//������
	SetCara(8,0,5,170,68,148,229,308,385,79,79,78,76,76);
	//�߼���
	SetCara(9,0,1,176,297,0,0,0,0,316,0,0,0,0);
	//���庸��
	SetCara(10,0,5,162,105,240,377,514,665,145,137,137,136,152);
	SetCara(10,1,4,161,106,271,375,527,0,132,142,149,201,0);
	//�·�
	SetCara(11,0,5,169,56,149,244,348,459,93,96,103,111,114);
}

void ani(int limit, int frames)
{
	static int delay;
	++delay;

	if(delay >= limit)
	{
		int max_frame=(frames-1)*2;
		RFrame++;
		if(RFrame>=max_frame)Frame=RFrame=0;
			else if(RFrame>=frames)Frame=max_frame-RFrame;
			else Frame=RFrame;

		delay = 0;
	}
}

int Click()
{
	bool key = true;
	while(1)
	{
		if(!ProcessMessage())break;

		if(LButton)
		{
			if(!key)break;
		}
		else key = false;

		jdd->Render();
	}

	return 0;
}

int YesNo(int text)
{
	int Select=0;
	while(!Select)
	{
		if(!ProcessMessage())break;

		game_lbutton = LeftDown();
		ani(30,3);
		SetRect(&BackRect, 42, 60, 222, 200);
		_DrawBmp(&BackRect, 230, 230, 9);
		//�޴�
		if(MouseX<240)MouseX=240;
		if(MouseX>386)MouseX=386;
		if(MouseY<240)MouseY=240;
		if(MouseY>336)MouseY=336;
		for(int i=0; i<2; i++){
			if(MouseY>=280 && MouseY<340 && MouseX>=(i*60)+260 && MouseX<(i*60)+320 && LButton)
			{
				if(game_lbutton)
				{
					PLAY_SOUND(0);
					Select=i+1;
				}

				SetRect(&BackRect, 180, 0, 240, 60);
			}
			else SetRect(&BackRect, Frame*60, 0, Frame*60+60, 60);
			_DrawBmp(&BackRect, i*60+260, 280, 9);
		}
		if(text==0)PutFontOutline(260,260,WHITE, "�����ðڽ��ϱ�?");
		if(text==1)PutFontOutline(260,260,WHITE, "���Ǹ� ��ġ�ڽ��ϱ�?");
		if(text==2)PutFontOutline(260,260,WHITE, "���� �����մϱ�?");
		if(text==3)PutFontOutline(260,260,WHITE, "���� �Ͻðڽ��ϱ�?");
		PutFontOutline(280,300,WHITE, "��");
		PutFontOutline(330,300,WHITE, "�ƴ�");

		//���콺 ����Ʈ
		SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
		_DrawBmp(&BackRect, MouseX, MouseY, 9);
		jdd->Render();
	}
	return Select;
}

void Change()
{
	for(int i=1;i<=90;i+=3)
	{
		SetRect(&BackRect,0,0,100,480);
		_DrawBmp(&BackRect,i*8-100,0, 5);
		jdd->Render();
	}
}

void Talk(int caracter, char* sen1, char* sen2, char* sen3, char* sen4, char* sen5)
{
	//��
	SetRect(&BackRect, caracter*270, 0, caracter*270+270, 340);
	_DrawBmp(&BackRect, 0, 140, 8);
	//��ȭâ
	SetRect(&BackRect,0,0,320,140);
	_DrawBmp(&BackRect,289,273, 2);
	
	PutFontOutline(320,290,JColor(128,128,128),sen1);
	PutFontOutline(320,310,JColor(128,128,128),sen2);
	PutFontOutline(320,330,JColor(128,128,128),sen3);
	PutFontOutline(320,350,JColor(128,128,128),sen4);
	PutFontOutline(320,370,JColor(128,128,128),sen5);
}

void _Story(int page, int end, int pic, int bgm)
{
	if(bgm==1)_MidiPlay("music//story01.mid",true);
		else if(bgm==2)_MidiPlay("music//story06.mid",true);
		else if(bgm==3)_MidiPlay("music//story07.mid",true);

	if(pic>=0)LoadPic(0,BattleBack[pic],false);

	Change();
	while(page!=(end+1))
	{
		jdd->DrawPicture(backbuffer,"P0",0,0,NULL);
		int face=atoi(snr1[page*6]);
		Talk(face,snr1[page*6+1],snr1[page*6+2],snr1[page*6+3],snr1[page*6+4],snr1[page*6+5]);
		jdd->Render();
		Click();
		page++;
	}
}

void opinion_back()
{
	//����ȸ�ǿ� ���
	SetRect(&BackRect, 0, 0, 560, 240);
	_DrawBmp(&BackRect, 40, 40, 0);
	SetRect(&BackRect, 0, 85, 40, 125);
	for(int i=0; i<16; i++)
	{
		_DrawBmp(&BackRect, i*40, 0, 9);
		_DrawBmp(&BackRect, i*40, 280, 9);
	}
	for(int i=0; i<6; i++)
	{
		_DrawBmp(&BackRect, 0, 40+i*40, 9);
		_DrawBmp(&BackRect, 600, 40+i*40, 9);
	}
	SetRect(&BackRect, 240, 0, 302, 60);
	for(int i=0; i<11; i++)
		for(int j=0; j<3; j++)
			_DrawBmp(&BackRect, i*62, j*60+320, 9);
}

void opinion()
{
	opinion_back();
	for(int i=0; i<640; i+=10)
	{
		_DrawBarAlpha(i,230,i+10,250,BLUE);
		jdd->Render();
	}
	for(int i=0; i<256; i+=8)
	{
		char tmp_text[80];
		sprintf(tmp_text,"%d��° ����",devil.turn+1);
		PutFontOutline(280,232,JColor(i,i,i),tmp_text);
		jdd->Render();
	}
	opinion_back();
	//��ȭâ
	Talk(0,"�׷� ȸ�Ǹ� �����ϰڽ��ϴ�.","��� �ǰ��� ���� �ֽʽÿ�.","","","");
	jdd->Render();
	Click();

	char next[40];
	//���� �˻�
	strcpy(next,cStage[devil.stage]);
	strcat(next,"���� �ĵ�� ���ô�.");

	for(int i=0; i<7; i++)
	{
		opinion_back();
		if(!foll[i].life || foll[i].state>0 || foll[i].idea==4)continue;

		switch(foll[i].idea){
			case 0: //����
				if(i==0)Talk(i+1,"�غ�� �������� �ƽ��ϴ�.",next,"","","");
				if(i==1)Talk(i+1,"������ ���� ���� ����� ����մϴ�.",next,"�ΰ��� ��� ���ֹ����ڰ��!","(��� �ΰ��� �� ���� ����� �ִµ�)","");
				if(i==2)Talk(i+1,next,"���ݸ���� ���� �ֽʽÿ�.","�� ���� 50���ηε�","�̱� �ڽ��� �ֽ��ϴ�.","");
				if(i==3)Talk(i+1,"ȣȣȣ, �ΰ��༮�� ȥ�� �ݽô�.,",next,"","","");
				if(i==4)Talk(i+1,"......",next,"","","");
				if(i==5)Talk(i+1,"����������!",next,"������������!","","");
				if(i==6)Talk(i+1,next,"���� ���� ������Ḧ ������ �ͱ���.","","","");
				break;
			case 1: //¡��
				if(i==0)Talk(i+1,"���縦 ��ƾ� �ڽ��ϴ�.","���� ���������","��� ���ڽ��ϴ�.","","");
				if(i==1)Talk(i+1,"������, �ĸ����߸� ����սô�!","��������!","(�� ������ �ʹ� �ߺ���...","���ľ���!","");
				if(i==2)Talk(i+1,"���簡 �ʿ��մϴ�.","ū������ ������ �� �ֽʽÿ�.","","","");
				if(i==3)Talk(i+1,"���ߵ�߸� �����ô�!","�ܶ�~ �ܶ� ������.","ȣȣȣȪ!","","");
				if(i==5)Talk(i+1,"����������!","�氥�� �� ��ƿ�.","������������!","","");
				if(i==6)Talk(i+1,"DHB�� �����ô�.","���� �Ϳ��� �̾�����.","�������ε� ����.","","");
				break;
			case 2: //����
				if(i==1)Talk(i+1,"������ ������ �ʿ��մϴ�!","�� �������� �ͽ��ϴ�!","(�׳� ��� �;���.","�ٺ�~ �ٺ�~)","");
				if(i==2)Talk(i+1,"���� 100���θ� 200���η�","����� �ͽ��ϴ�.","������ ���� ���ֽʽÿ�.","","");
				if(i==3)Talk(i+1,"�������� ������ �� �����ϴ�.","��𰡼� �ֳ� �ϳ� ��� �Ծ�� �ڽ��ϴ�.","","","");
				if(i==4)Talk(i+1,"...���� ������ ������ �մϴ�.","","","","");
				if(i==6)Talk(i+1,"��� ������ �ϰ� �ͽ��ϴ�.","��մ� ������ ������.","��ȿȿȿ��.","","");
				break;
			case 3: //���
				if(i==0)Talk(i+1,"�ΰ����� ������ ��ƾ߰ڽ��ϴ�.","","","","");
				if(i==1)Talk(i+1,"�ΰ������ ���� ���� �� �����ϴ�.","������ �׿������ô�!","ũ������~!!!","(�θ��� �����Ѱž�. ����~)","");
				if(i==2)Talk(i+1,"�ΰ����� 50���θ�","���ֹ��� ���� �ݽô�!","","","");
				if(i==3)Talk(i+1,"������ ������ ����� �����ô�.","���� �ΰ����� �л��սô�!","","","");
				if(i==4)Talk(i+1,"...���� ���� ������ �ʿ��մϴ�.","","","","");
				if(i==5)Talk(i+1,"����������!","�ΰ����� �׿���!","��������!","","");
				if(i==6)Talk(i+1,"���� ��ᰡ �����մϴ�.","�ΰ����� �� �׿��� �ڽ��ϴ�.","������ ���� �ְ��� �����.","��ȿȿȿ��.","");
				break;
		}
		
		jdd->Render();
		Click();
	}
}

void PreMeeting()
{
	devil.idea=-1;
	for(int i=0;i<7;i++)devil.agree[i]=false;

	int mumo[]={10,-30,15,-200,-200,-200,-20};	//������
	int musa[]={-20,0,20,40,0,10,30};	//������
	int geu[]={1,1,2,2,3,1,2};			//������

	for(int i=0;i<7;i++)
	{
		//[������]���� 1Ȯ���� �ǰ��� ��
		if(!foll[i].life || foll[i].state>0 || (rand()%geu[i])>0)
		{
			foll[i].idea=4;
			continue;
		}

		int all_monster=0;
		for(int k=0; k<6; k++)
			all_monster+=devil.monster[k];

		mumo[i]-=devil.stage*10;
		if(all_monster<30-mumo[i])	//���簡 ���� ���
		{
			if(i==4 || devil.fear<30+musa[i])foll[i].idea=3;
				else foll[i].idea=1;
		}
		else	//���簡 ���� ���
		{
			if(i!=0 && i!=5 && foll[i].lv<devil.stage/2)foll[i].idea=2;
				else
				{
					devil.agree[i]=true;
					foll[i].idea=0;
				}
		}
	}

	//Ȥ�� ���̳ʽ� ���Ͱ� �ִ��� �˻�
	for(int i=0;i<4;i++)
		if(devil.monster[i]<0)devil.monster[i]=0;

	tab=0;
	LoadPic(0,MapBMP[devil.stage],false);
	LoadPic(6,"DATA//monster.png");
	LoadPic(7,"DATA//tab.png");

	opinion();
}

void StartGame()
{
	int i;
	_Story(0,6,8,1);
	mode=2;
	to_menu=false;
	//���� �� �ʱ�ȭ
	for(int i=0;i<6;i++)devil.monster[i]=0;
	//��� �ʱ�ȭ
	for(int i=0;i<7;i++)
	{
		foll[i].bear=5-(i/2);		//������
		foll[i].state=0;			//����(0:���� 1:�λ� 2:�г� 3:����)
		foll[i].delay=0;			//��������
		if(i<2)foll[i].life=true;	//����
			else foll[i].life=false;
		foll[i].lv=1;				//����
	}
	devil.score=100;devil.mp=100;devil.fear=50;
	devil.stage=devil.turn=0;
	devil.stage=0;
	Change(); PreMeeting();
	_MidiPlay("music//meeting.mid",true);
}

bool load(char* fname)
{
	int i;
	FILE *Fp;
	if(!(Fp=fopen(fname,"r")))return false;

	int buffer[54];
	fread(buffer,sizeof(int),54,Fp);

	//��������, ��
	devil.stage=buffer[0];
	devil.turn=buffer[1];
	//���� ��
	for(int i=0;i<6;i++)
		devil.monster[i]=buffer[i+2];
	//�³�/����
	for(int i=0;i<7;i++)
	{
		if(buffer[i+8]==53)devil.agree[i]=true;
			else devil.agree[i]=false;
	}
	//��Ÿ ����
	devil.mp=buffer[15];
	devil.score=buffer[16];
	devil.idea=buffer[17];
	devil.fear=buffer[18];

	//����� ����
	for(int i=0; i<7; i++)
	{
		int j=19+i*5;
		foll[i].delay=buffer[j];
		foll[i].lv=buffer[j+1];
		foll[i].idea=buffer[j+2];
		foll[i].bear=buffer[j+3];
		foll[i].state=buffer[j+4];
	}

	for(int i=0; i<7; i++)
		foll[i].life=true;
	for(int i=2; i<7; i++)
		foll[i].life=false;
	if(devil.stage>0)foll[2].life=true;
	if(devil.stage>1)foll[5].life=true;
	if(devil.stage>2){foll[3].life=true; foll[4].life=true;}
	if(devil.stage>3)foll[6].life=true;

	fclose(Fp);
	return true;
}

void save(char* fname)
{
	int i;
	FILE *Fp;
	Fp=fopen(fname,"w");

	int buffer[54];

	//��������, ��
	buffer[0]=devil.stage;
	buffer[1]=devil.turn;
	//���� ��
	for(int i=0;i<6;i++)
		buffer[i+2]=devil.monster[i];
	//�³�/����
	for(int i=0;i<7;i++)
	{
		int tmp;
		if(devil.agree[i])tmp=53;
			else tmp=50;
		buffer[i+8]=tmp;
	}
	//��Ÿ ����
	buffer[15]=devil.mp;
	buffer[16]=devil.score;
	buffer[17]=devil.idea;
	buffer[18]=devil.fear;

	//����� ����
	for(int i=0; i<7; i++)
	{
		int j=19+i*5;
		buffer[j]=foll[i].delay;
		buffer[j+1]=foll[i].lv;
		buffer[j+2]=foll[i].idea;
		buffer[j+3]=foll[i].bear;
		buffer[j+4]=foll[i].state;
	}

	fwrite(buffer,sizeof(int),54,Fp);

	fclose(Fp);
}

int View(bool input)
{
	int Select=0;
	char* fname[4]={"devil1.sav","devil2.sav","devil3.sav","devil4.sav"};
	int fnum[4];int fturn[4]; bool fabl[4];
	FILE* Fp;
	for(int i=0;i<4;i++)
	{
		if(!(Fp=fopen(fname[i],"r"))){
			fabl[i]=false;
			continue;
		}
		else
		{
			fabl[i]=true;
			int buffer[2];
			fread(buffer,sizeof(int),2,Fp);
			fnum[i]=buffer[0];
			fturn[i]=buffer[1];

			fclose(Fp);
		}
	}
	while(!Select){
		if(!ProcessMessage())break;
		{
			game_lbutton = LeftDown();

			ani(30,3);
			SetRect(&BackRect, 42, 60, 222, 200);
			_DrawBmp(&BackRect, 230, 230, 9);
			//�޴�
			if(MouseX<240)MouseX=240;
			if(MouseX>386)MouseX=386;
			if(MouseY<240)MouseY=240;
			if(MouseY>336)MouseY=336;
			for(int i=0; i<4; i++){
				if(fabl[i])
				{
					char tmp_text[80];
					sprintf(tmp_text,"%s %d��",cStage[fnum[i]],fturn[i]);
					PutFontOutline(240,240+(i*20),WHITE,tmp_text);
				}
				else PutFontOutline(240,240+(i*20),BLUE,"����ִܸ��̴�!");
				if(MouseY>=240+(i*20) && MouseY<260+(i*20))
				{
					_DrawBarAlpha(240,240+(i*20),399,259+(i*20),BLUE);
					if(game_lbutton)
					{
						if(!input&&!fabl[i]);
						else{
							PLAY_SOUND(0);
							Select=i+1;
						}
					}
				}
			}
			PutFontOutline(300,340,RED,"���");
			if(MouseY>325)
			{
				_DrawBarAlpha(240,340,399,359,BLUE);
				if(game_lbutton)
				{
					PLAY_SOUND(0);
					return 0;
				}
			}
			//���콺 ����Ʈ
			SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
			_DrawBmp(&BackRect, MouseX, MouseY-10, 9);
			jdd->Render();
		}
	}
	if(!input){
		load(fname[Select-1]);
		tab=0;
		LoadPic(0,MapBMP[devil.stage],false);
		LoadPic(6,"DATA//monster.png");
		LoadPic(7,"DATA//tab.png");
		Change(); _MidiPlay("music//meeting.mid",true);
		mode=2;
	}
	else save(fname[Select-1]);
	return 0;
}

//ȥ�Ҿ� ����
int Dictionary()
{
	bool End=false;
	int word=-1;
	LoadPic(0,"DATA//dic.jpg",false);
	char* cWord[]={"��ݶ�","����","�̾�","����","����� ���","����������","����� ���հ�","ȥ���� ���","ȥ��"};
	while(!End){
		if(!ProcessMessage())break;

		game_lbutton = LeftDown();

		ani(10,3);
		jdd->DrawPicture(backbuffer,"P0",0,0,NULL);
		
		if(word==-1)
		{
			for(int i=0; i<9; i++)
			{
				int ty=40+i*20;
				if(MouseX>=40 && MouseX<200 && MouseY>=ty && MouseY<ty+20)
				{
					PutFontOutline(40,ty,YELLOW,cWord[i]);
					if(game_lbutton)word=i;
				}
				else
					PutFontOutline(40,ty,WHITE,cWord[i]);
			}
			if(MouseX>=560 && MouseX<610 && MouseY>=420 && MouseY<440)
			{
				PutFontOutline(560,420,YELLOW,"������");
				if(game_lbutton)End=true;
			}
			else
				PutFontOutline(560,420,WHITE,"������");
		}
		else
		{
			if(word==0)
			{
				PutFontOutline(40,40,YELLOW,"��ݶ�");
				PutFontOutline(40,80,WHITE,"���սô밡 ������ ������ �־��� ���.");
				PutFontOutline(40,100,WHITE,"����� ����� ��� �������� ���������� �ϼ��ؼ�");
				PutFontOutline(40,120,WHITE,"ȥ���� ������� ����� �� ���.");
			}
			else if(word==1)
			{
				PutFontOutline(40,40,YELLOW,"����");
				PutFontOutline(40,80,WHITE,"��� ����� ��� �ΰ��� �������� �ΰ��� ���¸� ������ ���� ����.");
				PutFontOutline(40,100,WHITE,"���� �����鵵 �ΰ��̾����� ��Ҵ���� ���� ��⹰�� �����鼭");
				PutFontOutline(40,120,WHITE,"�ű⼭ ���� �̻��� ������ �����Ǽ� ������ �� ������ �˷��� �ִ�.");
			}
			else if(word==2)
			{
				PutFontOutline(40,40,YELLOW,"�̾�");
				PutFontOutline(40,80,WHITE,"����(����)�� �ش��ϴ� ȥ�Ҿ�.");
				PutFontOutline(40,100,WHITE,"������ �ణ�� ���̴� �ִ�.");
				PutFontOutline(40,120,WHITE,"�̾��� �ΰ��� ������ ���� ������ ��Ī���� �Ĺ��� �������� �����Ѵ�.");
				PutFontOutline(40,140,WHITE,"");
			}
			else if(word==3)
			{
				PutFontOutline(40,40,YELLOW,"����");
				PutFontOutline(40,80,WHITE,"������ �ش��ϴ� ȥ�Ҿ�.");
				PutFontOutline(40,100,WHITE,"������ �� �� �ִ� ����� �������̶�� �θ���.");
			}
			else if(word==4)
			{
				PutFontOutline(40,40,YELLOW,"����� ���");
				PutFontOutline(40,80,WHITE,"ȥ���� ����� ���� �����ϴ� 3���� ����� �ϳ�");
				PutFontOutline(40,100,WHITE,"ȥ���� ����� ������ ����� ��Ұ�,");
				PutFontOutline(40,120,WHITE,"����� ����� ������ ������ �������� �ٿ����� �ǰ� �ִ�.");
				PutFontOutline(40,140,WHITE,"�������� ������ ��κ� ��ο, ����� 3���� ���� ���̴�.");
			}
			else if(word==5)
			{
				PutFontOutline(40,40,YELLOW,"���� ������");
				PutFontOutline(40,80,WHITE,"�������� ���ϴ� ���� ��Ḧ ��ƿ��� �̾�.");
				PutFontOutline(40,100,WHITE,"���������ε� ���δ�.");
			}
			else if(word==6)
			{
				PutFontOutline(40,40,YELLOW,"����� ���հ�");
				PutFontOutline(40,80,WHITE,"���սô� 2��° ���հ�.");
				PutFontOutline(40,100,WHITE,"ó�� ���սô븦 �� ������ ����� 1������ ���ϰ� ����.");
				PutFontOutline(40,120,WHITE,"���սô��� ��κ��� �����ϴ� ���հ�.");
			}
			else if(word==7)
			{
				PutFontOutline(40,40,YELLOW,"ȥ���� ���");
				PutFontOutline(40,80,WHITE,"ȥ���� ����� ���� �����ϴ� 3���� ����� �ϳ�");
				PutFontOutline(40,100,WHITE,"�ΰ����� ���� ��� ���� ��ȭ������ ������ �ִ�.");
				PutFontOutline(40,120,WHITE,"ȥ�� ������� ����� �����ϰ�, ���� �����̴�.");
				PutFontOutline(40,140,WHITE,"�߿��� �������� �峭�� �ļ� ������ �Ǳ⵵ �Ѵ�.");
			}
			else if(word==8)
			{
				PutFontOutline(40,40,YELLOW,"ȥ��");
				PutFontOutline(40,80,WHITE,"'ȥ���� ���'�� ���.");
				PutFontOutline(40,100,WHITE,"ȥ���� ����� ���� ȥ���� ���� �ƴ� ȥ������ �θ��⵵ �Ѵ�.");
				PutFontOutline(40,120,WHITE,"");
				PutFontOutline(40,140,WHITE,"");
			}
			if(MouseX>=580 && MouseX<620 && MouseY>=420 && MouseY<440)
			{
				PutFontOutline(580,420,YELLOW,"�ڷ�");
				if(game_lbutton)word=-1;
			}
			else
				PutFontOutline(580,420,WHITE,"�ڷ�");
		}

		//���콺 ����Ʈ
		SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
		_DrawBmp(&BackRect, MouseX-7, MouseY-20, 9);
		jdd->Render();
	}

	LoadPic(0,"DATA//title.jpg",false);
	return 0;
}

//����
int Ending()
{
	bool End=false;
	LoadPic(0,"DATA//back7.jpg",false);
	LoadPic(6,"DATA//rank.png");
	
	int y=480;
	char* text[]={"����� 3���� ������ ȥ�� ��ü�� �����ϰ� �ȴ�.",
				"ó���� ���εǼ� ������ ����� ����� ������",
				"������ ������ ���� ���̴�.",
				"����� 3���� ������ ȥ�� ��ü�� �����ϰ� �ȴ�.",
				"������ �״� �Ƶ鳻�̸� �׳� ��������",
				"�Ŀ� �׿� �ֽ��� ������ ����ų �����",
				"�޿��� ���� ���ߴ� ���̴�."};

	_MidiPlay("music//end.mid", true);
	while(!End){
		if(!ProcessMessage())break;
		{
			ani(10,3);
			jdd->DrawPicture(backbuffer,"P0",0,0,NULL);
			
			static int scroll_delay;
			++scroll_delay;
			if(scroll_delay >= 5)
			{
				scroll_delay=0;
				y--;
			}

			for(int i=0; i<7; i++)
			{
				int tx=320-strlen(text[i])*4;
				int ty=y+i*20;

				if(ty>=0 && ty<50)
				{
					int color=ty*5;
					PutFontOutline(tx,ty,JColor(color,color,color),text[i]);
				}
				else if(ty>=50 && ty<430)
				{
					PutFontOutline(tx,ty,WHITE,text[i]);
				}
				else if(ty>=430 && ty<480)
				{
					int color=250-((ty-430)*5);
					PutFontOutline(tx,ty,JColor(color,color,color),text[i]);
				}
			}

			if(y<-140 || _GetKeyState(VK_ESCAPE))End=true;

			jdd->Render();
		}
	}

	int rank;
	if(devil.score>100)rank=7;
		else if(devil.score>90)rank=6;
		else if(devil.score>80)rank=5;
		else if(devil.score>70)rank=4;
		else if(devil.score>60)rank=3;
		else if(devil.score>50)rank=2;
		else if(devil.score>40)rank=1;
		else rank=0;

	jdd->DrawPicture(backbuffer,"P0",0,0,NULL);
	PutFontOutline(270,200,WHITE,"����� ���");
	SetRect(&BackRect, 0, rank*40, 120, rank*40+40);
	_DrawBmp(&BackRect, 260, 220, 6);

	char tmp_text[80];
	sprintf(tmp_text,"����:%d",devil.score);
	PutFontOutline(280,300,WHITE,tmp_text);
	jdd->Render();
	Click();

	return 0;
}

//���Ӹ�� 0
void _MainMenu()
{
	ani(10,3);
	jdd->DrawPicture(backbuffer,"P0",0,0,NULL);

	//�޴�
	for(int i=0; i<8; i++)
	{
		if(i>=1 && i<=3)continue;
		if(MouseX>=0 && MouseX<60 && MouseY>=(i*60) && MouseY<(i*60)+60 && LButton)
		{
			if(game_lbutton)
			{
				PLAY_SOUND(0);
				Select=i+1;
			}

			SetRect(&BackRect, 180, 0, 240, 60);
		}
		else SetRect(&BackRect, Frame*60, 0, Frame*60+60, 60);
		_DrawBmp(&BackRect, 0, i*60, 9);
	}
	//���콺 ����Ʈ
	SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
	_DrawBmp(&BackRect, MouseX, MouseY, 9);
	jdd->Render();
	if(Select){
		switch(Select)
		{
			case 1:	case 2: case 3: case 4:
				StartGame(); break;
			case 5: Dictionary(); break;
			case 6: View(false);
				break;
			case 7: LoadPic(0,"DATA//staff.jpg",false);
				Change(); jdd->DrawPicture(backbuffer,"P0",0,0,NULL); jdd->Render(); Click();
				LoadPic(0,"DATA//title.jpg",false);
				Change();
				break;
			case 8: if(YesNo(0)==1)Quit=true; break;
		}
		Select=0;
	}
}


int kill()
{
	int time=4999, die=0, pow=0, many=5, i;
	int manx[5], manl[5], mand[5];
	for(int i=0;i<many;i++){
		manl[i]=rand()%30; mand[i]=rand()%2;
		manx[i]=rand()%620;
	}
	LoadPic(0,"DATA//back1.jpg",false);
	_MidiPlay("music//hunt.mid", true);

	while(time>0)
	{
		if(!ProcessMessage())break;

		ani(20,3);
		jdd->DrawPicture(backbuffer,"P0",0,0,NULL);
		for(int i=0;i<many;i++)
		{
			if(manl[i]>0)manl[i]--;
			if(!(rand()%100))mand[i]=(mand[i]==0)?1:0;
			if(manx[i]<=0)mand[i]=1;
			if(manx[i]>=619)mand[i]=0;
			if(mand[i])manx[i]+=3;
			if(!mand[i])manx[i]-=3;
			if(!manl[i])
			{
				SetRect(&BackRect, 222+Frame*20, 160, 242+Frame*20, 180);
				_DrawBmp(&BackRect, manx[i], 380+i*20, 9);
			}
		}
		//���콺 ����Ʈ
		if(pow<100)_DrawBox(MouseX-(5+Frame),MouseY-(5+Frame),MouseX+(5+Frame),MouseY+(5+Frame),RED);
			else _DrawBox(MouseX-(5+Frame),MouseY-(5+Frame),MouseX+(5+Frame),MouseY+(5+Frame),BLUE);
		_Pixel(MouseX, MouseY, BLUE);

		if(LeftDown()&&pow==100)
		{
			PLAY_SOUND(15);
			pow=0;
			for(int i=0;i<many;i++){
				if(MouseX>=manx[i] && MouseX<=manx[i]+20 && MouseY>=i*20+380 && MouseY<=i*20+400 && !manl[i])
				{
					PLAY_SOUND(14);
					die++; manl[i]=rand()%20+10;
				}
			}
		}

		char tmp_text[80];
		sprintf(tmp_text,"�ð�: %d",time/100);
		PutFontOutline(0,0,WHITE,tmp_text);
		sprintf(tmp_text,"�θ���: %d",die);
		PutFontOutline(100,0,WHITE,tmp_text);
		_DrawBar(300,0,300+pow,20,RED);
		_DrawBox(300,0,400,20,WHITE);
		time--; if(pow<100)pow+=10;
		jdd->Render();
	}

	SetRect(&BackRect, 42, 60, 222, 200);
	_DrawBmp(&BackRect, 230, 170, 9);
	char tmp_text[80];
	sprintf(tmp_text,"�ΰ����� ����+%d",die);
	PutFontOutline(250,230,YELLOW,tmp_text);
	jdd->Render();

	return die;
}

int draft() //¡���Լ�
{
	int origin[6];
	for(int i=0; i<6; i++)	//���� ���� ���
		origin[i]=devil.monster[i];

	bool End=false, Pause=false;
	int deal[]={2,3,3,4,5,3};
	int command=10;

	while(!End)
	{
		if(!ProcessMessage())break;
		
		game_lbutton = LeftDown(CLICK_DELAY);
		ani(5,3);
		SetRect(&BackRect, 240, 0, 302, 60);
		for(int i=0; i<11; i++)
			for(int j=0; j<8; j++)
				_DrawBmp(&BackRect, i*62, j*60, 9);
		
		for(int i=0; i<6; i++)
		{
			SetRect(&BackRect, i*70, 0, 70+i*70, 100);
			_DrawBmp(&BackRect, 85+i*80, 30, 3);
			if(devil.monster[i]>origin[i])
			{
				char tmp_text[80];
				sprintf(tmp_text,"%02d",devil.monster[i]);
				PutFontOutline(115+i*80,110,YELLOW,tmp_text);
			}
			else
			{
				char tmp_text[80];
				sprintf(tmp_text,"%02d",devil.monster[i],tmp_text);
				PutFontOutline(115+i*80,110,WHITE,tmp_text);
			}
		}

		//����
		SetRect(&BackRect, 0, 0, 270, 340);
		_DrawBmp(&BackRect, 0, 140, 8);
		PutFontOutline(290,190,WHITE,"���");
		char tmp_text[80];
		sprintf(tmp_text,"����: %d",devil.mp);
		PutFontOutline(290,220,WHITE,tmp_text);
		SetRect(&BackRect, 0, 125, 18, 145);
		for(int i=0; i<command; i++)
			_DrawBmp(&BackRect, 330+i*20, 189, 9);

		//¡��
		for(int i=0; i<6; i++)
		{
			int ButtonX=135+i*80;
			int ButtonY=100;
			SetRect(&BackRect, 780, 0, 810, 30);
			if(MouseX>=ButtonX && MouseX<ButtonX+30 && MouseY>=ButtonY && MouseY<ButtonY+30)
			{
				if(game_lbutton && command>0 && devil.mp>=deal[i] && devil.monster[i]<99)
				{
					command--;
					devil.monster[i]++;
					devil.mp-=deal[i];
					SetRect(&BackRect, 780, 30, 810, 60);
					Pause=true;
				}
			}
			_DrawBmp(&BackRect, ButtonX, ButtonY, 7);
		}
		//���
		for(int i=0; i<6; i++)
		{
			int ButtonX=85+i*80;
			int ButtonY=100;
			SetRect(&BackRect, 810, 0, 840, 30);
			if(MouseX>=ButtonX && MouseX<ButtonX+30 && MouseY>=ButtonY && MouseY<ButtonY+30)
			{
				if(game_lbutton && devil.monster[i]>origin[i])
				{
					command++;
					devil.monster[i]--;
					devil.mp+=deal[i];
					SetRect(&BackRect, 810, 30, 840, 60);
					Pause=true;
				}
			}
			_DrawBmp(&BackRect, ButtonX, ButtonY, 7);
		}
		
		//¡�� ��
		if(MouseX>=580 && MouseY>=420 && LButton)
		{
			if(game_lbutton)
			{
				PLAY_SOUND(0);
				End=true;
			}
			SetRect(&BackRect, 180, 0, 240, 60);
		}
		else SetRect(&BackRect, Frame*60, 0, Frame*60+60, 60);
		_DrawBmp(&BackRect, 580, 420, 9);
		PutFontOutline(593,440,WHITE,"����");

		//���콺 ����Ʈ
		SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
		_DrawBmp(&BackRect, MouseX-7, MouseY-20, 9);
		jdd->Render();
	}

	return 0;
}

bool Hurt(int who, int damage)
{
	if(damage<=0)PLAY_SOUND(8);
		else PLAY_SOUND(6);

	unit[who].hp-=damage;

	while(unit[who].hp<=0)
	{
		unit[who].hp+=8;
		unit[who].force--;
		if(unit[who].force<=0)return true;
	}

	return false;
}

void StateTo(int who, int state)
{
	cara[who].state=state;
	cara[who].end_action=false;
	cara[who].frame=0;
	cara[who].real_frame=0;
	cara[who].tempo=0;
	if(state==1)
	{
		PLAY_SOUND(7);
		cara[who].attack=false;
	}
}

int VS(bool who, int army, int enemy)
{
	cara[0].unit=army;
	cara[1].unit=enemy;

	//���� ����� ��� ������
	int time=5;
	bool End=false;

	cara[0].frz=cara[1].frz=0;
	if(who)
	{
		int def=unit[cara[0].unit].agl-unit[cara[1].unit].agl;
		if(def<0)time=Max(time+def,1);
			else if(def>0)cara[1].frz=def;
	}
	else
	{
		int def=unit[cara[1].unit].agl-unit[cara[0].unit].agl;
		if(def<0)time=Max(time+def,1);
			else if(def>0)cara[0].frz=def;
	}
	time=time*200-1;

	//�ʱ�ȭ ������
	for(int i=0; i<2; i++)
	{
		cara[i].kind=unit[cara[i].unit].kind;
		if(cara[i].kind>5)cara[i].kind-=10;
		cara[i].end_action=false;
		cara[i].state=0;
		cara[i].soundtempo=cara[i].tempo=cara[i].frame=cara[i].real_frame=0;
		cara[i].attack=true;
		cara[i].dam=0;
	}
	int clip_left=cara_data[cara[1].kind].width[0][0];
	int clip_right=640-cara_data[cara[0].kind].width[0][0];

	cara[0].x=clip_right;
	cara[1].x=clip_left;

	//������ ����
	if(cara[0].kind==0 || cara[0].kind==2 || cara[0].kind==3)cara[0].fight=true;
		else cara[0].fight=false;
	if(cara[1].kind==6 || cara[1].kind==7 || cara[1].kind==10)cara[1].fight=true;
		else cara[1].fight=false;
   
	//�׷��� �ʱ�ȭ
	LoadPic(6,Carac[cara[0].kind]);
	LoadPic(7,Carac[cara[1].kind]);

	while(!End)
	{
		if(!ProcessMessage())break;
		game_lbutton = LeftDown();

		//�÷��̾� ����
		if(cara[0].frz<=0)
		{
			if(cara[0].state==0)
			{
				if(cara[0].fight && game_lbutton)StateTo(0,1);

				if(_GetKeyState(VK_LEFT))
				{
					int speed=unit[army].mov;
					cara[0].x=Max(cara[0].x-speed,cara[1].x);
					//�ȴ� �Ҹ�
					cara[0].soundtempo++;
					if(cara[0].soundtempo>20)
					{
						PLAY_SOUND(Smov[unit[cara[0].unit].kind]);
						cara[0].soundtempo=0;
					}
				}
				else if(_GetKeyState(VK_RIGHT))
				{
					int speed=unit[army].mov;
					cara[0].x=Min(cara[0].x+speed,clip_right);
					//�ȴ� �Ҹ�
					cara[0].soundtempo++;
					if(cara[0].soundtempo>20)
					{
						PLAY_SOUND(Smov[unit[cara[0].unit].kind]);
						cara[0].soundtempo=0;
					}
				}
			}
		}

		//��� �κ�
		jdd->DrawPicture(backbuffer,"P0",0,0,NULL);

		//���� �ð�
		int sec=(time/100)+1;
		for(int i=0; i<sec; i++)
		{
			SetRect(&BackRect,302,0,347,45);
			_DrawBmp(&BackRect, 300-sec*23+i*45, 10, 9);
		}

		for(int i=0; i<2; i++)
		{
			int minus=(i==0)?-1:1;
			int opp=1-i;
			cara[i].tempo++;

			//ĳ���� ����
			int tempo_max=(cara[i].state==0)?10:5;
			if(cara[i].tempo > tempo_max)
			{
				int frames=cara_data[cara[i].kind].frame[cara[i].state];
				int max_frame=(frames-1)*2;
				cara[i].real_frame++;
				if(cara[i].real_frame>=max_frame)
				{
					cara[i].frame=cara[i].real_frame=0;
					cara[i].end_action=true;
					cara[i].attack=true;

					//�ߵ�
					if(unit[cara[i].unit].poisoned)
					{
						if(Hurt(cara[i].unit,1))End=true;
					}
				}
				else if(cara[i].real_frame>=frames)cara[i].frame=max_frame-cara[i].real_frame;
				else cara[i].frame=cara[i].real_frame;
				cara[i].tempo=0;
			}

			//�׸��� ������ ��ġ, ũ�� & �׸��� ���� ��ġ
			int rx=cara_data[cara[i].kind].x[cara[i].state][cara[i].frame];
			int ry=cara_data[cara[i].kind].y[cara[i].state];
			int rxx=rx+cara_data[cara[i].kind].width[cara[i].state][cara[i].frame];
			int ryy=ry+cara_data[cara[i].kind].hieght[cara[i].state];
			int px=cara[i].x-cara_data[cara[i].kind].center[cara[i].state][cara[i].frame];
			int py=440-cara_data[cara[i].kind].hieght[cara[i].state];
			
			SetRect(&BackRect, rx,ry,rxx,ryy);
			if(cara[i].dam>0)
			{
				px+=((cara[i].dam%3)-1)*3;
				cara[i].dam--;
			}
			_DrawBmp(&BackRect, px, py, i+6);

			if(!cara[i].attack && cara[i].real_frame<cara_data[cara[i].kind].frame[cara[i].state])
			{
				int ax;
				bool sucess=false;
				//����
				if(i==0)	//�Ʊ��� ����
				{
					ax=px;
					if(cara[opp].x>ax)sucess=true;
				}
				else		//������ ����
				{
					ax=px+(rxx-rx);
					if(cara[opp].x<ax)sucess=true;
				}

				if(sucess)
				{
					int damage=Max(unit[cara[i].unit].att*2-unit[cara[opp].unit].def,0);
					cara[opp].dam=30;
					if(Hurt(cara[opp].unit,damage))End=true;
					if(rand()%100 < unit[cara[i].unit].poison && !unit[cara[opp].unit].poisoned)
					{
						PLAY_SOUND(9);
						unit[cara[opp].unit].poisoned=true;
					}
					cara[i].attack=true;
				}
			}

			//A.I.
			if(i==1 && cara[i].fight && cara[i].frz<=0)
			{
				cara[i].x=Min(cara[i].x+unit[cara[i].unit].mov,cara[opp].x);
				int ax=px+(rxx-rx);
				if(cara[opp].x<ax && cara[i].state==0)StateTo(i,1);
				//�ȴ� �Ҹ�
				if(cara[1].state==0)cara[1].soundtempo++;
				if(cara[1].soundtempo>20)
				{
					PLAY_SOUND(Smov[unit[cara[1].unit].kind]);
					cara[1].soundtempo=0;
				}
			}

			//ĳ���� ����
			//ü��, ���
			SetRect(&BackRect, 0,145,34,175);
			int space=(i==0)?606:0;
			//���� ü��
			int i2;
			for(i2=0; i2<unit[cara[i].unit].hp; i2++)
				_DrawBmp(&BackRect, space, 210-i2*30, 9);
			//���� ü��
			SetRect(&BackRect, 0,175,34,205);
			for(; i2<8; i2++)
				_DrawBmp(&BackRect, space, 210-i2*30, 9);
			
			//���
			char tmp_text[80];
			sprintf(tmp_text,"%02d",unit[cara[i].unit].force);
			PutFontOutline(space+9,240,WHITE,tmp_text);

			//�����̻�
			if(unit[cara[i].unit].poisoned)
			{
				SetRect(&BackRect,0,125,18,145);
				_DrawBmp(&BackRect, space+8, 260, 9);
			}
			if(cara[i].frz>0)
			{
				SetRect(&BackRect,282,160,302,190);
				_DrawBmp(&BackRect, space+7, 280, 9);
			}

			//���¿� ���� ������
			if(cara[i].state>0 && cara[i].end_action)
			{
				StateTo(i,0);
			}
		}

		jdd->Render();
		if(--time<0)End=true;
		if((time%200)==0)
		{
			for(int i=0; i<2; i++)
				if(cara[i].frz>0)cara[i].frz--;
		}
	}

	LoadPic(6,"DATA//unit.png");
	LoadPic(7,"DATA//land.png");

	return 0;
}

void DrawMap()
{
	//ĳ���� �׸� ����
	int sprYY[]={80,90,90,110,55,60,0,0,0,0,0,0,0,0,0,0,90,90,90,70,85,75};
	int sprY[22];
	int Ys=0;
	for(int i=0; i<22; i++)
	{
		sprY[i]=Ys;
		Ys+=sprYY[i];
	}

	int x,y;
	int cx=focusX/60, cy=focusY/60;
	int cxx=Min(cx+12,16), cyy=Min(cy+12,16);
	//���� Ÿ��
	for(y=cy; y<cyy; y++)
		for(x=cx; x<cxx; x++)
		{
			if(map_tile[x][y]==1 || map_tile[x][y]<12)continue;
			int tx=x*60-focusX, ty=y*60-focusY;
			SetRect(&BackRect,map_tile[x][y]*70,0,60+map_tile[x][y]*70,60);
			_DrawBmp(&BackRect, tx, ty, 7);
		}
	//���� Ÿ��
	for(y=cy; y<cyy; y++)
		for(x=cx; x<cxx; x++)
		{
			if(map_tile[x][y]==1 || map_tile[x][y]>=12)continue;
			int tx=x*60-focusX, ty=y*60-focusY;
			SetRect(&BackRect,map_tile[x][y]*70,0,70+map_tile[x][y]*70,70);
			_DrawBmp(&BackRect, tx, ty, 7);
		}
	//ĳ����&����
	for(y=cy; y<cyy; y++)
		for(x=cx; x<cxx; x++)
		{
			int tx=x*60-focusX, ty=y*60-focusY;
			if(map_tile[x][y]==1)	//����
			{
				SetRect(&BackRect,map_tile[x][y]*70,0,60+map_tile[x][y]*70,80);
				_DrawBmp(&BackRect, tx, ty-20, 7);
			}
			else	//ĳ����
			{
				if(map_spr[x][y]!=99)
				{
					int spr_kind=unit[map_spr[x][y]].kind;
					if(unit[map_spr[x][y]].turn)SetRect(&BackRect,180,sprY[spr_kind],240,sprY[spr_kind]+sprYY[spr_kind]);	//�� ���� �δ�
						else if(spr_kind==19)SetRect(&BackRect,0,sprY[spr_kind],60,sprY[spr_kind]+sprYY[spr_kind]);			//�ִϸ��̼��� ���� �δ�
						else SetRect(&BackRect,60*Frame,sprY[spr_kind],60+60*Frame,sprY[spr_kind]+sprYY[spr_kind]);
					_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind]), 6);
				}
			}
		}
	//ĳ���� ����
	for(y=cy; y<cyy; y++)
		for(x=cx; x<cxx; x++)
		{
			if(map_spr[x][y]!=99 && unit[map_spr[x][y]].force>0)
			{
				int tx=x*60-focusX, ty=y*60-focusY;
				char tmp_text[80];
				sprintf(tmp_text,"%02d",unit[map_spr[x][y]].force);
				PutFontOutline(tx+35,ty+40,WHITE,tmp_text);

				//�ߵ�
				if(unit[map_spr[x][y]].poisoned)
				{
					SetRect(&BackRect,0,125,18,145);
					_DrawBmp(&BackRect, tx, ty+35, 9);
				}

				SetRect(&BackRect,18,125,23,130);
				for(int i=0; i<unit[map_spr[x][y]].hp; i++)
					_DrawBmp(&BackRect, tx+50, ty+35-i*5, 9);
			}
		}
}

void DrawMapS(int special, int who)	//Ư�� ��Ȳ�� �׸��� �Լ�
{
	//ĳ���� �׸� ����
	int sprYY[]={80,90,90,110,55,60,0,0,0,0,0,0,0,0,0,0,90,90,90,70,85,75};
	int sprY[22];
	int Ys=0;
	bool show_data=true;
	for(int i=0; i<22; i++)
	{
		sprY[i]=Ys;
		Ys+=sprYY[i];
	}

	int x,y;
	int cx=focusX/60, cy=focusY/60;
	int cxx=Min(cx+12,16), cyy=Min(cy+12,16);
	//���� Ÿ��
	for(y=cy; y<cyy; y++)
		for(x=cx; x<cxx; x++)
		{
			if(map_tile[x][y]==1 || map_tile[x][y]<12)continue;
			int tx=x*60-focusX, ty=y*60-focusY;
			SetRect(&BackRect,map_tile[x][y]*70,0,60+map_tile[x][y]*70,60);
			_DrawBmp(&BackRect, tx, ty, 7);
		}
	//���� Ÿ��
	for(y=cy; y<cyy; y++)
		for(x=cx; x<cxx; x++)
		{
			if(map_tile[x][y]==1 || map_tile[x][y]>=12)continue;
			int tx=x*60-focusX, ty=y*60-focusY;
			SetRect(&BackRect,map_tile[x][y]*70,0,70+map_tile[x][y]*70,70);
			_DrawBmp(&BackRect, tx, ty, 7);
		}
	//ĳ����&����
	for(y=cy; y<cyy; y++)
		for(x=cx; x<cxx; x++)
		{
			int tx=x*60-focusX, ty=y*60-focusY;
			if(map_tile[x][y]==1)	//����
			{
				SetRect(&BackRect,map_tile[x][y]*70,0,60+map_tile[x][y]*70,80);
				_DrawBmp(&BackRect, tx, ty-20, 7);
			}
			else	//ĳ����
			{
				if(map_spr[x][y]!=99)
				{
					int spr_kind=unit[map_spr[x][y]].kind;
					if(unit[map_spr[x][y]].turn)SetRect(&BackRect,180,sprY[spr_kind],240,sprY[spr_kind]+sprYY[spr_kind]);	//�� ���� �δ�
						else if(spr_kind==19)
						{
							if(map_spr[x][y]==who && special<3)
								SetRect(&BackRect,60*special,sprY[spr_kind],60+60*special,sprY[spr_kind]+sprYY[spr_kind]);
							else
								SetRect(&BackRect,0,sprY[spr_kind],60,sprY[spr_kind]+sprYY[spr_kind]);
						}
						else SetRect(&BackRect,60*Frame,sprY[spr_kind],60+60*Frame,sprY[spr_kind]+sprYY[spr_kind]);
					
					
					if(map_spr[x][y]==who)
					{
						//�߼��� �׸���
						if(special<3)
						{
							_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind]), 6);
						}
						//������� ��鸲
						else if(special<6)
						{
							special-=4;
							_DrawBmp(&BackRect, tx+special*5, ty+(55-sprYY[spr_kind]), 6);
						}
						//�Ǽ�
						else if(special<26)
						{
							special-=6;
							_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind]), 6);
							SetRect(&BackRect,0,1025,60,1045);
							_DrawBmp(&BackRect, tx, ty+40-(special*2), 6);
						}
						//���
						else if(special<46)
						{
							special-=26;
							SetRect(&BackRect,180,sprY[spr_kind],240,sprY[spr_kind]+sprYY[spr_kind]);
							_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind])-(special*10), 6);
							SetRect(&BackRect,210,1045,240,1065);
							_DrawBmp(&BackRect, tx+15, ty+(40-sprYY[spr_kind])-(special*10), 6);
							show_data=false;
						}
						//�ҵ��
						else if(special<55)
						{
							special-=46;
							_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind]), 6);
							SetRect(&BackRect,40*(special%3),985,40+40*(special%3),1025);
							_DrawBmp(&BackRect, tx+10, ty-10, 6);
						}
						//��������
						else if(special<61)
						{
							special-=55;
							_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind]), 6);
							SetRect(&BackRect,35*special,1045,35+35*special,1105);
							_DrawBmp(&BackRect, tx+12, ty-10, 6);
						}
						//ȸ��
						else if(special<76)
						{
							special-=61;
							_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind]), 6);
							SetRect(&BackRect,60+20*(special%3),1025,80+20*(special%3),1045);
							_DrawBmp(&BackRect, tx+20-special*2, ty+10, 6);
							_DrawBmp(&BackRect, tx+20+special*2, ty+10, 6);
							_DrawBmp(&BackRect, tx+20, ty+10-special*2, 6);
							_DrawBmp(&BackRect, tx+20, ty+10+special*2, 6);
						}
						//�Ѹ�����
						else if(special<79)
						{
							special-=76;
							_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind]), 6);
							SetRect(&BackRect,120+40*special,985,160+40*special,1045);
							_DrawBmp(&BackRect, tx+10, ty-10, 6);
						}
						//���� �̵�
						else if(special<94)
						{
							special-=79;
							_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind])-(special*4), 6);
							show_data=false;
						}
						//�Ʒ��� �̵�
						else if(special<109)
						{
							special-=94;
							_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind])+(special*4), 6);
							show_data=false;
						}
						//�·� �̵�
						else if(special<124)
						{
							special-=109;
							_DrawBmp(&BackRect, tx-(special*4), ty+(55-sprYY[spr_kind]), 6);
							show_data=false;
						}
						//��� �̵�
						else if(special<139)
						{
							special-=124;
							_DrawBmp(&BackRect, tx+(special*4), ty+(55-sprYY[spr_kind]), 6);
							show_data=false;
						}
						//�ƹ��͵� �ƴ�
						else
						{
							_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind]), 6);
						}
					}
					else
						_DrawBmp(&BackRect, tx, ty+(55-sprYY[spr_kind]), 6);
				}
			}
		}

	//ĳ���� ����
	if(show_data)
	{
		for(x=cx; x<cxx; x++)	
			for(y=cy; y<cyy; y++)
			{
				if(map_spr[x][y]!=99)
				{
					int tx=x*60-focusX, ty=y*60-focusY;
					char tmp_text[80];
					sprintf(tmp_text,"%02d",unit[map_spr[x][y]].force);
					PutFontOutline(tx+35,ty+40,WHITE,tmp_text);
	
					//�ߵ�
					if(unit[map_spr[x][y]].poisoned)
					{
						SetRect(&BackRect,0,125,18,145);
						_DrawBmp(&BackRect, tx, ty+35, 9);
					}

					SetRect(&BackRect,18,125,23,130);
					for(int i=0; i<unit[map_spr[x][y]].hp; i++)
						_DrawBmp(&BackRect, tx+50, ty+35-i*5, 9);
				}
			}
	}
}

void InitUnit(int who)	//���ְ� ������ ������ �ʱ�ȭ
{
	//�⺻ ����
	unit[who].life=true;
	unit[who].att=Matt[unit[who].kind];
	unit[who].def=Mdef[unit[who].kind];
	unit[who].mov=Mmov[unit[who].kind];
	unit[who].agl=Magl[unit[who].kind];
	unit[who].hp=8;
	unit[who].turn=false;
	unit[who].poison=unit[who].death=unit[who].laser=unit[who].recovery=unit[who].monster=0;
	unit[who].poisoned=false;

	//�δ뿡 ���� �ɷ�
	if(unit[who].kind==0)	//�氥
	{
		unit[who].poison=10;
	}
	else if(unit[who].kind==4)	//���ߵ��
	{
		unit[who].death=50;
	}
	//���ְ��� ���� �ɷ� ��ȭ
	if(unit[who].general==0)	//����
	{
		unit[who].att++;
		unit[who].def++;
		unit[who].mov++;
		unit[who].agl++;
	}
	else if(unit[who].general==1)	//����
	{
		unit[who].att*=2;
	}
	else if(unit[who].general==2)	//��µ��
	{
		unit[who].poison+=foll[1].lv*10;
	}
	else if(unit[who].general==3)	//100����
	{
		unit[who].laser=foll[2].lv*2;
	}
	else if(unit[who].general==4)	//�չ̸�
	{
		unit[who].death+=foll[3].lv*10;;
	}
	else if(unit[who].general==5)	//������
	{
		unit[who].recovery+=foll[4].lv*2;
	}
	else if(unit[who].general==7)	//�ѿ�
	{
		unit[who].monster=foll[6].lv*20;
	}
	else if(unit[who].general==8)	//����
	{
		unit[who].recovery=8;
	}
	else if(unit[who].general==9)	//����
	{
		unit[who].att+=3;
		unit[who].def++;
	}
	else if(unit[who].general==10)	//Į
	{
		unit[who].mov=0;
	}
	else if(unit[who].general==11)	//���
	{
		unit[who].att=5;
		unit[who].def=5;
		unit[who].mov=5;
		unit[who].agl=5;
	}
}

void Search(int x, int y, int expend);

void Check_move(int tx, int ty, int expend)
{
	if(tx<0 || tx>15 || ty<0 || ty>15)return;

	int texpend=expend+tile_expend[map_tile[tx][ty]];
	if(map_move[tx][ty]>texpend)
	{
		if(map_spr[tx][ty]==99)
		{
			map_move[tx][ty]=texpend;
			Search(tx, ty, texpend);
		}
	}
}

void Search(int x, int y, int expend)
{
	//�¿���� �˻�
	Check_move(x-1,y,expend);
	Check_move(x+1,y,expend);
	Check_move(x,y-1,expend);
	Check_move(x,y+1,expend);
}

void SetMove(int x, int y)
{
	int i, j;

	for(int i=0; i<16; i++)
		for(j=0; j<16; j++)
			map_move[i][j]=99;
	map_move[x][y]=0;

	Search(x, y, 0);
}

void focusing(int tx, int ty)	//���� ��ũ��
{
	//��ǥ ����
	tx=tx*60-290;
	tx=Max(tx,0);
	tx=Min(tx,320);

	ty=ty*60-210;
	ty=Max(ty,0);
	ty=Min(ty,480);

	if(focusX==tx && focusY==ty)return;	//���� �̵� �ʿ����

	int speed;
	int sx=abs(tx-focusX), sy=abs(ty-focusY);
	speed=(int)sqrt((double)Max(sx,sy))*3;

	//�����̵�
	int x=focusX;
	int y=focusY;
	for(int i=0; i<=speed; i++)
	{
		ani(10,3);
		focusX=x+((tx-x)*i/speed);
		focusY=y+((ty-y)*i/speed);
		DrawMap();
		jdd->Render();
	}
}

void focusing2(int tx, int ty)	//���� ��ũ��
{
	//��ǥ ����
	tx=tx-290;
	tx=Max(tx,0);
	tx=Min(tx,320);

	ty=ty-210;
	ty=Max(ty,0);
	ty=Min(ty,480);

	focusX=tx;
	focusY=ty;
}

void Move(int who, int tx, int ty)
{
	if(unit[who].x==tx && unit[who].y==ty)return;
	focusing(unit[who].x, unit[who].y);
	
	SetMove(tx,ty);
	int move=unit[who].mov;
	bool able=true;
	int dir;
	
	while(able)
	{
		int x=unit[who].x, y=unit[who].y;
		int least=map_move[x][y];
		int mx, my;

		for(int j=0; j<4; j++)
		{
			int movx, movy;
			if(j==0){ movx=x; movy=y-1;}
				else if(j==1){ movx=x; movy=y+1; }
				else if(j==2){ movx=x-1; movy=y; }
				else { movx=x+1; movy=y; }
			if(movx<0 || movx>15 || movy<0 || movy>15)continue;

			if(map_move[movx][movy]<least)
			{
				dir=j;
				least=map_move[movx][movy];
				mx=movx;
				my=movy;
			}
		}

		if(least!=map_move[x][y])	//������ �������� ����
		{
			PLAY_SOUND(Smov[unit[who].kind]);
			move-=tile_expend[map_tile[mx][my]];
			if(move>=0)	//�̵����� ����ϴٸ� �̵�
			{
				for(int j=0; j<15; j++)
				{
					if(dir==0)focusing2(x*60,y*60-j*4);
						else if(dir==1)focusing2(x*60,y*60+j*4);
						else if(dir==2)focusing2(x*60-j*4,y*60);
						else focusing2(x*60+j*4,y*60);
					ani(5,3);
					DrawMapS(79+j+dir*15,who);
					jdd->Render();
				}
				map_spr[mx][my]=map_spr[x][y];
				map_spr[x][y]=99;
				unit[who].x=mx;
				unit[who].y=my;
			}
			else able=false;
		}
		else able=false;
	}
}

void set_act(int x, int y, int from, int to, bool human)
{
	int i, j;
	for(int i=0; i<16; i++)
		for(j=0; j<16; j++)
			map_move[i][j]=99;

	for(j=from; j<to+1; j++)
	{
		for(int k=0; k<j; k++)
		{
			int sx, sy;
			for(int l=0; l<4; l++)	//�����¿� �˻�
			{
				if(l==0){sx=x-j+k; sy=y-k;}
					else if(l==1){sx=x+j-k; sy=y+k;}
					else if(l==2){sx=x+k; sy=y-j+k;}
					else {sx=x-k; sy=y+j-k;}

				if(sx<0 || sx>=16 || sy<0 || sy>=16)continue;
				if(j>1 && tile_high[map_tile[x][y]]<tile_high[map_tile[sx][sy]])continue;

				map_move[sx][sy]=j;
			}
		}
	}
}

char scout_enemy(int x, int y, int from, int to, bool human)	//�� ã��
{
	for(int j=from; j<to+1; j++)
	{
		for(int k=0; k<j; k++)
		{
			int sx, sy;
			for(int l=0; l<4; l++)	//�����¿� �˻�
			{
				if(l==0){sx=x-j+k; sy=y-k;}
					else if(l==1){sx=x+j-k; sy=y+k;}
					else if(l==2){sx=x+k; sy=y-j+k;}
					else {sx=x-k; sy=y+j-k;}

				if(sx<0 || sx>=16 || sy<0 || sy>=16)continue;
				if(j>1 && tile_high[map_tile[x][y]]<tile_high[map_tile[sx][sy]])continue;

				if(map_spr[sx][sy]!=99)
				{
					if(human && map_spr[sx][sy]>=8)return map_spr[sx][sy];
					if(!human && map_spr[sx][sy]<8)return map_spr[sx][sy];
				}
			}
		}
	}
	return -1;
}

char scout_army(int x, int y, int from, int to, bool human)	//�Ʊ� ã��
{
	for(int j=from; j<to+1; j++)
	{
		for(int k=0; k<j; k++)
		{
			int sx, sy;
			for(int l=0; l<4; l++)	//�����¿� �˻�
			{
				if(l==0){sx=x-j+k; sy=y-k;}
					else if(l==1){sx=x+j-k; sy=y+k;}
					else if(l==2){sx=x+k; sy=y-j+k;}
					else {sx=x-k; sy=y+j-k;}

				if(sx<0 || sx>=16 || sy<0 || sy>=16)continue;
				if(j>1 && tile_high[map_tile[x][y]]<tile_high[map_tile[sx][sy]])continue;

				if(map_spr[sx][sy]!=99)
				{
					if(!human && map_spr[sx][sy]>=8 && unit[map_spr[sx][sy]].hp<8)return map_spr[sx][sy];
					if(human && map_spr[sx][sy]<8 && unit[map_spr[sx][sy]].hp<8)return map_spr[sx][sy];
				}
			}
		}
	}
	return -1;
}

bool scout_item(int x, int y)	//���� �� ã��
{
	for(int j=1; j<2; j++)
	{
		for(int k=0; k<j; k++)
		{
			int sx, sy;
			for(int l=0; l<4; l++)	//�����¿� �˻�
			{
				if(l==0){sx=x-j+k; sy=y-k;}
					else if(l==1){sx=x+j-k; sy=y+k;}
					else if(l==2){sx=x+k; sy=y-j+k;}
					else {sx=x-k; sy=y+j-k;}

				if(sx<0 || sx>=16 || sy<0 || sy>=16)continue;
				if(j>1 && tile_high[map_tile[x][y]]<tile_high[map_tile[sx][sy]])continue;

				if(unit[map_spr[sx][sy]].kind==5)return true;	//DHB�߰�
			}
		}
	}
	return false;
}

int action(int num, bool right)
{
	int Select=-1;
	int tx, ty;
	int mx=MouseX, my=MouseY;
	int mmx=MouseX, mmy=MouseY;
	if(right)tx=450;
		else tx=10;
	ty=10;

	//��� ����
	int skill[4];
	int cx=0;
	char* act_name[]={"���","����","�Ѹ�����","��������","�Ա�","������","����","����","�ҵ��","ȸ������"};

	for(int i=0; i<4; i++)
	{
		skill[i]=-1;
	}
	
	//������
	if(unit[num].kind==0 || unit[num].kind==2 || unit[num].kind==3 ||
		 unit[num].kind==16 ||  unit[num].kind==17 ||  unit[num].kind==20)
		{
			skill[cx]=1;
			cx++;
		}
	//��Ÿ�
	else if(unit[num].kind==1)
	{
		skill[cx]=2;
		cx++;
	}
	else if(unit[num].kind==18)
	{
		skill[cx]=8;
		cx++;
	}
	else if(unit[num].kind==19)
	{
		skill[cx]=7;
		cx++;
	}
	else if(unit[num].kind==21)
	{
		skill[cx]=9;
		cx++;
	}

	//����
	if(unit[num].monster>0)
	{
		skill[cx]=6;
		cx++;
	}

	//������
	if(unit[num].laser>0)
	{
		skill[cx]=5;
		cx++;
	}

	//���� ����
	if(unit[num].death>0)
	{
		skill[cx]=3;
		cx++;
	}

	//�Ա�
	if(unit[num].hp<8 && scout_item(unit[num].x,unit[num].y))
	{		
		skill[cx]=4;
		cx++;
	}

	//���
	skill[3]=0;

	while(Select<0)
	{
		if(!ProcessMessage())break;

		game_lbutton = LeftDown();
		ani(30,3);
		SetRect(&BackRect, 42, 60, 222, 200);
		_DrawBmp(&BackRect, tx, ty, 9);
		
		//���콺 �̵�
		mx+=MouseX-mmx;
		my+=MouseY-mmy;
		mmx=MouseX;
		mmy=MouseY;

		//�޴�
		if(mx<tx+10)mx=tx+10;
		if(mx>tx+156)mx=tx+156;
		if(my<ty+10)my=ty+10;
		if(my>ty+106)my=ty+106;

		
		for(int i=0; i<4; i++){
			if(skill[i]==-1)continue;
			int tx2=tx+(86-strlen(act_name[skill[i]])*4);
			PutFontOutline(tx2,ty+10+(i*20),WHITE,act_name[skill[i]]);
			if(my>=ty+10+(i*20) && my<ty+30+(i*20))
			{
				_DrawBarAlpha(tx+10,ty+10+(i*20),tx+159,ty+29+(i*20),BLUE);
				if(game_lbutton)Select=i;
			}
		}
		//���콺 ����Ʈ
		SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
		_DrawBmp(&BackRect, mx, my-10, 9);
		jdd->Render();

		if(RightDown())Select=99;
	}

	if(Select==99)return 99;
		else return skill[Select];
}

void Die(int who)
{
	focusing(unit[who].x,unit[who].y);

	PLAY_SOUND(17);
	for(int i=26; i<46; i++)
	{
		DrawMapS(i,who);
		jdd->Render();
	}
	unit[who].life=false;
	map_spr[unit[who].x][unit[who].y]=99;
	
	if(who==0)
	{
		Talk(0,"����... ���⼭...","������... ���...","ũ�ƾƾƾ�!","","");
		jdd->Render();
		Click();
	}
	//�λ�
	else if(who>0 && who<8 && foll[who-1].life && foll[who-1].state==0)
	{
		foll[who-1].state=1;
		foll[who-1].delay=3;
	}
}

void Damage(int who, int damage)
{
	PLAY_SOUND(6);
	for(int f2=0; f2<10; f2++)
	{
		for(int f=3; f<6; f++)
		{
			DrawMapS(f,who);
			jdd->Render();
		}
	}

	damage=Max(damage*2-unit[who].def,0);
	for(int i=0; i<damage; i++)
	{
		unit[who].hp--;
		if(unit[who].hp<=0)
		{
			unit[who].hp=8;
			unit[who].force--;
		}
		DrawMap();
		jdd->Render();

		if(unit[who].force<=0)
		{
			Die(who);				
			break;
		}
	}
}

void Damage2(int who, int damage)
{
	for(int f2=0; f2<10; f2++)
	{
		for(int f=3; f<6; f++)
		{
			DrawMapS(f,who);
			jdd->Render();
		}
	}

	for(int i=0; i<damage; i++)
	{
		unit[who].hp--;
		if(unit[who].hp<=0)
		{
			unit[who].hp=8;
			unit[who].force--;
		}
		DrawMap();
		jdd->Render();

		if(unit[who].force<=0)
		{
			Die(who);				
			break;
		}
	}
}

void Heel(int who, int heel)
{
	PLAY_SOUND(11);
	for(int f=61; f<75; f++)
	{
		DrawMapS(f,who);
		jdd->Render();
	}

	for(int i=0; i<heel; i++)
	{
		if(unit[who].hp>=8)break;
		unit[who].hp++;
		DrawMap();
		jdd->Render();
	}
}

void Sucking(int who, int whom)
{
	Damage(whom,unit[who].monster/2);
	Heel(who,unit[who].monster/5);
}

void Laser(int who)
{
	int dir=rand()%4;
	PLAY_SOUND(10);
	
	int startX=unit[who].x*60+20, startY=unit[who].y*60+20;
	if(dir==0)
	{
		for(int i=startY; i>20; i-=10)
		{
			DrawMap();
			focusing2(startX,i);
			SetRect(&BackRect, 220, 1065+dir*20, 240, 1085+dir*20);
			for(int j=startY; j>i-1; j-=10)
				_DrawBmp(&BackRect, startX-focusX, j-focusY, 6);
			jdd->Render();
		}
		for(int i=unit[who].y-1; i>=0; i--)
		{
			int whom=map_spr[unit[who].x][i];
			if(whom!=99)
			{
				focusing(unit[whom].x,unit[whom].y);
				Damage(whom,unit[who].laser);
			}
		}
	}
	else if(dir==1)
	{
		for(int i=startY; i<940; i+=10)
		{
			DrawMap();
			focusing2(startX,i);
			SetRect(&BackRect, 220, 1065+dir*20, 240, 1085+dir*20);
			for(int j=startY; j<i+1; j+=10)
				_DrawBmp(&BackRect, startX-focusX, j-focusY, 6);
			jdd->Render();
		}
		for(int i=unit[who].y+1; i<16; i++)
		{
			int whom=map_spr[unit[who].x][i];
			if(whom!=99)
			{
				focusing(unit[whom].x,unit[whom].y);
				Damage(whom,unit[who].laser);
			}
		}
	}
	else if(dir==2)
	{
		for(int i=startX; i>20; i-=10)
		{
			DrawMap();
			focusing2(i,startY);
			SetRect(&BackRect, 220, 1065+dir*20, 240, 1085+dir*20);
			for(int j=startX; j>i-1; j-=10)
				_DrawBmp(&BackRect, j-focusX, startY-focusY, 6);
			jdd->Render();
		}
		for(int i=unit[who].x-1; i>=0; i--)
		{
			int whom=map_spr[i][unit[who].y];
			if(whom!=99)
			{
				focusing(unit[whom].x,unit[whom].y);
				Damage(whom,unit[who].laser);
			}
		}
	}
	else if(dir==3)
	{
		for(int i=startX; i<940; i+=10)
		{
			DrawMap();
			focusing2(i,startY);
			SetRect(&BackRect, 220, 1065+dir*20, 240, 1085+dir*20);
			for(int j=startX; j<i+1; j+=10)
				_DrawBmp(&BackRect, j-focusX, startY-focusY, 6);
			jdd->Render();
		}
		for(int i=unit[who].x+1; i<16; i++)
		{
			int whom=map_spr[i][unit[who].y];
			if(whom!=99)
			{
				focusing(unit[whom].x,unit[whom].y);
				Damage(whom,unit[who].laser);
			}
		}
	}
}

void Miss(int who)
{
	for(int f=6; f<26; f++)
	{
		DrawMapS(f,who);
		jdd->Render();
	}
}

void AiRange(int scout, int act_color)
{
	//���� �Ÿ��� ������
	int delay;
	for(delay=0; delay<20; delay++)
	{
		ani(10,3);
		DrawMap();

		int cx=focusX/60, cy=focusY/60;
		int cxx=Min(cx+12,16), cyy=Min(cy+12,16);

		int x, y;
		for(x=cx; x<cxx; x++)	
			for(y=cy; y<cyy; y++)
			{
				if(map_move[x][y]!=99)
				{
					int tx=x*60-focusX, ty=y*60-focusY;
					SetRect(&BackRect, (Frame*60)+(180*act_color), 0, (Frame*60)+(180*act_color)+60, 60);
					_DrawBmp(&BackRect, tx, ty, 1);
				}
			}
		jdd->Render();
	}

	//���� ����� ������
	for(delay=0; delay<20; delay++)
	{
		ani(10,3);
		DrawMap();

		int tx=unit[scout].x*60-focusX, ty=unit[scout].y*60-focusY;
		SetRect(&BackRect, (Frame*60)+(180*act_color), 0, (Frame*60)+(180*act_color)+60, 60);
		_DrawBmp(&BackRect, tx, ty, 1);

		jdd->Render();
	}
}

int Battle()
{
	char boss=-1;	//���� ����
	int waste_turn=1;	//�ҿ���
	int ox, oy;
	int win_lose=0, do_what=0, target, act_color, act_kind;	//���� ���, �ൿ, �ൿ���, �ൿ ��, �ൿ ����
	int x,y;
	focusX=focusY=0;
	int game_turn=0;
	char* mapfile[]={"map//stage1.map","map//stage2.map","map//stage3.map","map//stage4.map",
					"map//stage5.map","map//stage6.map","map//stage7.map","map//stage8.map"};
	//�� �б�
	FILE *Fp;
	if(!(Fp=fopen(mapfile[devil.stage],"r")))return false;
	for(x=0; x<16; x++)	
		for(y=0; y<16; y++)
		{
			fscanf(Fp,"%c",&map_tile[y][x]);
			if(map_tile[y][x]<0x30)map_tile[y][x]-=0x20;
				else map_tile[y][x]-=0x24;
		}
	int cx=8;
	for(x=0; x<16; x++)	
		for(y=0; y<16; y++)
		{
			fscanf(Fp,"%c",&map_spr[y][x]);
			if(map_spr[y][x]<0x40)map_spr[y][x]=99;
				else if(map_spr[y][x]<0x50)	//�Ʊ� �δ�
				{
					map_spr[y][x]-=0x40;
					if(unit[map_spr[y][x]].force>0)
					{
						unit[map_spr[y][x]].life=true;
						unit[map_spr[y][x]].x=y;
						unit[map_spr[y][x]].y=x;
						InitUnit(map_spr[y][x]);
					}
					else
					{
						unit[map_spr[y][x]].life=false;
						map_spr[y][x]=99;
					}
				}
				else	//���� �δ�
				{
					unit[cx].kind=map_spr[y][x]-0x40;
					map_spr[y][x]=cx;
					unit[cx].x=y;
					unit[cx].y=x;
					cx++;
				}
		}
	//���� �����
	char background, bgm;
	fscanf(Fp,"%c",&background);
	fscanf(Fp,"%c",&bgm);

	//���δ� �ʱ�ȭ
	for(int i=8; i<SMAX; i++)
	{
		unit[i].life=false;
		unit[i].general=-1;
	}

	//���� ���ְ�
	char general;
	fscanf(Fp,"%c",&general);
	if(general>=0)
	{
		char where;
		fscanf(Fp,"%c",&where);
		boss=where;
		unit[where+8].general=(int)general;
	}

	//���� ����
	for(int i=8; i<cx; i++)
	{
		fscanf(Fp,"%c",&unit[i].force);
		InitUnit(i);
	}

	fclose(Fp);

	//�׷��� �غ�
	LoadPic(0,BattleBack[background],false);
	LoadPic(6,"DATA//unit.png");
	LoadPic(7,"DATA//land.png");

	//���� ���丮
	if(devil.stage==0) //� ���
	{
		_MidiPlay("music//battle1.mid",true);
		focusing(unit[0].x,unit[0].y);
		DrawMap();
		Talk(0,"���Ⱑ �����̱�.","��Ű�� ����� ���� ����.","","","");
		BATTLE_STORY
		Talk(1,"���մ��� ��Ȱ�� ����","�𸣴� �� ������.","���� ������ �� �ְڽ��ϴ�.","","");
		BATTLE_STORY
		Talk(2,"���մ�, �̰����� ��Ͽ� ���ϸ�","�̰��� ��뺴�Ⱑ �ִٰ� �մϴ�.","�Ƹ� �� ���� �����ϸ�","�츮�� �� �� �ְ���.","");
		BATTLE_STORY
		Talk(0,"��, ����!","������ ���� �ϳ� ���������.","������ SPRG��İ� ���� �Ȱ���.","�� �������� �׼��� ����.","");
		BATTLE_STORY
		Talk(0,"���������� ����ϴ� ��","����Ű�� ���콺 ���� ��ư.","����Ű�� �̵�,","���콺 ���� ��ư�� ��������.","");
		BATTLE_STORY
		Talk(0,"�׼��̶�� �ص� �ܼ�����.","�ֳĸ� �̰� �����׼ǰ����� �ƴϰ�","�����ù��̴ϱ�.","��Ʈ�� ���� �ɷ�ġ�� ���̿�","�δ��� ���� �߿�����.");
		BATTLE_STORY
		Talk(0,"�׸��� ������ �Ұ� ĳ���ʹ�","������ �� �ϴϱ�","���� �뾲�� �����...","������ �׳� �´°ž�.","");
		BATTLE_STORY
		Talk(0,"���� �˰���?","����! �ܼ��� ��������!","","","");
		BATTLE_STORY
	}
	else if(devil.stage==1) //Į�� ���
	{
		_MidiPlay("music//battle1.mid",true);
		focusing(unit[0].x,unit[0].y);
		DrawMap();
		Talk(0,"���� ���� �ΰ� �༮���� �츮��","�������� ��ġ �� æ �� ����.","","","");
		BATTLE_STORY
		Talk(1,"���մ��� ��Ȱ�� ��������","���� �̷������ �Դϴ�.","�̴�ζ�� ȥ���������","�׳� �� �� �ְڽ��ϴ�.","");
		BATTLE_STORY
		Talk(8,"�����! ����� ���!","�� ���ϰ� ���� ��ٸ��� �־���.","","","");
		BATTLE_STORY
		Talk(0,"��� �� ����?","�̷��� ���� �ҽ��� ���� ����?","","","");
		BATTLE_STORY
		Talk(8,"������ �� ������ ���, �峻�̴Բ���","������ ���� Ǯ�� ��츦 �����","�� ���⿡ ������ �ξ���.","ȥ������� �� ����.","���⼭ ����� ����.");
		BATTLE_STORY
	}
	else if(devil.stage==2) //������ ��
	{
		_MidiPlay("music//story05.mid",true);
		focusing(unit[0].x,unit[0].y);
		DrawMap();
		Talk(0,"8�� ���̱�.","ȥ������� ���","���� ����� ���� ����.","","");
		BATTLE_STORY
		Talk(1,"�̻��ϱ� �̻��մϴ�.","��� ����� �׷��� �ĵ�","ȥ������� �̷��� ���簡 ���ٴ�.","","");
		BATTLE_STORY
		Talk(2,"Ȥ�� �ٸ� ������ ���� �� �ƴұ��?","���� �����ڸ� ���� ���ڽ��ϴ�.","","","");
		BATTLE_STORY
		Talk(0,"��·�� ������ ����.","������ ����?","","","");
		BATTLE_STORY
	}
	else if(devil.stage==3) //���ռ�
	{
		_MidiPlay("music//story04.mid",true);
		focusing(unit[11].x,unit[11].y);
		DrawMap();
		Talk(9,"����, ���ռ��� �� ���̱�.","��? ��� �Ȱ���?","�̾˵��� ���� �ִµ�...","","");
		BATTLE_STORY
		Talk(0,"��û�� ��.","�츮 ������ ���������� �� �� ����.","å�� ���� �� �о��� ����.","��ݶ��� ����ų ���� �׷��µ�...","");
		BATTLE_STORY
		Talk(9,"å�̶�. �׷���.","��Ÿ���� �� �Ⱦ...","����� ����������","�������� �糪�̰� �ƴ���!","��, ����!");
		BATTLE_STORY
	}
	else if(devil.stage==4) //�縷
	{
		_MidiPlay("music//battle2.mid",true);
		focusing(unit[0].x,unit[0].y);
		DrawMap();
		Talk(0,"����...","���� ����...","","","");
		BATTLE_STORY
		Talk(2,"���⼱ �縷 ����� �̾� ����","�̾˵��� ü���� ���̰� �˴ϴ�.","","","");
		BATTLE_STORY
		Talk(0,"�׷��� ���ۿ� ���߾����!","�� �Ҷ� �׾�� �ؾ���!","","","");
		BATTLE_STORY
		Talk(2,"�Դٰ� ���� ���� �� �ϱ���.","(�� ������ ȭ�� ��! ����~ )","","","");
		BATTLE_STORY
	}
	else if(devil.stage==5) //����� ��
	{
		_MidiPlay("music//story03.mid",true);
		focusing(unit[0].x,unit[0].y);
		DrawMap();
		Talk(0,"��? �̻��ϴ�.","���� ����� �ٲ� �� ������...","�ű�� ���ڱ� ���� ��Ÿ�� �� �ϱ�.","","");
		BATTLE_STORY
		Talk(10,"������, �ڱ��� ���䵵 �𸣳�?","���� ����� ��. ����� ��� �ٲ���.","�ű�� �츮�� �߼�������","���� ġ�� �ִ�!","");
		BATTLE_STORY
		Talk(0,"�׷���. �׷�����.","���� ���ѵ��� ��Ծ���.","�׷��� �� ������!","","");
		BATTLE_STORY
		Talk(10,"����, �� ��ݶ��� ���� ����,","Į�� �ļ��� Į�̴�.","�ڶ����� �̸��� �����޾���!","","");
		BATTLE_STORY
		Talk(0,"�� ���� ���� �ļ��ΰ�?","���ݸ� ��ٷ���!","�̱� �� �ݹ� ������!","","");
		BATTLE_STORY
	}
	else if(devil.stage==6) //�ع� ���
	{
		_MidiPlay("music//story02.mid",true);
		focusing(unit[9].x,unit[9].y);
		DrawMap();
		Talk(0,"���⿡ �ֱ�!","������ �����ΰ�?","�ű⼭ ����ٰ�","���� �� �� �Ƴ�?","");
		BATTLE_STORY
		Talk(10,"����� ���� �����ؾ� �Ұ�.","���� ���� ��踦 �ĳ���.","������� ���� �� �׾��!","","");
		BATTLE_STORY
	}
	else if(devil.stage==7) //��
	{
		_MidiPlay("music//battle3.mid",true);
		focusing(unit[0].x,unit[0].y);
		DrawMap();
		Talk(0,"���� �������̴�!","��! ��������!","�ΰ��� �뿹�� ������!","","");
		BATTLE_STORY
		Talk(2,"�����ϴ�. ���մ�!","�뿹�� ����ô�!","(�뿹�� �ʹ� ���ѵ�...","�ΰ��� �ҽ���...)","");
		BATTLE_STORY
		focusing(unit[10].x,unit[10].y);
		Talk(11,"�׷��� ���� ������!","���� ������ ���ָ�!","","","");
		BATTLE_STORY
		Talk(0,"�� ����...","�� �����ߴ� �峻���� �����ΰ�?","�� ��������!","","");
		BATTLE_STORY
	}

	while(!win_lose){
		if(!ProcessMessage())break;
		{
			ani(5,3);
			DrawMap();

			if(game_turn==0)	//�÷��̾� ���� �˸�
			{
				//���� ���߱�
				focusing(unit[0].x, unit[0].y);

				//������ ����
				for(int i=0; i<640; i+=10)
				{
					_DrawBarAlpha(i,230,i+10,250,BLUE);
					jdd->Render();
				}
				for(int i=0; i<256; i+=8)
				{
					PutFontOutline(280,232,JColor(i,i,i),"������ ����");
					jdd->Render();
				}
				game_turn=1;
			}
			else if(game_turn==1)	//�÷��̾��� ��
			{
				if(_GetKeyState(VK_ESCAPE))
				{
					if(YesNo(0)==1)win_lose=-2;
				}
				//ġƮŰ
				else if(_GetKeyState('D') && _GetKeyState('H') && _GetKeyState('B'))
				{
					win_lose=1;
				}
				else if(_GetKeyState('K') && _GetKeyState('I') && _GetKeyState('N'))
				{
					for(int z=0; z<8; z++)
					{
						if(unit[z].life)unit[z].force=99;
					}
				}

				int mx=(MouseX+focusX)/60, my=(MouseY+focusY)/60;
				//���� ��ư
				if(LeftDown())
				{
					if(do_what==0 && map_spr[mx][my]<8 && !unit[map_spr[mx][my]].turn)
					{						
						target=map_spr[mx][my];
						ox=unit[target].x; oy=unit[target].y;
						SetMove(unit[target].x, unit[target].y);
						do_what=1;						
					}
					else if(do_what==1)
					{
						//�̵�
						if(map_move[mx][my]<=unit[target].mov && (map_spr[mx][my]==99 || map_spr[mx][my]==target))
						{
							Move(target,mx,my);
							do_what=3;
						}
					}
					else if(do_what==2)
					{
						if(map_move[mx][my]!=99 && map_spr[mx][my]!=99)
						{
							if(act_kind==4)
							{
								 if(unit[map_spr[mx][my]].kind==5)
								 {
									Damage(map_spr[mx][my],1);
									Heel(target,8);
									
									unit[target].turn=true;
									do_what=0;
								 }
							}
							else if(map_spr[mx][my]>=8)
							{
								int n_enemy=map_spr[mx][my];
								
								//������
								if(act_kind==1)
								{
									VS(true,target,n_enemy);
									if(unit[target].force<=0)Die(target);
									if(unit[n_enemy].force<=0)Die(n_enemy);
								}
								//�Ѹ� ����
								else if(act_kind==2)
								{
									PLAY_SOUND(12);
									for(float f=76; f<79; f+=0.2)
									{
										DrawMapS((int)f,n_enemy);
										jdd->Render();
									}

									Damage(n_enemy,unit[target].att);
								}
								//���� ����
								else if(act_kind==3)
								{
									PLAY_SOUND(13);
									for(float f=55; f<61; f+=0.2)
									{
										DrawMapS((int)f,n_enemy);
										jdd->Render();
									}

									if((rand()%100)<unit[target].death)
									{
										Damage(n_enemy,0);
										unit[n_enemy].force--;
										unit[n_enemy].hp=8;
										if(unit[n_enemy].force<=0)Die(n_enemy);
									}
									else
									{
										Miss(n_enemy);
									}
								}
								//����
								else if(act_kind==6)
								{
									Sucking(target,n_enemy);
								}
								unit[target].turn=true;
								do_what=0;
							}
						}
					}
				}

				//������ ��ư
				
				if(RightDown())
				{
					if(do_what==0)
					{
						if(YesNo(2)==1)
						{
							for(int i=0; i<8; i++)
								unit[i].turn=true;
						}
					}
					else if(do_what==1)
					{
						do_what=0;
					}
					else if(do_what==2)	//���ݹ����� �ƹ��� ���� ��
					{
						do_what=3;
					}
				}


				//�̵� ����
				if(do_what==1)
				{
					int cx=focusX/60, cy=focusY/60;
					int cxx=Min(cx+12,16), cyy=Min(cy+12,16);
					//�̵� ������
					for(x=cx; x<cxx; x++)	
						for(y=cy; y<cyy; y++)
						{
							if(map_move[x][y]<=unit[target].mov)
							{
								int tx=x*60-focusX, ty=y*60-focusY;
								SetRect(&BackRect, Frame*60, 0, Frame*60+60, 60);
								_DrawBmp(&BackRect, tx, ty, 1);
							}
						}
					if(RightDown())do_what=0;
				}
				else if(do_what==2)
				{
					int cx=focusX/60, cy=focusY/60;
					int cxx=Min(cx+12,16), cyy=Min(cy+12,16);
					//���� ������
					for(x=cx; x<cxx; x++)	
						for(y=cy; y<cyy; y++)
						{
							if(map_move[x][y]!=99)
							{
								int tx=x*60-focusX, ty=y*60-focusY;
								SetRect(&BackRect, (Frame*60)+(180*act_color), 0, (Frame*60)+(180*act_color)+60, 60);
								_DrawBmp(&BackRect, tx, ty, 1);
							}
						}
				}
				else if(do_what==3)
				{
					bool right;
					if(MouseX>=320)right=false;
						else right=true;
					DrawMap();
					int what=action(target,right);
					act_kind=what;

					int cmx=unit[target].x;
					int cmy=unit[target].y;

					if(what==0)	//���
					{
						unit[target].turn=true;
						do_what=0;
					}
					else if(what==1)	//����
					{
						set_act(cmx,cmy,1,1,true);
						do_what=2;
						act_color=1;
					}
					else if(what==2)	//�Ѹ�����
					{
						set_act(cmx,cmy,2,3,true);
						do_what=2;
						act_color=1;
					}
					else if(what==3)	//��������
					{
						set_act(cmx,cmy,1,2,true);
						do_what=2;
						act_color=3;
					}
					else if(what==4)	//�Ա�
					{
						set_act(cmx,cmy,1,1,true);
						do_what=2;
						act_color=2;
					}
					else if(what==5)	//������
					{
						Laser(target);
						unit[target].turn=true;
						do_what=0;
					}
					else if(what==6)	//����
					{
						set_act(cmx,cmy,1,1,true);
						do_what=2;
						act_color=3;
					}
					else if(what==99)
					{
						map_spr[unit[target].x][unit[target].y]=99;
						unit[target].x=ox;
						unit[target].y=oy;
						map_spr[ox][oy]=target;

						do_what=0;
					}
				}

				//�δ뼳��
				if(map_spr[mx][my]!=99)
				{
					int hx, hy;
					if(MouseY<240){hx=395; hy=330;}
						else{hx=10; hy=10;}

					SetRect(&BackRect, 42, 60, 208, 200);
					_DrawBmp(&BackRect, hx, hy, 9);
					SetRect(&BackRect, 152, 60, 222, 200);
					_DrawBmp(&BackRect, hx+166, hy, 9);
					

					//���ְ� ��
					if(unit[map_spr[mx][my]].general>=0)
					{
						char tmp_text[80];
						sprintf(tmp_text,"���ְ�: %s",cGeneral[unit[map_spr[mx][my]].general]);
						PutFontOutline(hx+10,hy+30,YELLOW,tmp_text);
						SetRect(&BackRect, unit[map_spr[mx][my]].general*70, 0, unit[map_spr[mx][my]].general*70+70, 90);
						_DrawBmp(&BackRect, hx+156, hy+40, 4);
					}

					//�ɷ�ġ
					PutFontOutline(hx+10,hy+10,WHITE,cName[unit[map_spr[mx][my]].kind]);
					PutFontOutline(hx+10,hy+50,WHITE,"����");
					PutFontOutline(hx+10,hy+70,WHITE,"���");
					PutFontOutline(hx+10,hy+90,WHITE,"�̵�");
					PutFontOutline(hx+10,hy+110,WHITE,"��ø");
					SetRect(&BackRect, 0, 125, 18, 145);

					for(int i=0; i<4; i++)
					{
						int ability, gap;
						if(i==0)ability=unit[map_spr[mx][my]].att;
							else if(i==1)ability=unit[map_spr[mx][my]].def;
							else if(i==2)ability=unit[map_spr[mx][my]].mov;
							else ability=unit[map_spr[mx][my]].agl;

						if(ability<=5)gap=20;
							else gap=100/ability;
						for(int j=0; j<ability; j++)
							_DrawBmp(&BackRect, hx+45+j*gap, hy+50+i*20, 9);
					}
				}

				//���콺 ����Ʈ
				SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
				_DrawBmp(&BackRect, MouseX-7, MouseY-20, 9);

				//��ũ��
				if(MouseX==639)focusX=Min(focusX+5,320);
				if(MouseX==0)focusX=Max(focusX-5,0);
				if(MouseY==479)focusY=Min(focusY+5,480);
				if(MouseY==0)focusY=Max(focusY-5,0);

				//ó�� ����
				bool end_turn=true;
				for(int i=0; i<8; i++)
					if(unit[i].life && !unit[i].turn)end_turn=false;
				if(end_turn)
				{
					for(int i=0; i<8; i++)unit[i].turn=false;
					game_turn=2;
				}
			}
			/////////////////////////////
			//���� ��
			else if(game_turn==2)
			{
				//���� ����
				for(int i=0; i<640; i+=10)
				{
					_DrawBarAlpha(i,230,i+10,250,BLUE);
					jdd->Render();
				}
				for(int i=0; i<256; i+=8)
				{
					PutFontOutline(290,232,JColor(i,i,i),"���� ����");
					jdd->Render();
				}

				for(int i=8; i<SMAX; i++)
				{
					if(gameover)break;

					if(!unit[0].life)	//���� ����
					{
						win_lose=-1;
						break;
					}
					if(!unit[i].life)continue;
					int tx=unit[i].x, ty=unit[i].y;
					int enemy_act=0;
					focusing(unit[i].x, unit[i].y);

					//�̵�
					if(unit[i].mov>0)
					{
						//��� ã��
						int target_army=-1, def_army=99;
						SetMove(unit[i].x,unit[i].y);
						int from, to;
						if(unit[i].kind!=21){from=0; to=8;}
							else {from=8; to=SMAX;}
						for(int j=from; j<to; j++)
						{
							if(unit[j].life && j!=i)
							{
								int x=unit[j].x, y=unit[j].y;
								int mx, my;
								int least=map_move[x][y];

								int movx, movy;
								int from2=1, to2=1;
								if(unit[i].kind==18)to2=2;	//������
									else if(unit[i].kind==21)to2=3;	//�·�

								for(int j2=from2; j2<to2+1; j2++)
								{
									for(int k=0; k<j2; k++)
									{
										for(int l=0; l<4; l++)	//�����¿� �˻�
										{
											if(l==0){movx=x-j2+k; movy=y-k;}
												else if(l==1){movx=x+j2-k; movy=y+k;}
												else if(l==2){movx=x+k; movy=y-j2+k;}
												else {movx=x-k; movy=y+j2-k;}

											if(movx<0 || movx>=16 || movy<0 || movy>=16)continue;

											if(map_move[movx][movy]<least)
											{
												least=map_move[movx][movy];
												mx=movx;
												my=movy;
											}
										}
									}

									if(least<99 && map_move[mx][my]<def_army)
									{
										target_army=j2;
										def_army=map_move[mx][my];
										tx=mx; ty=my;
									}
								}
							}
						}
						//���� ����� ������ ����
						if(target_army>=0)Move(i, tx, ty);
					}

					//���� ����
					//enemy_act: 0=��� 1=������ 2=��Ÿ� ���� 3=ġ�� 4=�ҵ��
					//����, ���, ���庸��
					if(unit[i].kind==16 || unit[i].kind==17 || unit[i].kind==20)
					{
						set_act(unit[i].x,unit[i].y,1,1,false);
						enemy_act=1;
						act_color=1;
					}
					//�߼���
					else if(unit[i].kind==19)
					{
						set_act(unit[i].x,unit[i].y,2,4,false);
						enemy_act=2;
						act_color=1;
					}
					//������
					else if(unit[i].kind==18)
					{
						set_act(unit[i].x,unit[i].y,1,2,false);
						enemy_act=4;
						act_color=1;
					}
					//�·�
					else if(unit[i].kind==21)
					{
						set_act(unit[i].x,unit[i].y,1,3,true);
						enemy_act=3;
						act_color=2;
					}

					//����
					if(enemy_act==1)
					{
						int scout=scout_enemy(unit[i].x, unit[i].y, 1, 1, false);

						if(scout>=0)
						{
							AiRange(scout,1);
							VS(false,scout,i);
							if(unit[i].force<=0)Die(i);
							if(unit[scout].force<=0)Die(scout);
						}
					}
					//����
					else if(enemy_act==2)
					{
						int scout=scout_enemy(unit[i].x, unit[i].y, 2, 4, false);
						if(scout>=0)
						{
							AiRange(scout,1);

							PLAY_SOUND(1);
							for(int f=0; f<3; f++)
							{
								DrawMapS(f,i);
								jdd->Render();
							}
							//10-����� ��ø�� Ȯ���� ����
							if((rand()%10)<(10-unit[scout].agl))
							{
								Damage(scout,unit[i].att);
							}
							else	//�Ǽ�
							{
								Miss(scout);
							}
						}
					}
					//ġ��
					else if(enemy_act==3)
					{
						int scout=scout_army(unit[i].x, unit[i].y, 1, 3, false);
						if(scout>=0)
						{
							AiRange(scout,1);
							Heel(scout,8);
						}
					}
					//�ҵ��
					else if(enemy_act==4)
					{
						int scout=scout_enemy(unit[i].x, unit[i].y, 1, 2, false);
						if(scout>=0)
						{
							AiRange(scout,1);

							PLAY_SOUND(16);
							for(int f=46; f<55; f++)
							{
								DrawMapS(f,scout);
								jdd->Render();
							}
							
							Damage(scout,unit[i].att);
						}
					}

					unit[i].turn=true;
				}
				for(int i=8; i<SMAX; i++)unit[i].turn=false;

				///////////////////////////////////////////
				//�� �� �� ó�� ����
				waste_turn++;
				if(devil.stage==4)	//�縷����
				{
					for(int i=0; i<SMAX; i++)
						if(unit[i].life && unit[i].kind!=0 && unit[i].kind!=2)
						{
							focusing(unit[i].x,unit[i].y);
							Damage2(i,2);
						}
				}
				else if(devil.stage==5)	//����� ��
				{
					for(x=1; x<15; x++)	
						for(y=1; y<15; y++)
						{							
							if(map_tile[x][y]==10)
							{
								int tree_x=x, tree_y=7;
								int tree_mov=rand()%4;
								if(tree_mov==0)tree_y--;
									else if(tree_mov==0)tree_y++;
									else if(tree_mov==0)tree_x--;
									else tree_x++;
								
								if(map_tile[tree_x][tree_y]==9 && map_spr[tree_x][tree_y]==99)
								{
									map_tile[x][y]=9;
									map_tile[tree_x][tree_y]=10;
								}
							}
						}
				}
				else if(devil.stage==6 && win_lose==0)	//Į�� ����
				{
					int for_who;
					do{
						for_who=rand()%8;
					}while(!unit[for_who].life);

					focusing(unit[for_who].x,unit[for_who].y);
					PLAY_SOUND(16);
					for(int f=46; f<55; f++)
					{
						DrawMapS(f,for_who);
						jdd->Render();
					}

					Damage(for_who,4);

					if(waste_turn==5)	//����
					{
						map_tile[9][15]=9;
						for(int i=11; i<15; i++)
						{
							if(map_spr[i+1][15]==99)
							{
								map_spr[i+1][15]=i;
								unit[i].x=i+1;
								unit[i].y=15;
								unit[i].kind=18;
								unit[i].force=10;
								InitUnit(i);
							}
						}
						focusing(unit[11].x,unit[11].y);

						DrawMap();
						Talk(10,"������! ������ �Ա�!","����! ������!","","","");
						BATTLE_STORY
					}
				}

				//ȸ��
				for(int i=0; i<SMAX; i++)
				{
					//�ߵ� ���� ȸ��
					if(unit[i].poisoned)unit[i].poisoned=false;
					//��ü ȸ��
					if(unit[i].recovery>0)unit[i].hp=Min(unit[i].hp+unit[i].recovery,8);
				}

				game_turn=0;
			}

			//�¸� �˻�
			if(!unit[0].life)win_lose=-1;
			if(boss>=0 && !unit[boss+8].life)win_lose=1;	//���ְ� �����߸�
			
			bool complete=true;
			for(int i=8; i<SMAX; i++)
			{
				if(unit[i].life)
				{
					complete=false;
					break;
				}
			}
			if(complete)win_lose=1;

			jdd->Render();
		}
	}

	//���� ���
	if(win_lose==-2)		//����
	{
		to_menu=true;		
	}
	else if(win_lose==-1)	//���� ����
	{
		Change();
		_MidiPlay("music//gameover.mid", true);
		LoadPic(0,"DATA//gameover.jpg",false);
		jdd->DrawPicture(backbuffer,"P0",0,0,NULL);
		jdd->Render();
		Click();

		to_menu=true;
	}
	else if(win_lose==1)	//�¸�
	{
		for(int i=0; i<8; i++)	//�δ� �� ����
		{
			if(unit[i].life)
				devil.monster[unit[i].kind]+=unit[i].force;
		}

		//���� �� �̾߱�
		if(devil.stage==0)
		{
			_Story(7,16,-1,3);
			foll[2].life=true;
		}
		else if(devil.stage==1)
		{
			_Story(17,24,-1,3);
			foll[5].life=true;
		}
		else if(devil.stage==2)
		{
			_Story(25,34,-1,3);
			foll[3].life=true;
			foll[4].life=true;
		}
		else if(devil.stage==3)
		{
			_Story(35,43,-1,3);
			foll[6].life=true;
		}
		else if(devil.stage==4)
		{
			_Story(44,44,-1,2);
		}
		else if(devil.stage==5)
		{
			_Story(45,50,-1,2);
		}
		else if(devil.stage==6)
		{
			_Story(51,61,-1,2);
		}
		else if(devil.stage==7)
		{
			_Story(62,69,-1,2);
		}

		//����
		if(waste_turn<10)devil.score+=10;
			else if(waste_turn<15)devil.score+=5;
			else if(waste_turn<20)devil.score+=1;
			else if(waste_turn<25)devil.score-=5;
			else if(waste_turn<30)devil.score-=10;
			else if(waste_turn<35)devil.score-=15;
			else devil.score-=20;
		devil.stage++;
		if(devil.stage>=8)
		{
			Ending();
			to_menu=true;
		}
		else _MidiPlay("MUSIC//meeting.mid",true);
	}

	return 0;
}

int reshuffle()	//�δ� �� �Լ�
{
	for(int i=0; i<8; i++)
	{
		unit[i].force=0;
		unit[i].kind=0;
		unit[i].able=false;
	}
	bool End=false;

	while(!End)
	{
		if(!ProcessMessage())break;
		{
			game_lbutton = LeftDown(CLICK_DELAY);
			ani(5,3);
			//�����
			for(int i=0; i<8; i++)
			{
				//ĭ
				if(!unit[i].able)SetRect(&BackRect, 700, 0, 780, 30);
					else SetRect(&BackRect, 700, 30, 780, 60);
				_DrawBmp(&BackRect, i*80, 0, 7);
				char tmp_text[80];
				sprintf(tmp_text,"%d�δ�", i+1);
				PutFontOutline(18+i*80,8,WHITE,tmp_text);
				SetRect(&BackRect, 222, 60, 302, 160);
				_DrawBmp(&BackRect, i*80, 30, 9);
				//��
				if(i==0 || foll[i-1].life && foll[i-1].state==0)
				{
					SetRect(&BackRect, i*70, 0, i*70+70, 90);
					_DrawBmp(&BackRect, i*80+5, 35, 4);
				}
			}
			SetRect(&BackRect, 240, 0, 302, 60);
			for(int i=0; i<11; i++)
				for(int j=0; j<6; j++)
					_DrawBmp(&BackRect, i*62, 130+j*60, 9);

			for(int i=0; i<8; i++)
			{
				SetRect(&BackRect, unit[i].kind*70, 0, 70+unit[i].kind*70, 100);
				_DrawBmp(&BackRect, 5+i*80, 130, 3);
				char tmp_text[80];
				sprintf(tmp_text,"%02d",unit[i].force);
				PutFontOutline(35+i*80,253,WHITE,tmp_text);
			}

			//����
			SetRect(&BackRect, 0, 0, 420, 100);
			_DrawBmp(&BackRect, 0, 370, 3);
			int all_monster=0;
			for(int i=0; i<6; i++)
			{
				char tmp_text[80];
				sprintf(tmp_text,"%02d",devil.monster[i]);
				PutFontOutline(i*70,450,WHITE,tmp_text);
				all_monster+=devil.monster[i];
			}

			//->
			for(int i=0; i<8; i++)
			{
				int ButtonX=55+i*80;
				int ButtonY=130;
				SetRect(&BackRect, 870, 0, 900, 30);
				if(MouseX>=ButtonX && MouseX<ButtonX+30 && MouseY>=ButtonY && MouseY<ButtonY+30)
				{
					if(game_lbutton && all_monster>0)
					{
						do
						{
							devil.monster[unit[i].kind]+=unit[i].force;
							unit[i].force=0;
							unit[i].able=false;

							unit[i].kind++;
							if(unit[i].kind>5)unit[i].kind=0;

						}while(devil.monster[unit[i].kind]<=0);
						SetRect(&BackRect, 870, 30, 900, 60);
					}
				}
				_DrawBmp(&BackRect, ButtonX, ButtonY, 7);
			}

			//<-
			for(int i=0; i<8; i++)
			{
				int ButtonX=5+i*80;
				int ButtonY=130;
				SetRect(&BackRect, 840, 0, 870, 30);
				if(MouseX>=ButtonX && MouseX<ButtonX+30 && MouseY>=ButtonY && MouseY<ButtonY+30)
				{
					if(game_lbutton && all_monster>0)
					{
						do
						{
							devil.monster[unit[i].kind]+=unit[i].force;
							unit[i].force=0;
							unit[i].able=false;

							unit[i].kind--;
							if(unit[i].kind<0)unit[i].kind=5;

						}while(devil.monster[unit[i].kind]<=0);
						SetRect(&BackRect, 840, 30, 870, 60);
					}
				}
				_DrawBmp(&BackRect, ButtonX, ButtonY, 7);
			}

			//+
			for(int i=0; i<8; i++)
			{
				int ButtonX=55+i*80;
				int ButtonY=230;
				SetRect(&BackRect, 780, 0, 810, 30);
				if(MouseX>=ButtonX && MouseX<ButtonX+30 && MouseY>=ButtonY && MouseY<ButtonY+30)
				{
					if(game_lbutton && devil.monster[unit[i].kind]>0)
					{
						devil.monster[unit[i].kind]--;
						unit[i].force++;
						SetRect(&BackRect, 780, 30, 810, 60);
						unit[i].able=true;
					}
				}
				_DrawBmp(&BackRect, ButtonX, ButtonY, 7);
			}
			//99
			for(int i=0; i<8; i++)
			{
				int ButtonX=55+i*80;
				int ButtonY=260;
				SetRect(&BackRect, 930, 0, 960, 30);
				if(MouseX>=ButtonX && MouseX<ButtonX+30 && MouseY>=ButtonY && MouseY<ButtonY+30)
				{
					if(game_lbutton && devil.monster[unit[i].kind]>0)
					{
						unit[i].force+=devil.monster[unit[i].kind];
						devil.monster[unit[i].kind]=0;						
						SetRect(&BackRect, 930, 30, 960, 60);
						unit[i].able=true;
					}
				}
				_DrawBmp(&BackRect, ButtonX, ButtonY, 7);
			}

			//-
			for(int i=0; i<8; i++)
			{
				int ButtonX=5+i*80;
				int ButtonY=230;
				SetRect(&BackRect, 810, 0, 840, 30);
				if(MouseX>=ButtonX && MouseX<ButtonX+30 && MouseY>=ButtonY && MouseY<ButtonY+30)
				{
					if(game_lbutton && unit[i].force>0)
					{
						devil.monster[unit[i].kind]++;
						unit[i].force--;
						SetRect(&BackRect, 810, 30, 840, 60);
						if(unit[i].force==0)unit[i].able=false;
					}
				}
				_DrawBmp(&BackRect, ButtonX, ButtonY, 7);
			}
			//0
			for(int i=0; i<8; i++)
			{
				int ButtonX=5+i*80;
				int ButtonY=260;
				SetRect(&BackRect, 900, 0, 930, 30);
				if(MouseX>=ButtonX && MouseX<ButtonX+30 && MouseY>=ButtonY && MouseY<ButtonY+30)
				{
					if(game_lbutton && unit[i].force>0)
					{
						devil.monster[unit[i].kind]+=unit[i].force;
						unit[i].force=0;						
						SetRect(&BackRect, 900, 30, 930, 60);
						unit[i].able=false;
					}
				}
				_DrawBmp(&BackRect, ButtonX, ButtonY, 7);
			}
			
			//�� ��
			if(unit[0].able)
			{
				if(MouseX>=580 && MouseY>=420 && LButton)
				{
					if(game_lbutton)
					{
						PLAY_SOUND(0);
						End=true;
					}
					SetRect(&BackRect, 180, 0, 240, 60);
				}
				else SetRect(&BackRect, Frame*60, 0, Frame*60+60, 60);
				_DrawBmp(&BackRect, 580, 420, 9);
				PutFontOutline(593,440,WHITE,"����");
			}

			//���콺 ����Ʈ
			SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
			_DrawBmp(&BackRect, MouseX-7, MouseY-20, 9);
			jdd->Render();
		}
	}

	//�δ� ����
	unit[0].general=0;
	for(int i=1; i<8; i++)
	{
		if(foll[i-1].life && foll[i-1].state==0)
		{
			unit[i].general=i;
			unit[i].life=true;
		}
		else
		{
			unit[i].general=-1;
			unit[i].life=false;
		}
	}
	Battle();
	return 0;
}

void turn()
{
	//������ Ȱ��
	if(devil.idea==1)	//¡��
	{
		Change();
		draft();
		Change();
	}
	else if(devil.idea==3)	//�ΰ� ���
	{
		Talk(0,"�׷� �ΰ��� ������ ȥ������ ����?","","","","");
		jdd->Render();
		Click();
		Change();
		devil.fear=Min(devil.fear+kill(),100);
		_MidiPlay("MUSIC//meeting.mid",true);
		LoadPic(0,MapBMP[devil.stage],false);
		Change();
	}
	//���ϵ��� Ȱ��
	int draft_monster[]={3,1,2,4,0,0,5};	//���� ����ϴ� �̾� ����
	int command[]={5,4,3,2,0,10,3};			//��ַ�
	int precise[]={5,10,15,7,3,20,12};		//��Ȯ��
	int deal[]={2,3,3,4,5,3};
	int draft_number;

	for(int i=0; i<7; i++)
	{
		opinion_back();
		if(!foll[i].life || !devil.agree[i] || foll[i].idea==0 || foll[i].idea==4)continue;

		switch(foll[i].idea){
			case 1: //¡��
				draft_number=Min(command[i], devil.mp/deal[draft_monster[i]]);
				if(draft_number>0)
				{
					Talk(i+1,"","","","","");
					char tmp_text[80];
					sprintf(tmp_text,"%s %d������ ��ҽ��ϴ�.",cName[draft_monster[i]], draft_number);
					PutFontOutline(320,290,JColor(128,128,128),tmp_text);
					devil.monster[draft_monster[i]]=Min(devil.monster[draft_monster[i]]+draft_number,99);
				}
				else Talk(i+1,"�ƹ� �͵� �� �߽��ϴ�.","���� ������ �� �� �ž�?","","","");
				break;
			case 2: //����
				if(i==1)Talk(i+1,"�׷� �ٳ���ڽ��ϴ�.","(��Ĳ��~ ��Ĳ��~)","","","");
				if(i==2)Talk(i+1,"200���ΰ� �Ǿ ���ڽ��ϴ�.","","","","");
				if(i==3)Talk(i+1,"�׷� ���� ������ �ٳ���ڽ��ϴ�.","��ȣȣȣȪ!","","","");
				if(i==4)Talk(i+1,"...�׷� �ٳ���ڽ��ϴ�.","","","","");
				if(i==6)Talk(i+1,"��ȿȿ��, �ٳ� ���ڽ��ϴ�.","���� ������ ����� ����.","��ȿȿȿ��.","","");
				foll[i].state=3; foll[i].delay=foll[i].lv+1;
				foll[i].lv++;
				break;
			case 3: //���
				Talk(i+1,"","���� �ΰ����� ������ ������.","","","");
				precise[i]+=rand()%5;
				char tmp_text[80];
				sprintf(tmp_text,"�ΰ� %d���� ���ݽ��ϴ�.", precise[i]);
				PutFontOutline(320,290,JColor(128,128,128),tmp_text);
				devil.fear=Min(devil.fear+precise[i],100);
				break;
		}
		
		jdd->Render();
		Click();
	}

	//���� �ǰ� ����
	int aree_war=0, all_follower=1;
	if(devil.idea==0)aree_war++;
	for(int i=0; i<7; i++)
	{
		if(foll[i].life)all_follower++;
		if(foll[i].idea==0)aree_war++;
	}

	//���� ����
	if(aree_war>0)
	{
		opinion_back();
		if(aree_war>=(all_follower+1)/2)
			Talk(0,"�� �̻��� ����������","���￡ �����ڽ��ϴ�.","�δ� ���� ���ϴ�.","","");
		else Talk(0,"������ �� �̻��� ���� �� �����Ƿ�","�ΰ�Ǿ����ϴ�.","","","");
		jdd->Render();
		Click();

		if(aree_war>=(all_follower+1)/2)
		{
			Change();
			reshuffle();
		}
	}

	if(to_menu)
	{
		to_menu=false;
		_MidiPlay("music//open.mid", true);
		LoadPic(0,"DATA//title.jpg",false);
		Change();mode=0;
		return;
	}

	//�� ���� �� ó�� ����
	for(int i=0; i<7; i++)
	{
		if(foll[i].state!=0)
		{
			foll[i].delay--;
			if(foll[i].delay<=0)foll[i].state=0;
		}
	}

	devil.mp=Min(devil.mp+devil.fear,100);
	devil.fear=Max(devil.fear-10,0);
	devil.turn++;
	devil.score--;
	
	if(!gameover)PreMeeting();
}

int think()
{
	int Select=-1;

	while(Select<0)
	{
		if(!ProcessMessage())break;

		game_lbutton = LeftDown();
		ani(30,3);
		SetRect(&BackRect, 42, 60, 222, 200);
		_DrawBmp(&BackRect, 230, 230, 9);
		//�޴�
		if(MouseX<240)MouseX=240;
		if(MouseX>386)MouseX=386;
		if(MouseY<240)MouseY=240;
		if(MouseY>336)MouseY=336;

		PutFontOutline(300,240,WHITE,"����");
		PutFontOutline(300,260,WHITE,"¡��");
		PutFontOutline(300,280,WHITE,"���");
		PutFontOutline(300,300,WHITE,"�޽�");
		
		for(int i=0; i<4; i++){
			if(MouseY>=240+(i*20) && MouseY<260+(i*20))
			{
				_DrawBarAlpha(240,240+(i*20),399,259+(i*20),BLUE);
				if(game_lbutton)Select=i;
			}
		}
		PutFontOutline(300,340,RED,"���");
		if(MouseY>325)
		{
			_DrawBarAlpha(240,340,399,359,BLUE);
			if(game_lbutton)return 0;
		}
		//���콺 ����Ʈ
		SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
		_DrawBmp(&BackRect, MouseX, MouseY-10, 9);
		jdd->Render();
	}
	
	if(Select>1)devil.idea=Select+1;
		else devil.idea=Select;
	return 0;
}

//���Ӹ�� 2
void _Meeting()
{
	if(tab<0 || tab>2)tab=0;
	bool End=false;
	
	ani(3,3);
	//����
	SetRect(&BackRect, 0, 0, 560, 240);
	_DrawBmp(&BackRect, 40, 40, 0);
	//�׵θ�
	SetRect(&BackRect, 0, 85, 40, 125);
	for(int i=0; i<16; i++)
	{
		_DrawBmp(&BackRect, i*40, 0, 9);
		_DrawBmp(&BackRect, i*40, 280, 9);
	}
	for(int i=0; i<6; i++)
	{
		_DrawBmp(&BackRect, 0, 40+i*40, 9);
		_DrawBmp(&BackRect, 600, 40+i*40, 9);
	}
	//��
	for(int i=0; i<3; i++)
	{
		int j=i*100;
		if(MouseX>=j && MouseX<j+100 && MouseY>=290 && MouseY<330 && game_lbutton)tab=i;

		int yPos=0;
		if(i==tab)yPos=30;
		SetRect(&BackRect, i*100, yPos, i*100+100, 30+yPos);
		_DrawBmp(&BackRect, i*100, 320, 7);
	}
	for(int i=0; i<17; i++)
	{
		SetRect(&BackRect, 0, 0, 10, 30);
		_DrawBmp(&BackRect, i*20+300, 320, 7);
		SetRect(&BackRect, 90, 0, 100, 30);
		_DrawBmp(&BackRect, i*20+310, 320, 7);
	}
	if(tab==0)	//ȸ��
	{
		//�����
		for(int i=0; i<8; i++)
		{
			//ĭ
			SetRect(&BackRect, 222, 60, 302, 160);
			_DrawBmp(&BackRect, i*80, 350, 9);
			//�ǰ�
			if(i>0)
			{
				if(!devil.agree[i-1])SetRect(&BackRect, 300+80*foll[i-1].idea, 0, 380+80*foll[i-1].idea, 30);
						else SetRect(&BackRect, 300+80*foll[i-1].idea, 30, 380+80*foll[i-1].idea, 60);
			}
			else
			{
				if(devil.idea==-1)SetRect(&BackRect, 300, 0, 380, 30);
					else SetRect(&BackRect, 300+80*devil.idea, 30, 380+80*devil.idea, 60);
			}
			_DrawBmp(&BackRect, i*80, 450, 7);
			//��
			if(i==0 || foll[i-1].life)
			{
				SetRect(&BackRect, i*70, 0, i*70+70, 90);
				_DrawBmp(&BackRect, i*80+5, 355, 4);
				if(i>0)
				{
					if(foll[i-1].state==1)PutFontOutline(i*80+24,400,RED,"�λ�");
						else if(foll[i-1].state==3)PutFontOutline(i*80+24,400,YELLOW,"����");
				}
			}
		}

		//�³�, �ź�
		if(game_lbutton && MouseY>=435)
		{
			int aree_with_who;
			for(int i=0; i<8; i++)
			{
				if(MouseX>=i*80 && MouseX<=i*80+80)
				{
					aree_with_who=i-1;
					break;
				}
			}
			if(aree_with_who==-1)	//������ �ǰ�
			{
				think();
			}
			else if(foll[aree_with_who].idea==0);
			else		//���ϵ� �ǰ� �³�/�ź�
			{
				if(!foll[aree_with_who].life || foll[aree_with_who].idea>3);
				else
					devil.agree[aree_with_who]=(devil.agree[aree_with_who])?false:true;
			}
		}

		//ȸ�� ��
		if(devil.idea>=0)
		{
			if(MouseX>=580 && MouseY>=270 && MouseY<340 && LButton)
			{
				if(game_lbutton)
				{
					PLAY_SOUND(0);
					End=true;
				}

				SetRect(&BackRect, 180, 0, 240, 60);
			}
			else SetRect(&BackRect, Frame*60, 0, Frame*60+60, 60);
			_DrawBmp(&BackRect, 580, 280, 9);
			PutFontOutline(593,300,WHITE,"����");
		}

		//����
		if(LButton && MouseY>=350 && MouseY<435)
		{
			int help_for_who;
			for(int i=0; i<8; i++)
			{
				if(MouseX>=i*80 && MouseX<=i*80+80)
				{
					help_for_who=i;
					break;
				}
			}
			if(help_for_who==0 || foll[help_for_who-1].life)
			{
				SetRect(&BackRect, help_for_who*270, 20, help_for_who*270+270, 340);
				_DrawBmp(&BackRect, 0, 0, 8);
				
				if(help_for_who==0){
					PutFontOutline(280,50,WHITE,"�̸�: ����� 3�� / ����: ����");
					PutFontOutline(280,70,WHITE,"������ ��.");
					PutFontOutline(280,150,WHITE,"Ư���ɷ�: �δ� �ɷ�ġ ���+1");
				}
				else if(help_for_who==1){
					PutFontOutline(280,50,WHITE,"�̸�: ���� / ����: �屺");
					PutFontOutline(280,70,WHITE,"���ձ� �ְ��� ����");
					PutFontOutline(280,90,WHITE,"����, �Ǹ��� �߽��Ѵ�.");
					PutFontOutline(280,150,WHITE,"Ư���ɷ�: �δ� ���ݷ� 2��");
				}
				else if(help_for_who==2){
					PutFontOutline(280,50,WHITE,"�̸�: ��µ�� / ����: ������");
					PutFontOutline(280,70,WHITE,"������ ����ϰ� �ߺ��ϴ�.");
					PutFontOutline(280,90,WHITE,"������ ���� ���� ������.");
					char tmp_text[80];
					sprintf(tmp_text,"Ư���ɷ�: %d/10 Ȯ���� �ߵ� �߻�",foll[2].lv);
					PutFontOutline(280,150,WHITE,tmp_text);
				}
				else if(help_for_who==3){
					PutFontOutline(280,50,WHITE,"�̸�: 100���� / ����: ��뺴��");
					PutFontOutline(280,70,WHITE,"������ 50������ �������� �ο�ٰ� �Ѵ�.");
					PutFontOutline(280,90,WHITE,"�װ� 100���θ� ���� ���� �� �����");
					PutFontOutline(280,110,WHITE,"�ƹ��� ����.");
					PutFontOutline(280,150,WHITE,"Ư���ɷ�: ������");
				}
				else if(help_for_who==4){
					PutFontOutline(280,50,WHITE,"�̸�: �չ̸� / ����: �������");
					PutFontOutline(280,70,WHITE,"�ڱⰡ ���ڴٴ� ������ ���� �Ҹ���.");
					PutFontOutline(280,90,WHITE,"������ �����Ƿ¸��� �Ϸ��̴�.");
					PutFontOutline(280,110,WHITE,"�ΰ������� ������ �ְ�, �� ���� ��︰��.");
					char tmp_text[80];
					sprintf(tmp_text,"Ư���ɷ�: �������� ������ %d/10 ����",foll[3].lv);
					PutFontOutline(280,150,WHITE,tmp_text);
				}
				else if(help_for_who==5){
					PutFontOutline(280,50,WHITE,"�̸�: ������ / ����: �������");
					PutFontOutline(280,70,WHITE,"���� ���� ���� �����̴�.");
					PutFontOutline(280,90,WHITE,"������ �ƴ� �ΰ�.");
					PutFontOutline(280,150,WHITE,"Ư���ɷ�: �δ� �ڵ�ȸ��");
				}
				else if(help_for_who==6){
					PutFontOutline(280,50,WHITE,"�̸�: �ڸ� / ����: �������");
					PutFontOutline(280,70,WHITE,"���� �ô��� ���� �ְ��� �̳�");
					PutFontOutline(280,150,WHITE,"Ư���ɷ�: ����");
				}
				else if(help_for_who==7){
					PutFontOutline(280,50,WHITE,"�̸�: �ѿ� / ����: �̾˻�");
					PutFontOutline(280,70,WHITE,"�̻��� ������ �ϰ�, �̻��� �̾��� �����.");
					PutFontOutline(280,90,WHITE,"� �� �Ǹ��� �̾��� �����Ѵ�.");
					PutFontOutline(280,150,WHITE,"Ư���ɷ�: ����");
				}
			}
		}
	}
	else
	{
		//Ÿ��
		SetRect(&BackRect, 240, 0, 302, 60);
		for(int i=0; i<11; i++)
			for(int j=0; j<3; j++)
				_DrawBmp(&BackRect, i*62, j*60+350, 9);
		if(tab==1)	//����
		{
			char tmp_text[80];
			sprintf(tmp_text,"�Ѹ���: %d",devil.mp);
			PutFontOutline(20,360,WHITE,tmp_text);
			sprintf(tmp_text,"�ΰ����� ����: %d",devil.fear);
			PutFontOutline(20,380,WHITE,tmp_text);
			sprintf(tmp_text,"�ҿ� �ϼ�: %d",devil.turn);
			PutFontOutline(20,400,WHITE,tmp_text);
			SetRect(&BackRect, 0, 0, 420, 100);
			_DrawBmp(&BackRect, 220, 370, 3);
			for(int i=0; i<6; i++)
			{
				char tmp_text[80];
				sprintf(tmp_text,"%02d",devil.monster[i]);
				PutFontOutline(240+i*70,450,WHITE,tmp_text);
			}
			
			//ĳ���� ����
			if(LButton && MouseY>=350 && MouseY<470)
			{
				int help_for_who;
				for(int i=0; i<6; i++)
				{
					if(MouseX>=i*70+220 && MouseX<=i*70+290)
					{
						help_for_who=i;
						break;
					}
				}
				if(help_for_who>=0 && help_for_who<=5)
				{
					SetRect(&BackRect, help_for_who*200, 0, help_for_who*200+200, 320);
					_DrawBmp(&BackRect, 0, 0, 6);
					
					if(help_for_who==0){
						PutFontOutline(210,50,WHITE,"�氥(�Ӽ�: �� / ������: �縷)");
						PutFontOutline(210,70,WHITE,"���� ������ ������ ���� ���Ƶ��δ�.");
						PutFontOutline(210,90,WHITE,"������ ���Դ� ���ǵ����� ���´�.");
						PutFontOutline(210,110,WHITE,"���� ���� �Ǿ� �ִµ� �̴� �縷���� ��Ƴ��� ����");
						PutFontOutline(210,130,WHITE,"���� ���� �����ϰ� �ִ� ���̴�.");

						PutFontOutline(400,170,YELLOW,"������ ����");
						PutFontOutline(400,200,YELLOW,"�ߵ�(������: 1/10)");
					}
					else if(help_for_who==1){
						PutFontOutline(210,50,WHITE,"�ĸ�����(�Ӽ�: �Ĺ� / ������: ��)");
						PutFontOutline(210,70,WHITE,"���߸���� �Ӹ��� �� �̾�.");
						PutFontOutline(210,90,WHITE,"�Ӹ��� ������ ���� Ƣ��ٴѴ�.");
						PutFontOutline(210,110,WHITE,"�ǵ帮�� ������ ���������� �ʴ´�.");

						PutFontOutline(400,170,YELLOW,"��Ÿ� ���� ����");
					}
					else if(help_for_who==2){
						PutFontOutline(210,50,WHITE,"ū����(�Ӽ�: �� / ������: �縷)");
						PutFontOutline(210,70,WHITE,"����� ���� ���� �̾�.");
						PutFontOutline(210,90,WHITE,"�Ӹ��� ���� �������� �¸��� ���̴�.");
						PutFontOutline(210,110,WHITE,"�Ӹ��� ũ�� ������ ū������ �̸��� �ٿ�����.");

						PutFontOutline(400,170,YELLOW,"������ ����");
					}
					else if(help_for_who==3){
						PutFontOutline(210,50,WHITE,"������(�Ӽ�: ö, ��� / ������: ����)");
						PutFontOutline(210,70,WHITE,"���սô� ���Ǿ��ٴ� �̾�.");
						PutFontOutline(210,90,WHITE,"�ֱٿ��� ������ �������� ������ ������ ���̶�");
						PutFontOutline(210,110,WHITE,"�����ϴ� ���ڰ� �־� �̾˷� ġ�� �ʱ⵵ �Ѵ�.");
						PutFontOutline(210,130,WHITE,"������ � ���·ε� �����̹Ƿ�");
						PutFontOutline(210,150,WHITE,"�̾˷� ����ϴ� ���� �����̴�.");

						PutFontOutline(400,170,YELLOW,"������ ����");
					}
					else if(help_for_who==4){
						PutFontOutline(210,50,WHITE,"���ߵ��(�Ӽ�: ��� / ������: ����)");
						PutFontOutline(210,70,WHITE,"����� ����� ���� ���ܼ� '���ߵ��'�� �̸��� �پ���.");
						PutFontOutline(210,90,WHITE,"�买(������� �̾�)�� �˷��� �ִ�.");
						PutFontOutline(210,110,WHITE,"������� ���� �ǿ�鼭 ����� ������ ȥ����Ų��.");
						PutFontOutline(210,130,WHITE,"���ںκп� ���� ��ġ�� �� ����� �װԵȴٰ� �Ѵ�.");

						PutFontOutline(400,170,YELLOW,"��������(������: 1/2)");
					}
					else if(help_for_who==5){
						PutFontOutline(210,50,WHITE,"DHB(�Ӽ�: ? / ������: ?)");
						PutFontOutline(210,70,WHITE,"�¸��� ���˷� �ѷ����� ���׶� �̾�.");
						PutFontOutline(210,90,WHITE,"���縸���� �̾�. �� �Ա� �ź�.");
					}
					//�ɷ�ġ
					PutFontOutline(210,170,WHITE,"����");
					PutFontOutline(210,200,WHITE,"���");
					PutFontOutline(210,230,WHITE,"�̵�");
					PutFontOutline(210,260,WHITE,"��ø");
					SetRect(&BackRect, 0, 125, 18, 145);
					for(int i=0; i<5; i++)
					{
						if(Matt[help_for_who]>i)
							_DrawBmp(&BackRect, 250+i*20, 169, 9);
						if(Mdef[help_for_who]>i)
							_DrawBmp(&BackRect, 250+i*20, 199, 9);
						if(Mmov[help_for_who]>i)
							_DrawBmp(&BackRect, 250+i*20, 229, 9);
						if(Magl[help_for_who]>i)
							_DrawBmp(&BackRect, 250+i*20, 259, 9);
					}
				}
			}
		}
		else if(tab==2)	//�ý���
		{
			//�޴�
			int Select=-1;
			for(int i=0; i<3; i++){
				int EyeX[]={30,100,581};
				int EyeY[]={382,376,413};
				char* str[]={"����","�ε�","�ߴ�"};
				if(MouseX>=EyeX[i] && MouseX<EyeX[i]+60 && MouseY>=EyeY[i] && MouseY<EyeY[i]+60 && LButton)
				{
					if(game_lbutton)
					{
						PLAY_SOUND(0);
						Select=i;
					}

					SetRect(&BackRect, 180, 0, 240, 60);
				}
				else SetRect(&BackRect, Frame*60, 0, Frame*60+60, 60);
				_DrawBmp(&BackRect, EyeX[i], EyeY[i], 9);
				PutFontOutline(EyeX[i]+13,EyeY[i]+20,WHITE,str[i]);
			}
			//����
			if(Select==0)View(true);
				else if(Select==1)View(false);
				else if(Select==2)
				{
					if(YesNo(0)==1){
						_MidiPlay("music//open.mid", true);
						LoadPic(0,"DATA//title.jpg",false);
						Change();mode=0;
					}
				}
		}
	}

	//���콺 ����Ʈ
	SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
	_DrawBmp(&BackRect, MouseX, MouseY, 9);
	jdd->Render();

	//�������� ����
	if(End)turn();
}

int main()
{
	if(!MainInitialize("Devil2-1", TRUE, TRUE, window_mode))return 0;

	//������â �̵�
	if(window_mode)
	{
		jdd->OnMove(INIT_WINDOWX, INIT_WINDOWX);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}

	font20=jdd->CreateFont("����ü",14,true,false,false,false,false);

	//�׷��� �ʱ�ȭ
	LoadPic(0,"DATA//title.jpg",false);
	LoadPic(1,"DATA//box.png");
	LoadPic(2,"DATA//dlg.png");
	LoadPic(3,"DATA//monsters.png");
	LoadPic(4,"DATA//meeting_face.png");
	LoadPic(5,"DATA//mask.png");
	LoadPic(8,"DATA//face.png");
	LoadPic(9,"DATA//sprites.png");

	//�����ʱ�ȭ
	_PlayAVI("open.avi");
	_MidiPlay("music//open.mid", true);
	
	if ( SoundOBJ )
	{
		Sound[0] = SndObjCreate(SoundOBJ,"Sound//select.WAV",2);
		Sound[1] = SndObjCreate(SoundOBJ,"Sound//shoong.WAV",2);
		Sound[2] = SndObjCreate(SoundOBJ,"Sound//tack.WAV",2);
		Sound[3] = SndObjCreate(SoundOBJ,"Sound//jump.WAV",2);
		Sound[4] = SndObjCreate(SoundOBJ,"Sound//stone.WAV",2);
		Sound[5] = SndObjCreate(SoundOBJ,"Sound//toca.WAV",2);
		Sound[6] = SndObjCreate(SoundOBJ,"Sound//puck.WAV",2);
		Sound[7] = SndObjCreate(SoundOBJ,"Sound//wind.WAV",2);
		Sound[8] = SndObjCreate(SoundOBJ,"Sound//def.WAV",2);
		Sound[9] = SndObjCreate(SoundOBJ,"Sound//poison.WAV",2);
		Sound[10] = SndObjCreate(SoundOBJ,"Sound//beam.WAV",2);
		Sound[11] = SndObjCreate(SoundOBJ,"Sound//heal.WAV",2);
		Sound[12] = SndObjCreate(SoundOBJ,"Sound//root.WAV",2);
		Sound[13] = SndObjCreate(SoundOBJ,"Sound//die.WAV",2);
		Sound[14] = SndObjCreate(SoundOBJ,"Sound//uk.WAV",2);
		Sound[15] = SndObjCreate(SoundOBJ,"Sound//tang.WAV",2);
		Sound[16] = SndObjCreate(SoundOBJ,"Sound//fire.WAV",2);
		Sound[17] = SndObjCreate(SoundOBJ,"Sound//death.WAV",2);
    }
	InitCara();	//ĳ���� ���� �ʱ�ȭ
	while (!Quit){
		if(!ProcessMessage())break;
		game_lbutton = LeftDown();

		switch(mode)
		{
			case 0: _MainMenu(); break;
			case 2: _Meeting();break;
		}
	}
	Change();
	_MidiStop();

	//���� ��, ����
	jdd->DeleteFont(font20);

	return 0;
}

//������ �޼��� ó��
LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch ( msg )
    {
		case WM_LBUTTONDOWN  :	if(mouse_control)LButton=TRUE;
								SetCapture(hwnd);
								break;

		case WM_RBUTTONDOWN  :	if(mouse_control)RButton=TRUE;
								SetCapture(hwnd);
								break;

		case WM_LBUTTONUP    :	if(mouse_control)LButton=FALSE;
								ReleaseCapture();
								break;

		case WM_RBUTTONUP    :	if(mouse_control)RButton=FALSE;
								ReleaseCapture();
								break;

		case WM_MOUSEMOVE    :	if(mouse_control)
								{
									MouseX=LOWORD(lParam);
									MouseY=HIWORD(lParam);
									mouse_move = TRUE;
									ShowCursor(FALSE);
								}
								break;

		case MM_MCINOTIFY    :	if ( ReplayFlag && wParam == MCI_NOTIFY_SUCCESSFUL )_MidiReplay();
								break;

		case WM_DESTROY 	 :	_CrtDumpMemoryLeaks();
								break;

		case WM_SYSCOMMAND	 :  //�ݱ� �޽��� ����ä��
								if(wParam==SC_CLOSE)
								{
									wParam=0;
									gameover=TRUE;
								}
								break;

		case WM_SIZE		 :	if(wParam == SIZE_MINIMIZED)activate=false;
								else activate=true;
								break;

		case WM_MOVE		 :	if(jdd)jdd->OnMove(LOWORD(lParam), HIWORD(lParam));
								break;
		
		case WM_ACTIVATE	 : if(LOWORD(wParam))activate=true;
								else activate=false;
							   break;
	}

	return DefWindowProc(wnd,msg,wParam,lParam);
}