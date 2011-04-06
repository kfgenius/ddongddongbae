#include "Logo.h"

#define FADE_DELAY	100

CLogo::CLogo(void)
{
	game_time = 0;

	jdd->LoadPicture("Logo", "DATA\\Logo.png", NULL, true);
}

CLogo::~CLogo(void)
{
	jdd->DeleteSurface("Logo");
}


//Ã³¸®
void CLogo::Process()
{
	game_time++;
	if(game_time == FADE_DELAY)
	{
		FadeOut();
		Stop();
	}

	if(game_time < FADE_DELAY)
	{
		jdd->DrawPicture(backbuffer, "Logo", 0, 0, NULL);
	}

	CGameProcess::Process();
}