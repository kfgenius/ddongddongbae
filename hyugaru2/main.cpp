#include <stdio.h>
#include <math.h>

#include "donglib.h"
#include "dscript.h"
#include "srpg.h"

bool window_mode=true;

//�ؽ�
#define HASH(x)		jdd->GetHash(x)

//Ư�� ����
#define MAP_ID		144793
#define START_POINT	213225
#define BATTLE_MAP	253874


//////////////////////////////////////////////////
//���� �ڵ�
//////////////////////////////////////////////////
int main(char* arg[])
{
	//_crtBreakAlloc = 12057;

	//â ����
	if(!MainInitialize("Sample", TRUE, FALSE, window_mode))return 0;

	//������â �̵�
	if(window_mode)
	{
		jdd->OnMove(INIT_WINDOWX, INIT_WINDOWY);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}

	//���� ����
	map<int, int> m_valuemap;
	map<int, char*> m_textmap;

	//����
	HSNDOBJ m_snd[10];

	//�⺻ �׸� ���ҽ�
	jre->LoadResource("DATA\\common.mlc");
	jre->LoadResource("DATA\\global.mlc");

	//���� ����
	CSRPG* m_game = NULL;
	CScript m_script(m_snd, &m_valuemap, &m_textmap);
	BOOL fade_in;				//���̵� ���� ���� ����
	BOOL script_state=FALSE;	//�̺�Ʈ ������ �˻�

	//�⺻ �ý��� ��ũ��Ʈ ����
	m_script.Load("SYSTEM");
	m_script.SetPage(0);
	while(m_script.Script());

	//���� ����
	while(1)
	{
		if(!ProcessMessage())break;
		
		//�ʺ���
		if(!script_state && m_valuemap[MAP_ID]>=0)
		{
			FadeOut(1);

			char map_name[16];
			bool battle;

			if(m_valuemap[BATTLE_MAP] == 0)battle = false;
				else battle = true;
			sprintf(map_name, "MAP\\MAP%02d", m_valuemap[MAP_ID]);
			
			if(m_game)delete m_game;
			m_game = new CSRPG();
			m_game->Load(map_name, battle);
			m_game->SetAutoHeroXY(m_valuemap[START_POINT]);
			m_script.Load(map_name);

			m_valuemap[MAP_ID] = -1;
			m_valuemap[BATTLE_MAP] = 0;

			m_script.SetPage(0);
			fade_in = TRUE;
		}

		//�⺻ ���� ó��
		m_game->InitEvent();
		if(!script_state)m_game->Control();
		m_game->Process();
		m_game->Draw();

		//�̺�Ʈ ó��
		int event_no = m_game->GetEventNo();
		if(event_no >= 0)m_script.SetPage(event_no);
		script_state = m_script.Script();

		//���̵��� ó��
		if(fade_in)
		{
			FadeIn(1);
			fade_in = FALSE;
		}

		jdd->Render();
	}

	//���� ����
	delete m_game;
	m_game = NULL;

	//���ڸ� ����
	std::map<int, char*>::iterator it;
	for(it=m_textmap.begin(); it!=m_textmap.end(); ++it)
	{
		delete[] it->second;
		it->second = NULL;
	}

	//��Ʈ ��ü
	jdd->DeleteFont(global_font);

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