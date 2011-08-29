#include "NoteMinigameState.h"
#include "NoteMinigameSoundManager.h"

#include "global.h"

#include <math.h>

enum
{
	note_3_sol,
	note_3_ra,
	note_3_si,
	note_4_do,
	note_4_re,
	note_4_mi,
	note_4_pa,
	note_4_sol,
	note_4_ra,
	note_4_si,
	note_5_do,
	note_5_re,
	note_5_mi,
	note_max
};

HSNDOBJ note_sound[note_max];

int phase_1_1[] = {note_3_sol, 100, note_4_re, 100, note_4_ra, 100, note_5_mi, 100, note_4_ra, 100, note_4_re, 100, note_3_sol, 100, note_max};

int note_y[note_max];

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

	jdd->LoadPicture("failed", "DATA/failed.png", &jpi, true);
	jdd->LoadPicture("clear", "DATA/clear.png", &jpi, true);

	if(SoundOBJ)
	{
		note_sound[note_3_sol] = SndObjCreate(SoundOBJ, "Sound\\sol.wav", 2);
		note_sound[note_4_re] = SndObjCreate(SoundOBJ, "Sound\\re.wav", 2);
		note_sound[note_4_ra] = SndObjCreate(SoundOBJ, "Sound\\mi.wav", 2);
		note_sound[note_5_mi] = SndObjCreate(SoundOBJ, "Sound\\ra.wav", 2);
	}

	note_y[note_3_sol] = SCREEN_HEIGHT - 12;
	note_y[note_3_ra] = SCREEN_HEIGHT - 16;
	note_y[note_3_si] = SCREEN_HEIGHT - 20;
	note_y[note_4_do] = SCREEN_HEIGHT - 22;
	note_y[note_4_re] = SCREEN_HEIGHT - 26;
	note_y[note_4_mi] = SCREEN_HEIGHT - 30;
	note_y[note_4_pa] = SCREEN_HEIGHT - 32;
	note_y[note_4_sol] = SCREEN_HEIGHT - 36;
	note_y[note_4_ra] = SCREEN_HEIGHT - 40;
	note_y[note_4_si] = SCREEN_HEIGHT - 44;
	note_y[note_5_do] = SCREEN_HEIGHT - 46;
	note_y[note_5_re] = SCREEN_HEIGHT - 50;
	note_y[note_5_mi] = SCREEN_HEIGHT - 54;

	next_time = 0;
	note_end = false;
	is_clear = false;

	stage_phase = phase_1_1;
}

NoteMiniGameState::~NoteMiniGameState()
{
	jdd->DeleteSurface("back");

	delete m_note_attribute;
	delete m_mold_attribute;

	for(int i = 0; i < 4; i++)
	{
		SndObjDestroy(note_sound[i]);
	}
}

void NoteMiniGameState::LoadSpriteFiles()
{
}

void NoteMiniGameState::Process()
{
	static int time = 0;
	if(time == next_time && note_end == false)
	{
		int id = GetFreeNoteID();
		if(id >= 0)
		{
			//int sound_id = (rand() % note_max);
			static int phase = 0;
			int sound_id = stage_phase[phase];
			phase++;

			if(sound_id == note_max)
			{
				note_end = true;
			}
			else
			{
				m_note[id]->Set(-10, note_y[sound_id], m_note_attribute);
				m_note[id]->SetAngle(0);
				_Play(note_sound[sound_id]);

				next_time = stage_phase[phase];
				phase++;
			}
		}

		time = 0;
	}
	else
	{
		time++;
	}

	for(int i = 0; i < NOTE_MAX; i++)
	{
		m_note[i]->Update();
	}

	if(old_LButton == FALSE && LButton == TRUE)
	{
		TouchesDown(MouseX, MouseY);
	}
	else if(old_LButton == TRUE && LButton == TRUE)
	{
		TouchesMove(MouseX, MouseY);
	}
	else if(old_LButton == TRUE && LButton == FALSE)
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

	if(is_clear == false && IsNoEnemy())
	{
		is_clear = true;
	}

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

	if(is_clear == true)
	{
		jdd->DrawPicture(backbuffer, "clear", 0, 0, NULL);
	}
	else if(note_end == true && IsNoNote() == true)
	{
		jdd->DrawPicture(backbuffer, "failed", 0, 0, NULL);
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

bool NoteMiniGameState::IsNoNote()
{
	for(int i = 0; i < NOTE_MAX; i++)
	{
		if(m_note[i]->GetLife() == true)
		{
			return false;
		}
	}

	return true;
}

bool NoteMiniGameState::IsNoEnemy()
{
	for(int j = 0; j < ENEMY_MAX; j++)
	{
		if(m_enemy[j]->GetLife() == true)
		{
			return false;
		}
	}

	return true;
}