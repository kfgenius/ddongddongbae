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

	jdd->LoadPicture("AutoOff", "DATA\\대화창-오토-OFF.png", NULL, true);
	jdd->LoadPicture("AutoOn", "DATA\\대화창-오토-ON.png", NULL, true);
	jdd->LoadPicture("SkipOff", "DATA\\대화창-스킵-OFF.png", NULL, true);
	jdd->LoadPicture("SkipOn", "DATA\\대화창-스킵-ON.png", NULL, true);

	m_script = new CScript("script\\테스트");
	m_script->SetPage(0);
}

CStory::~CStory(void)
{
	jdd->DeleteSurface("Cursor");
}

void CStory::Process()
{
	//오토
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

	//스킵
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

	//스크립트 처리
	m_script->Scripting();

	//오토 버튼
	if(m_script->GetTextAuto())
	{
		jdd->DrawPicture(backbuffer, "AutoOn", AUTO_X, AUTO_Y, NULL);
	}
	else
	{
		jdd->DrawPicture(backbuffer, "AutoOff", AUTO_X, AUTO_Y, NULL);
	}

	//스킵 버튼
	if(m_script->GetTextSkip())
	{
		jdd->DrawPicture(backbuffer, "SkipOn", SKIP_X, SKIP_Y, NULL);
	}
	else
	{
		jdd->DrawPicture(backbuffer, "SkipOff", SKIP_X, SKIP_Y, NULL);
	}

	//커서
	jdd->DrawPicture(backbuffer, "Cursor", MouseX, MouseY, NULL);

	CGameProcess::Process();
}
