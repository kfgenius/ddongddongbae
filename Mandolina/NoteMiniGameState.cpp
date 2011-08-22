#include "NoteMinigameState.h"
#include "NoteMinigameSoundManager.h"

#include "global.h"

#include <math.h>

static HSNDOBJ sound[4];

NoteMiniGameState::NoteMiniGameState()
{
	LoadSpriteFiles();

	for(int i = 0; i < NOTE_MAX; i++)
	{
		m_note[i] = new CNoteObject();
	}

	for(int i = 0; i < ENEMY_MAX; i++)
	{
		m_enemy[i] = new CEnemyObject();
	}

	old_LButton = false;

	m_note_attribute = new CAttribute("note", 10, -8, -38);
	m_mold_attribute = new CAttribute("mold", 28, -46, -26);

	m_enemy[0]->Set(100, 100, m_mold_attribute);

	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0, 0, 255));

	jdd->LoadPicture("back", "DATA/back.png", NULL, true);
	jdd->LoadPicture("note", "DATA/note.png", &jpi, true);
	jdd->LoadPicture("mold", "DATA/mold.png", &jpi, true);

	if(SoundOBJ)
	{
		sound[0] = SndObjCreate(SoundOBJ, "Sound\\mi.wav", 1);
		sound[1] = SndObjCreate(SoundOBJ, "Sound\\ra.wav", 1);
		sound[2] = SndObjCreate(SoundOBJ, "Sound\\re.wav", 1);
		sound[3] = SndObjCreate(SoundOBJ, "Sound\\sol.wav", 1);
	}
}

NoteMiniGameState::~NoteMiniGameState()
{
	jdd->DeleteSurface("back");

	delete m_note_attribute;
	delete m_mold_attribute;

	for(int i = 0; i < 4; i++)
	{
		SndObjDestroy(sound[i]);
	}
}

void NoteMiniGameState::LoadSpriteFiles()
{
}

void NoteMiniGameState::Process()
{
	static int time = 0;
	time++;
	if(time == 100)
	{
		int id = GetFreeNoteID();
		if(id >= 0)
		{
			int y = (rand() % 4);
			m_note[id]->Set(-10, SCREEN_HEIGHT - (y * 18 + 20), m_note_attribute);
			m_note[id]->SetAngle(0);
			_Play(sound[y]);
		}

		time = 0;
	}

	for(int i = 0; i < NOTE_MAX; i++)
	{
		m_note[i]->Update();
	}

	if(old_LButton == false && LButton == true)
	{
		TouchesDown(MouseX, MouseY);
	}
	else if(old_LButton == true && LButton == true)
	{
		TouchesMove(MouseX, MouseY);
	}
	else if(old_LButton == true && LButton == false)
	{
		TouchesUp(MouseX, MouseY);
	}

	for(int i = 0; i < NOTE_MAX; i++)
	{
		if(m_note[i]->GetLife() == true)
		{
			for(int j = 0; j < ENEMY_MAX; j++)
			{
				if(m_enemy[j]->GetLife() == true && m_enemy[j]->IsCollision(m_note[i]))
				{
					m_enemy[j]->Die();
				}
			}
		}
	}
	old_LButton = LButton;

	Draw();
}

void NoteMiniGameState::Draw()
{
	RenderNormalState();
}

//  ==== render =======
void NoteMiniGameState::RenderNormalState()
{
	jdd->DrawPicture(backbuffer, "back", 0, 0, NULL);

	for(int i = 0; i < NOTE_MAX; i++)
	{
		m_note[i]->Draw();
	}

	for(int i = 0; i < ENEMY_MAX; i++)
	{
		m_enemy[i]->Draw();
	}
}

void NoteMiniGameState::TouchesDown(int x, int y)
{
	origin_x = old_x = x;
	origin_y = old_y = y;
}

void NoteMiniGameState::TouchesUp(int x, int y)
{
}

void NoteMiniGameState::TouchesMove(int x, int y)
{
	NormalTouchesMove(x, y);

	old_x = x;
	old_y = y;
}

int NoteMiniGameState::GetFreeNoteID()
{
	for(int i = 0; i < NOTE_MAX; i++)
	{
		if(m_note[i]->GetLife() == false)
		{
			return i;
		}
	}

	return -1;
}

void NoteMiniGameState::NormalTouchesMove(int x, int y)
{
	int diffX = x - old_x;
	int diffY = y - old_y;

	float gradient;
	if((x - old_x) == 0)
	{
		gradient = 0;	//unused
	}
	else
	{
		gradient = (y - old_y) / (x - old_x);
	}

	float intercept = y - gradient * x;

	for(int i = 0; i < NOTE_MAX; i++)
	{
		if(m_note[i]->GetLife() == true)
		{
			bool isCollision = m_note[i]->IsLineInSprite(x, y, old_x, old_y, gradient, intercept);

			if(isCollision)
			{
				// Calculate the angle of the touch from the center of the joypad
				float dx = (float)diffX;
				float dy = (float)diffY;
				
				//float distance = sqrtf(dx * dx + dy * dy);
				
				float touchAngle = atan2(dy, dx);

				if(touchAngle < 0)
				{
					// Calculate the angle of the players touch from the center of the joypad
					//playerAngle = touchAngle;
					//playerSpeed = CLAMP(distance, 0, 50) * 4;

					m_note[i]->Bounce(touchAngle);
				}
			}
		}
	}
}