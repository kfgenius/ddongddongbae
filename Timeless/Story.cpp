#include "Story.h"

#define AUTO_X		440
#define AUTO_Y		296
#define AUTO_WIDTH	82
#define AUTO_HEIGHT	42

#define SKIP_X		522
#define SKIP_Y		296
#define SKIP_WIDTH	82
#define SKIP_HEIGHT	42

CStory::CStory(void)
{
	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0, 0, 0));

	jdd->LoadPicture("Cursor", "DATA\\cursor.png", &jpi, true);

	jdd->LoadPicture("AutoOff", "DATA\\��ȭâ-����-OFF.png", NULL, true);
	jdd->LoadPicture("AutoOn", "DATA\\��ȭâ-����-ON.png", NULL, true);
	jdd->LoadPicture("SkipOff", "DATA\\��ȭâ-��ŵ-OFF.png", NULL, true);
	jdd->LoadPicture("SkipOn", "DATA\\��ȭâ-��ŵ-ON.png", NULL, true);

	m_script = new CScript("script\\�׽�Ʈ");
	m_script->SetPage(0);
}

CStory::~CStory(void)
{
	jdd->DeleteSurface("Cursor");
}

void CStory::Process()
{
	//����
	if(MouseX >= AUTO_X && MouseY >= AUTO_Y && MouseX < AUTO_X + AUTO_WIDTH && MouseY < AUTO_Y + AUTO_HEIGHT)
	{
		if(LeftDown())
		{
			if(m_script->GetTextAuto())
			{
				m_script->SetTextAuto(FALSE);
			}
			else
			{
				m_script->SetTextAuto(TRUE);
			}
		}
	}

	//��ŵ
	if(MouseX >= SKIP_X && MouseY >= SKIP_Y && MouseX < SKIP_X + SKIP_WIDTH && MouseY < SKIP_Y + SKIP_HEIGHT)
	{
		if(LeftDown())
		{
			if(m_script->GetTextSkip())
			{
				m_script->SetTextSkip(FALSE);
			}
			else
			{
				m_script->SetTextSkip(TRUE);
			}
		}
	}

	//��ũ��Ʈ ó��
	m_script->Scripting();

	//���� ��ư
	if(m_script->GetTextAuto())
	{
		jdd->DrawPicture(backbuffer, "AutoOn", AUTO_X, AUTO_Y, NULL);
	}
	else
	{
		jdd->DrawPicture(backbuffer, "AutoOff", AUTO_X, AUTO_Y, NULL);
	}

	//��ŵ ��ư
	if(m_script->GetTextSkip())
	{
		jdd->DrawPicture(backbuffer, "SkipOn", SKIP_X, SKIP_Y, NULL);
	}
	else
	{
		jdd->DrawPicture(backbuffer, "SkipOff", SKIP_X, SKIP_Y, NULL);
	}

	//Ŀ��
	jdd->DrawPicture(backbuffer, "Cursor", MouseX, MouseY, NULL);

	CGameProcess::Process();
}
