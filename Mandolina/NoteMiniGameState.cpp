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
	m_mold_attribute[mold_normal] = new CAttribute("normal_mold", 28, -46, -26);
	m_mold_attribute[mold_move] = new CAttribute("move_mold", 28, -46, -26);
	m_mold_attribute[mold_fat] = new CAttribute("fat_mold", 28, -46, -26);
	m_mold_attribute[mold_twin] = new CAttribute("twin_mold", 28, -46, -26);
	m_mold_attribute[mold_mini] = new CAttribute("mini_mold", 28, -46, -26);
	m_mold_attribute[mold_brain] = new CAttribute("brain_mold", 28, -46, -26);
	m_mold_attribute[mold_block] = new CAttribute("block_mold", 28, -46, -26);

	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0, 0, 255));

	jdd->LoadPicture("back", "DATA/back.png", NULL, true);
	jdd->LoadPicture("note", "DATA/note.png", &jpi, true);
	jdd->LoadPicture("mold", "DATA/mold.png", &jpi, true);
	jdd->LoadPicture("normal_mold", "DATA/normal_mold.png", &jpi, true);
	jdd->LoadPicture("move_mold", "DATA/move_mold.png", &jpi, true);
	jdd->LoadPicture("fat_mold", "DATA/fat_mold.png", &jpi, true);
	jdd->LoadPicture("twin_mold", "DATA/twin_mold.png", &jpi, true);
	jdd->LoadPicture("mini_mold", "DATA/mini_mold.png", &jpi, true);
	jdd->LoadPicture("brain_mold", "DATA/brain_mold.png", &jpi, true);
	jdd->LoadPicture("block_mold", "DATA/block_mold.png", &jpi, true);

	jdd->LoadPicture("failed", "DATA/failed.png", &jpi, true);
	jdd->LoadPicture("clear", "DATA/clear.png", &jpi, true);

	InitSound();

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

	phase = 0;
	next_time = 0;
	note_end = false;
	is_clear = false;

	LoadStagePhase(1);
	LoadStageMap(1);
}

NoteMiniGameState::~NoteMiniGameState()
{
	jdd->DeleteSurface("back");

	delete m_note_attribute;

	for(int i = 0; i < MOLD_TYPE_MAX; i++)
	{
		delete m_mold_attribute[i];
	}

	for(int i = 0; i < 4; i++)
	{
		SndObjDestroy(note_sound[i]);
	}

	if(stage_phase)
	{
		delete [] stage_phase;
	}
}

void NoteMiniGameState::LoadSpriteFiles()
{
}

void NoteMiniGameState::Process()
{
	static float time = 0;

	if(time >= next_time && note_end == false)
	{
		if(phase >= stage_phase_max)
		{
			note_end = true;
		}
		else
		{
			int id = GetFreeNoteID();
			if(id >= 0)
			{
				//int sound_id = (rand() % note_max);
				int sound_id = stage_phase[phase].note;
				next_time = (float)stage_phase[phase].delay;
				phase++;


				m_note[id]->Set((float)-10, (float)note_y[sound_id], m_note_attribute);
				m_note[id]->SetAngle(0);
				m_note[id]->SetSpeed(game_speed);

				_Play(note_sound[sound_id]);
			}

			time = 0;
		}
	}
	else
	{
		time += game_speed;
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

int NoteMiniGameState::GetFreeEnemyID()
{
	for(int i = 0; i < ENEMY_MAX; i++)
	{
		if(m_enemy[i]->GetLife() == false)
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
		gradient = (float)(y - old_y) / (x - old_x);
	}

	float intercept = y - gradient * x;

	for(int i = 0; i < NOTE_MAX; i++)
	{
		if(m_note[i]->GetLife() == true)
		{
			bool isCollision = m_note[i]->IsLineInSprite((float)x, (float)y, (float)old_x, (float)old_y, gradient, intercept);

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

int NoteMiniGameState::GetColonPos(char* buffer)
{
	size_t length = strlen(buffer);
	for(size_t i = 0; i < length; i++)
	{
		if(buffer[i] == ':')
		{
			return (int)i;
		}
	}

	return -1;
}

int NoteMiniGameState::CharToNote(char* buffer)
{
	if(buffer[0] == 'C')
	{
		if(buffer[1] == '4')
		{
			return note_4_do;
		}
		else if(buffer[1] == '5')
		{
			return note_5_do;
		}
	}
	else if(buffer[0] == 'D')
	{
		if(buffer[1] == '4')
		{
			return note_4_re;
		}
		else if(buffer[1] == '5')
		{
			return note_5_re;
		}
	}
	else if(buffer[0] == 'E')
	{
		if(buffer[1] == '4')
		{
			return note_4_mi;
		}
		else if(buffer[1] == '5')
		{
			return note_5_mi;
		}
	}
	else if(buffer[0] == 'F')
	{
		if(buffer[1] == '4')
		{
			return note_4_pa;
		}
	}
	else if(buffer[0] == 'G')
	{
		if(buffer[1] == '3')
		{
			return note_3_sol;
		}
		else if(buffer[1] == '4')
		{
			return note_4_sol;
		}
	}
	else if(buffer[0] == 'A')
	{
		if(buffer[1] == '3')
		{
			return note_3_ra;
		}
		else if(buffer[1] == '4')
		{
			return note_4_ra;
		}
	}
	else if(buffer[0] == 'B')
	{
		if(buffer[1] == '3')
		{
			return note_3_si;
		}
		else if(buffer[1] == '4')
		{
			return note_4_si;
		}
	}


	return 0;
}

void NoteMiniGameState::InitSound()
{
	if(SoundOBJ)
	{
		note_sound[note_3_sol] = SndObjCreate(SoundOBJ, "Sound\\sol.wav", 2);
		note_sound[note_4_re] = SndObjCreate(SoundOBJ, "Sound\\re.wav", 2);
		note_sound[note_4_ra] = SndObjCreate(SoundOBJ, "Sound\\mi.wav", 2);
		note_sound[note_5_mi] = SndObjCreate(SoundOBJ, "Sound\\ra.wav", 2);
	}
}

void NoteMiniGameState::LoadStagePhase(int stage)
{
	char filename[80];
	sprintf(filename, "Stage/stage%d.txt", stage);

	stage_phase_max = 0;
	FILE* fp = fopen(filename, "rb");
	stage_phase = NULL;
	game_speed = 1;

	if(fp)
	{
		char buffer[80];

		fgets(buffer, 80, fp);
		game_speed = (float)atoi(buffer) / 10;

		while(fgets(buffer, 80, fp))
		{
			if(GetColonPos(buffer) >= 2)	//0,1의 위치에 있으면 에러
			{
				stage_phase_max++;
			}
		}

		stage_phase = new StagePhase[stage_phase_max];

		fseek(fp, 0, SEEK_SET);

		int cx = 0;
		fgets(buffer, 80, fp);
		while(fgets(buffer, 80, fp))
		{
			int colon_pos = GetColonPos(buffer);
			if(colon_pos >= 2)
			{
				stage_phase[cx].note = CharToNote(buffer);
				stage_phase[cx].delay = atoi(&buffer[colon_pos + 1]);
				cx++;
			}
		}

		fclose(fp);
	}
}

void NoteMiniGameState::LoadStageMap(int stage)
{
	char filename[80];
	sprintf(filename, "Stage/stage%d_map.txt", stage);

	FILE* fp = fopen(filename, "rb");

	if(fp)
	{
		for(int i = 0; i < 8; i++)
		{
			char buffer[80];
			fgets(buffer, 80, fp);

			for(int j = 0; j < 8; j++)
			{
				if(buffer[j] >= '0' && buffer[j] <= '6')
				{
					int enemy_id = GetFreeEnemyID();
					m_enemy[enemy_id]->Set((float)(j * 40 + 20), (float)(i * 40 + 20), m_mold_attribute[buffer[j] - '0']);
				}
			}
		}

		fclose(fp);
	}
}