#include "JDirectDraw.h"
#include "JResourceManager.h"
#include "resource.h"

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <crtdbg.h>
#include <gdiplus.h>

using namespace Gdiplus;

JDirectDraw* jdd;
JResourceManager* jre;

char* backbuffer;
JFont font20, font12;
MSG msg;

LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam);

#include "data.h"

bool window_mode = true;

void FadeOut()
{
	for(int i=0; i<10; i++)
	{
		jdd->DrawPicture(backbuffer,"막",0,0,NULL);
		jdd->Render();
		Sleep(30);
	}
}

/////////////////////////////////
//명령 처리 클래스
class CCommand
{
	int com_id[16];
	int command_max;
	int no;
	int kind;
public:
	char commands[256];

	void AddCom(int id);
	int GetComID(int no);
	bool IsFull();
	void NormalCom(int max, int sw_start, int plus);

	CCommand(int k, int max=16);
};

CCommand::CCommand(int k, int max)	//초기화
{
	no=0;
	kind=k;
	command_max=max;

	strcpy(commands,"");
	for(int i=0; i<5; i++)com_id[i]=-1;
}

void CCommand::AddCom(int id)	//사용할 수 있는 명령 추가
{
	if(no>=command_max)return;	//명령어가 꽉 참

	char* com_base[9]={"대화","공격","유도심문","사람을 관찰","이곳을 조사","귀를 기울인다.","이동","대기","도구 구입"};
	char* com_talk[40]={"정치","경제","사회","문화","외교","뉴스","화제거리","유행","군대","스포츠","건강","의학","컴퓨터","쇼핑","취미","특기","교육","정의","철학","종교","결혼","사랑","이상형","꿈","가족","나이","키","체중","혈액형","별자리","애인","사생활","가치관","추억","은사","첫사랑","원수","나","너","수다"};
	char* com_trial[8]={"누굴 죽일 건가요?","게임이니 누굴 죽여야 겠죠?","M씨!","유도 좋아하시나요?","M은 누굴까요?","역시 오늘이 좋겠죠?","어디 숨겼죠?","배고프다."};
	char* com_observe[32]={"머리","눈","코","입","귀","턱","목","어깨","가슴","배","등","팔","손","다리","발","콧구멍 속","귓구멍 속","입 속","가랑이","손톱","발톱","수염","옷","악세사리","몸 속","가랑이 속","꼬리","더듬이","아가미","날개","비늘","아무데나"};
	char* com_move[13]={"거실","계단","2층 복도","2층 계단","3층","욕실","주방","방1","방2","방3","방4","밖","취소"};
	char* com_invest[29]={"바닥","쇼파","항아리","벽","쇼파 및","조명","창밖","의자","문","계단","천장","난간","벽난로","탁자","욕조","세면대","거울","싱크대","휴지통","시계","침대","베개","침대 및","스탠드","나무","하늘","저택","TV","취소"};
	char* com_item[5]={"반창고(2카오)","안티프라민(5카오)","반창고 5개(9카오)","안티프라민 5개(23카오)","취소"};

	if(no>0)strcat(commands,"\n");
	switch(kind)
	{
		case 0: strcat(commands,com_talk[id]); break;
		case 2: strcat(commands,com_trial[id]); break;
		case 3: strcat(commands,com_observe[id]); break;
		case 4: strcat(commands,com_invest[id]); break;
		case 6: strcat(commands,com_move[id]); break;
		case 8: strcat(commands,com_item[id]); break;
		case 99: strcat(commands,com_base[id]); break;
	}
	com_id[no]=id;
	no++;
}

int CCommand::GetComID(int no)
{
	if(no>=0 && no<command_max)return com_id[no];
		else return -1;
}

bool CCommand::IsFull()
{
	if(no>=command_max)return true;
		else return false;
}

void CCommand::NormalCom(int max, int sw_start, int plus)
{
	//가능한 조건을 검사
	for(int i=0; i<max-1; i++)
	{
		int sw_no=sw_start+i;
		if(!m_sv.sw[sw_no])AddCom(i);
		if(IsFull())break;
	}
	if(!IsFull())AddCom(max-1);

	//실행
	int com=m_dlg.TextSel(commands);
	int com_id=GetComID(com);
	m_sv.sw[sw_start+com_id]=true;
	m_sv.var[47+m_sv.var[26]]+=(com_id/5+1)*plus;	//호감도
	m_dlg.TextSnr(sw_start+48+com_id);
}

void GameOver()
{
	while(1)
	{
		jdd->DrawPicture(backbuffer,"GameOver",0,0,NULL);
		if(_GetKeyState(VK_RETURN))
		{
			if(!enter)break;
		}
		else enter=false;
		jdd->Render();
	}
	FadeOut();
}

void Ending()
{
	//if(!m_nosound)jds->Stop("BGM");
	//if(!m_nosound)jds->Play("Ending");
	int page=0;
	while(page<6)
	{
		jdd->DrawPicture(backbuffer,StrAdd("cd","Ending",page),0,0,NULL);
		jdd->Render();

		Sleep(12000);
		FadeOut();
		page++;
	}
	//if(!m_nosound)jds->Stop("Ending");
}
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■																		■
■                             메   인                                  ■
■																		■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//int main()
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstancem, LPSTR lpCmdLine, int nShowCmd)
{
	jdd=CreateDirectDraw();
	jre=CreateDXResourceManager(jdd);

	WNDCLASS wc={0};
	wc.hIcon=LoadIcon(hInstance,"satam3.ico");
	wc.hCursor=LoadCursor(hInstance,IDC_ARROW);
	wc.lpfnWndProc=WndProc;
	wc.hInstance=hInstance;
	wc.style=CS_HREDRAW|CS_VREDRAW;
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName="Demo";
	RegisterClass(&wc);

	if(window_mode)
	{
		LONG ws=WS_OVERLAPPEDWINDOW|WS_VISIBLE;
		ws &= ~WS_THICKFRAME;
		ws &= ~WS_MAXIMIZEBOX;

		RECT crt;
		SetRect(&crt, 0, 0, SCREEN_X, SCREEN_Y);
		AdjustWindowRect(&crt, ws, FALSE);

		hwnd = CreateWindow("Demo","사립탐정 이동헌: 나의 수호천사", ws, 100, 100, crt.right - crt.left, crt.bottom - crt.top, NULL, NULL, hInstance, NULL);
		ShowCursor( TRUE );
	}
	else
	{
		hwnd=CreateWindow("Demo","사립탐정 이동헌: 나의 수호천사",WS_POPUP|WS_VISIBLE,0,0,640,480,NULL,NULL,hInstance,NULL);
		ShowCursor( FALSE );
	}

	//셋업 정보 부르기
	FILE *fp;
	if(fp = fopen("setup.sav","rb"))
	{
		fread(&m_nosound,sizeof(BOOL),1,fp);
		fread(&m_640,sizeof(BOOL),1,fp);
		fclose(fp);
	}
	
	//초기화
	//if(m_640)jdd->Initialize(NULL,hwnd,640,480,16,true);
	//	else jdd->Initialize(NULL,hwnd,320,240,16,true);
	jdd->Initialize(NULL,hwnd,640,480,16,true,window_mode);

	//임시 서페이스 생성
	JPictureInfo jpi;
	jpi.SetWidth(SCREEN_X);
	jpi.SetHeight(SCREEN_Y);
	jdd->CreateSurface(SCREEN_BUFFER, &jpi, TRUE);

	//윈도우창 이동
	if(window_mode)
	{
		jdd->OnMove(100, 100);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}

	backbuffer=jdd->GetBackBuffer();
	font20=jdd->CreateFont("굴림체",20,true,false,false,false,false);
	font12=jdd->CreateFont("HY엽서L",15,true,false,false,false,false);
	srand( (unsigned)time( NULL ) );
	
	jdd->SetFrameRate(100,true);
	jdd->SetVerticalSync(false);

	//그림 데이터 불러오기
	jre->LoadResource("data\\back.mlc");
	jre->LoadResource("data\\face.mlc");
	jre->LoadResource("data\\battle.mlc");
	jre->LoadResource("data\\etc.mlc");

	if(!m_nosound)
	{
		//소리 데이터 불러오기
		//JSoundInfo si;
		//si.SetLoopState(true);
		//jds->LoadSound("Title","sound\\title.mp3",&si);
		//jds->LoadSound("BGM","sound\\bg.mp3",&si);
		//jds->LoadSound("Ending","sound\\end.mp3");
		//si.SetVolume(0.9f);
		//jds->LoadSound("Battle","sound\\battle.mid",&si);
		
		//jds->LoadSound("Ral","sound\\ral.mp3");
		//jds->LoadSound("Boomerang","sound\\boomerang.mp3");
		//jds->LoadSound("Fire","sound\\fire.mp3");
		//jds->LoadSound("Puck","sound\\puck.mp3");
		//jds->LoadSound("Lock","sound\\lock.mp3");
		//jds->LoadSound("Move","sound\\move.mp3");
	}

	bool end=false;
	m_sv.sw[8]=true;
	while(!end)
	{
		/////////////////////////////////////////////
		//본 게임
		int sec=0;
		while(!m_sv.sw[8])
		{
			if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
			{
				if(!GetMessage(&msg,NULL,0,0)) break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			//김미노의 추리
			if(!m_sv.sw[498] && m_sv.var[35]==0 && m_sv.var[27]==1)
			{
				m_dlg.TextSnr(621);
				m_sv.var[26]=0;
				m_sv.sw[498]=true;
				if(m_sv.sw[3])m_sv.SetVars(42,47,-1,2,-1,4,-1,-1);
					else m_sv.SetVars(42,47,-1,2,-1,-1,-1,-1);
			}
			//첫 살인
			else if(!m_sv.sw[7] && m_sv.var[26]==0 && m_game.GetTimeN()>=100000)
			{
				m_sv.sw[3]=false;
				m_sv.var[45]=-1;
				m_sv.var[27]=1;
				m_dlg.TextSnr(655);
				m_sv.sw[7]=true;
				m_sv.var[35]=0;
				FadeOut();
			}

			//행동
			if(_GetKeyState(VK_SPACE))m_game.Note(true);
			if(_GetKeyState(VK_RETURN))
			{
				if(!enter)
				{
					enter=true;

					bool existence=false;	//지금 장소에 사람 존재 여부
					CCommand m_com(99);
					//있는 사람을 검사
					for(int i=0; i<6; i++)
					{
						if(m_sv.var[42+i]==m_sv.var[35])
						{
							m_com.AddCom(0);
							if(m_sv.var[36+i]==2)	//제거 대상
							{
								m_com.AddCom(1);
							}
							else if(m_sv.var[36+i]==1)	//요주의 인물
							{
								m_com.AddCom(2);
								m_com.AddCom(3);
							}
							existence=true;
							break;
						}
					}
					if(!existence)m_com.AddCom(5);	//귀를 기울인다.

					if(m_sv.var[26]==4)m_com.AddCom(8);
					//조사와 이동과 대기
					m_com.AddCom(4);
					m_com.AddCom(6);
					m_com.AddCom(7);

					//명령 실행
					int com=m_dlg.TextSel(m_com.commands);
					if(m_com.GetComID(com)==0)	//대화
					{
						CCommand m_com2(0,8);
						m_com2.NormalCom(40,9+(m_sv.var[26]-1)*80,1);
					}
					else if(m_com.GetComID(com)==1)	//공격
					{
						//므족소녀 사망
						if(m_sv.var[26]==4)
						{
							m_dlg.TextSnr(618);
							m_sv.sw[m_sv.var[26]-1]=false;
							m_sv.var[41+m_sv.var[26]]=-1;
							m_sv.var[26]=0;
						}
						//김미노 도망
						else if(m_sv.var[27]==1 && m_sv.var[26]==2)
						{
							m_dlg.TextSnr(628);
							m_sv.var[43]=-1;
						}
						//전투
						else
						{
							//전투 전 대화
							if(m_sv.var[26]==1)m_dlg.TextSnr(657);
							else if(m_sv.var[26]==3)m_dlg.TextSnr(659);
							else if(m_sv.var[26]==5)m_dlg.TextSnr(661);
							else if(m_sv.var[26]==6)m_dlg.TextSnr(664);

							CBattle m_battle(m_sv.var[26]);
							bool win_lose=m_battle.Battle();
							FadeOut();
							if(win_lose==false)
							{								
								GameOver();
								m_sv.sw[8]=true;
								break;
							}
							else
							{
								//사망
								m_sv.sw[m_sv.var[26]-1]=false;
								m_sv.var[41+m_sv.var[26]]=-1;
								m_sv.var[26]=0;
							}
						}
						//첫 살인
						if(m_sv.var[27]==0)
						{
							m_dlg.TextSnr(613);
							m_sv.var[27]=1;
							m_sv.var[35]=11;
							m_dlg.TextSnr(614);
						}
					}
					else if(m_com.GetComID(com)==2)	//유도심문
					{
						CCommand m_com2(2,4);
						m_com2.NormalCom(8,9+(m_sv.var[26]-1)*80+40,-1);
					}
					else if(m_com.GetComID(com)==3)	//관찰
					{
						CCommand m_com2(3,8);
						m_com2.NormalCom(32,9+(m_sv.var[26]-1)*80+48,-1);
					}
					else if(m_com.GetComID(com)==4)	//조사
					{
						int backup=m_sv.var[26];
						m_sv.var[26]=0;
						//조사가능장소
						char invest_spot[12][7]={{0,1,2,3,4,5,6},{0,6,7,8,9,11,-1},
							{0,3,5,8,10,-1,-1},{0,8,9,10,11,-1,-1},{0,5,6,13,7,12,27},
							{0,14,15,16,-1,-1,-1},{0,17,18,19,7,8,-1},{21,20,22,23,6,-1,-1},
							{21,20,22,23,6,-1,-1},{21,20,22,23,6,-1,-1},{21,20,22,23,6,-1,-1},
							{0,24,25,26,-1,-1,-1}};

						CCommand m_com2(4,8);
						//조사 가능 장소 추가
						for(int i=0; i<7; i++)
						{
							if(invest_spot[m_sv.var[35]][i]==-1)break;
								else m_com2.AddCom(invest_spot[m_sv.var[35]][i]);
						}
						m_com2.AddCom(28);

						//대화창을 투명하게
						JPictureInfo pi;
						pi.SetOpacity(0.2f);
						jdd->SetPictureInfo("대화창",&pi);
						com=m_dlg.TextSel(m_com2.commands);
						//원상복귀
						pi.SetOpacity(0.8f);
						jdd->SetPictureInfo("대화창",&pi);
						//조사
						if(m_com2.GetComID(com)<28)
						{
							m_game.PlusTime(15);
							if(m_sv.var[35]==0)	//거실
							{
								if(com==4)
								{
									//쇼파및 동전 찾기
									if((rand()%3)==0)m_dlg.TextSnr(537);
										else
										{
											m_dlg.TextSnr(541);
											m_sv.var[33]++;
										}
								}
								else m_dlg.TextSnr(537+com);
							}
							else if(m_sv.var[35]==1)	//계단
							{
								switch(com)
								{
									case 0: m_dlg.TextSnr(537); break;
									default: m_dlg.TextSnr(544+com); break;
								}
							}
							else if(m_sv.var[35]==2)	//복도
							{
								switch(com)
								{
									case 0: m_dlg.TextSnr(537); break;
									default: m_dlg.TextSnr(549+com); break;
								}
							}
							else if(m_sv.var[35]==3)	//계단2
							{
								switch(com)
								{
									case 0: m_dlg.TextSnr(537); break;
									default: m_dlg.TextSnr(553+com); break;
								}
							}
							else if(m_sv.var[35]==4)	//3층
							{
								switch(com)
								{
									case 0: m_dlg.TextSnr(537); break;
									default: m_dlg.TextSnr(557+com); break;
								}
							}
							else if(m_sv.var[35]==5)	//욕실
							{
								m_dlg.TextSnr(564+com);
							}
							else if(m_sv.var[35]==6)	//주방
							{
								if(com==0)
								{
									m_dlg.TextSnr(537);
								}
								else if(com==1)
								{
									if(!m_sv.sw[489])
									{
										m_dlg.TextSnr(544);
										//비밀 싱크대
										int pass100=m_dlg.TextSel("0\n1\n2\n3\n4\n5\n6\n7\n8\n9\nA\nB\nC\nD\nE\nF");
										int pass10=m_dlg.TextSel("0\n1\n2\n3\n4\n5\n6\n7\n8\n9\nA\nB\nC\nD\nE\nF");
										int pass1=m_dlg.TextSel("0\n1\n2\n3\n4\n5\n6\n7\n8\n9\nA\nB\nC\nD\nE\nF");
										if(pass100==3 && pass10==8 && pass1==1)
										{
											//가스총 획득
											m_dlg.TextSnr(573);
											m_sv.var[4]++;
											m_sv.sw[489]=true;
										}
										else m_dlg.TextSnr(571);
									}
									else m_dlg.TextSnr(537);
								}
								else if(com==2)
								{
									//휴지통 뒤지기
									if((rand()%4)!=0)m_dlg.TextSnr(537);
										else
										{
											m_dlg.TextSnr(572);
											m_sv.var[EQUIPMAX]++;
										}
								}
								else m_dlg.TextSnr(565+com);
							}
							else if(m_sv.var[35]>=7 && m_sv.var[35]<=10)	//방
							{
								if(com==2)
								{
									//악마의 눈알
									if(m_sv.var[35]==10 && !m_sv.sw[490])
									{
										m_dlg.TextSnr(580);
										m_sv.var[5]=1;
										m_sv.sw[490]=true;
									}
									else m_dlg.TextSnr(537);
								}
								else m_dlg.TextSnr(578+com);
							}
							else if(m_sv.var[35]==11)	//밖
							{
								//열쇠
								if(com==1 && !m_sv.sw[497])
								{
									m_dlg.TextSnr(612);
									m_sv.var[EQUIPMAX+2]=1;
									m_sv.sw[497]=true;
								}
								else m_dlg.TextSnr(574+com);
							}
						}
						m_sv.var[26]=backup;
					}
					else if(m_com.GetComID(com)==5)	//귀를 기울이다.
					{
						int listen=41;
						if(m_sv.var[27]==0)	//1막
						{
							if(m_sv.CmpVar(35,4,0,5,6,11))	//1층
							{
								if(m_sv.sw[5] && m_sv.sw[4])listen=42+rand()%6;
									else if(m_sv.sw[5])listen=42+rand()%3;
									else if(m_sv.sw[4])listen=45+rand()%3;
							}
							else if(m_sv.CmpVar(35,6,1,2,7,8,9,10))	//2층
							{
								if(m_sv.sw[1] && m_sv.sw[2])listen=48+rand()%6;
									else if(m_sv.sw[2])listen=48+rand()%3;
									else if(m_sv.sw[1])listen=51+rand()%3;
							}
							else	//3층
							{
								if(m_sv.sw[0])listen=54+rand()%3;
							}
						}

						m_dlg.TextSnr(listen);
						m_sv.var[26]=0;
					}
					else if(m_com.GetComID(com)==6)	//이동
					{
						//이동가능장소(0:거실 1:계단 2:복도 3:3층으로의 계단 4:3층 5:욕실 6:주방 7~10:방 11 밖)
						char move_spot[12][6]={{1,5,6,-1,-1,-1}, {0,2,-1,-1,-1,-1}, {1,7,8,9,10,3}, {4,2,-1,-1,-1,-1},
											{3,-1,-1,-1,-1,-1}, {0,-1,-1,-1,-1,-1}, {0,-1,-1,-1,-1,-1},
											{2,-1,-1,-1,-1,-1}, {2,-1,-1,-1,-1,-1}, {2,-1,-1,-1,-1,-1},
											{2,-1,-1,-1,-1,-1}, {0,-1,-1,-1,-1,-1}};

						CCommand m_com2(6);
						//이동 가능 장소
						for(int i=0; i<6; i++)
						{
							if(move_spot[m_sv.var[35]][i]==-1)break;
								else m_com2.AddCom(move_spot[m_sv.var[35]][i]);
						}
						m_com2.AddCom(12);
						
						//이동
						com=m_dlg.TextSel(m_com2.commands);
						if(m_com2.GetComID(com)<12)
						{
							bool lock=false;
							//잠긴 방들
							if(m_sv.var[27]==0 && m_com2.GetComID(com)==10)lock=true;
							else if(m_sv.var[27]==1 && m_sv.var[EQUIPMAX+2]<1 &&
								m_com2.GetComID(com)>=7 && m_com2.GetComID(com)<=10)lock=true;

							if(lock)
							{
								Play("Lock");
								m_dlg.TextSnr(583);
							}
							else
							{
								Play("Move");
								m_sv.var[35]=m_com2.GetComID(com);
								m_sv.var[26]=0;
								//지금 장소에 있는 사람 검사
								for(int i=0; i<6; i++)
								{
									if(m_sv.var[42+i]==m_sv.var[35])
									{
										m_sv.var[26]=i+1;
										break;
									}
								}
								FadeOut();
								m_game.PlusTime(20);

								//첫 만남
								if(m_sv.var[26]>0 && m_sv.var[26]<7 && !m_sv.sw[490+m_sv.var[26]])
								{
									int first_meet[]={584,590,599,602,604,605};
									m_dlg.TextSnr(first_meet[m_sv.var[26]-1]);
									m_sv.sw[490+m_sv.var[26]]=true;
								}
								//사라진 사람들
								if(!m_sv.sw[499] && m_sv.var[27]==1 && m_sv.var[35]>=7 && m_sv.var[35]<=10)
								{
									m_sv.sw[499]=true;
									m_dlg.TextSnr(627);
									m_sv.var[43]=-1;
								}
								//기습
								if(m_sv.var[26]==0 && m_sv.sw[492] && !m_sv.sw[500] && (rand()%3)==0)
								{
									int fight[]={1,3,5,6};
									
									if(m_sv.sw[fight[m_sv.var[54]]-1])
									{
										m_dlg.TextSnr(631);
										CBattle m_battle(fight[m_sv.var[54]]);
										bool win_lose=m_battle.Battle();
										FadeOut();
										if(win_lose==false)
										{
											GameOver();
											m_sv.sw[8]=true;
											break;
										}
										else
										{
											//사망
											m_dlg.TextSnr(633);
											m_sv.sw[fight[m_sv.var[54]]-1]=false;
										}
										//김미노 사라짐
										m_sv.var[43]=-1;
									}

									if(++m_sv.var[54]>=4)m_sv.sw[500]=true;
								}
								//김미노 등장
								else if(m_sv.sw[500] && m_sv.var[35]==0)
								{
									m_dlg.TextSnr(634);

									CBattle m_battle(2);
									bool win_lose=m_battle.Battle();
									FadeOut();
									if(win_lose==false)
									{
										GameOver();
										m_sv.sw[8]=true;
										break;
									}
									else
									{
										//엔딩
										m_dlg.TextSnr(645);
										FadeOut();
										Ending();
										m_sv.sw[8]=true;
										break;
									}
								}
							}
						}
					}
					else if(m_com.GetComID(com)==8)	//도구 구입
					{
						CCommand m_com3(8,5);
						if(m_sv.var[33]>=2 && m_sv.var[33]<23)m_com3.AddCom(0);
						if(m_sv.var[33]>=5)m_com3.AddCom(1);
						if(m_sv.var[33]>=9)m_com3.AddCom(2);
						if(m_sv.var[33]>=23)m_com3.AddCom(3);
						m_com3.AddCom(4);
						switch(m_com3.GetComID(m_dlg.TextSel(m_com3.commands)))
						{
							case 0: m_sv.var[EQUIPMAX]++; m_sv.var[33]-=2; break;
							case 1: m_sv.var[EQUIPMAX+1]++; m_sv.var[33]-=5; break;
							case 2: m_sv.var[EQUIPMAX]+=5; m_sv.var[33]-=9; break;
							case 3: m_sv.var[EQUIPMAX+1]+=5; m_sv.var[33]-=23; break;
						}
					}
				}
			}
			else enter=false;

			if(++sec >= 100)
			{
				m_game.PlusTime(1000);
				sec=0;
			}
			m_dlg.Background();
			m_game.Render();
		}

		FadeOut();
		/////////////////////////////////////////////
		//메인메뉴
		//if(!m_nosound)jds->Stop("BGM");
		//if(!m_nosound)jds->Play("Title");
		jdd->DrawPicture(backbuffer,"Title",0,0,NULL);
		jdd->Render();
		Sleep(1000);
		while(m_sv.sw[8])
		{
			int main_menu=m_game.MainMenu();
			//if(!m_nosound)jds->Stop("Title");
			//새 게임
			if(main_menu==0)
			{
				m_sv.InitSwitch(false);
				m_sv.SetSwitch(0,5,true);	//생존

				m_sv.InitVar(0);
				m_sv.var[23]=9;			//시간
				m_sv.SetVars(32,33,100,10);	//체력, 돈
				m_sv.SetVar(0,3,1);	//아이템

				m_dlg.TextSnr(0);
				//마니또 확인
				m_sv.var[29]=rand()%7;
				m_dlg.TextSnr(25+m_sv.var[29]);
				//목적을 설정
				m_sv.var[30]=m_dlg.TextSel(32);
				if(m_sv.var[30]==0)	//게임을 즐기자
				{
					m_sv.SetVar(36,41,2);
					if(m_sv.var[29]>0)m_sv.var[35+m_sv.var[29]]=3;
				}
				else m_sv.SetVar(36,41,m_sv.var[30]);
				//조사를 시작
				m_dlg.TextSnr(33);
				m_sv.SetVars(42,47,4,10,8,2,5,6);
				m_game.Note(true);
				//if(!m_nosound)jds->Play("BGM");
			}
			else if(main_menu==1)
			{
				m_sv.sw[8]=false;
				//if(!m_nosound)jds->Play("BGM");
			}
			else if(main_menu==2)
			{
				end=true;
				m_sv.sw[8]=false;
			}
		}
		FadeOut();
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch ( msg )
    {
		case WM_MOVE		 :	if(jdd)jdd->OnMove(LOWORD(lParam), HIWORD(lParam));
								break;
		
		case WM_SIZE		 :	if(wParam == SIZE_MINIMIZED)activate=false;
								else activate=true;
								break;
		
		case WM_ACTIVATE	 : if(LOWORD(wParam))activate=true;
								else activate=false;
							   break;

		case WM_SYSCOMMAND	 :  //닫기 메시지 가로채기
								if(wParam==SC_CLOSE)
								{
									wParam=0;
									exit(0);
								}
								break;

	}

	return DefWindowProc(wnd,msg,wParam,lParam);
}