#include "MainMenu.h"

CMainMenu::CMainMenu(void)
{
	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0, 0, 0));

	jdd->LoadPicture("Title1", "DATA\\Title1.png", NULL, true);
	jdd->LoadPicture("Title2", "DATA\\Title2.png", NULL, true);
	jdd->LoadPicture("Title3", "DATA\\Title3.png", NULL, true);
	jdd->LoadPicture("Cursor", "DATA\\cursor.png", &jpi, true);

	game_time = 0;
}

CMainMenu::~CMainMenu(void)
{
	jdd->DeleteSurface("Title1");
	jdd->DeleteSurface("Title2");
	jdd->DeleteSurface("Title3");
	jdd->DeleteSurface("Cursor");
}

//처리
void CMainMenu::Process()
{
	//시간에 따른 타이틀 변화
	if(game_time == 0)
	{
		strcpy(title_name, "Title1");
	}
	else if(game_time == 100)
	{
		strcpy(title_name, "Title2");
	}
	else if(game_time == 200)
	{
		strcpy(title_name, "Title3");
	}

	game_time++;
	if(game_time >= 800)
	{
		game_time = 0;
	}

	//보여주기
	if(game_time == 1)
	{
		jdd->DrawPicture(backbuffer, "Title1", 0, 0, NULL);
		FadeIn();
	}
	else
	{
		jdd->DrawPicture(backbuffer, title_name, 0, 0, NULL);
		jdd->DrawPicture(backbuffer, "Cursor", MouseX, MouseY, NULL);
	}

	CGameProcess::Process();
}