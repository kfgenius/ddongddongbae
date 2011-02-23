#include "Story.h"

CStory::CStory(void)
{
	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0, 0, 0));

	jdd->LoadPicture("Cursor", "DATA\\cursor.png", &jpi, true);

	m_script = new CScript("script\\테스트");
	m_script->SetPage(0);
}

CStory::~CStory(void)
{
	jdd->DeleteSurface("Cursor");
}

void CStory::Control()
{
}

void CStory::Process()
{
	m_script->Scripting();
}

void CStory::Render()
{
	//커서
	jdd->DrawPicture(backbuffer, "Cursor", MouseX, MouseY, NULL);
}
