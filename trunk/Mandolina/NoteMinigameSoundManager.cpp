#include "NoteMinigameSoundManager.h"

NoteMinigameSoundManager::NoteMinigameSoundManager()
{
}

NoteMinigameSoundManager::~NoteMinigameSoundManager ()
{
}

void  NoteMinigameSoundManager::PlaySound(NoteMinigameSound id)
{
	if(id == sound_splash)
	{
		//VoxSoundManager::getInstance()->Play("sfx_acorn_splash_water");
	}
	else if(id == sound_bite)
	{
		//VoxSoundManager::getInstance()->Play("sfx_Note_bite");
	}
	else if(id == sound_gong)
	{
		//VoxSoundManager::getInstance()->Play("sfx_gong");
	}
	else if(id == sound_scrat_cryout)
	{
		//VoxSoundManager::getInstance()->Play("sfx_scrat_cryout");
	}
	else if(id == sound_scrat_punch1)
	{
		//VoxSoundManager::getInstance()->Play("sfx_scrat_punch_01");
	}
	else if(id == sound_scrat_punch2)
	{
		//VoxSoundManager::getInstance()->Play("sfx_scrat_punch_02");
	}
	else if(id == sound_scrat_kick1)
	{
		//VoxSoundManager::getInstance()->Play("sfx_scrat_kick_01");
	}
	else if(id == sound_scrat_kick2)
	{
		//VoxSoundManager::getInstance()->Play("sfx_scrat_kick_02");
	}
	else if(id == sound_Note_fly)
	{
		//VoxSoundManager::getInstance()->Play("sfx_Note_fly");
	}
	else if(id == sound_break_arcon)
	{
		//VoxSoundManager::getInstance()->Play("env_scrat_hurt");
	}
	else if(id == sound_combo)
	{
	}
}

void  NoteMinigameSoundManager::StopSound()
{
}