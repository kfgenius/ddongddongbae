#pragma once

enum NoteMinigameSound
{
	sound_splash,
	sound_bite,
	sound_gong,
	sound_scrat_cryout,
	sound_scrat_punch1,
	sound_scrat_punch2,
	sound_scrat_kick1,
	sound_scrat_kick2,
	sound_Note_fly,
	sound_break_arcon,
	sound_combo
};

class NoteMinigameSoundManager
{
public:
	static NoteMinigameSoundManager* getInstance()
	{
		static NoteMinigameSoundManager instance;
		return &instance;
	}

public:
	NoteMinigameSoundManager();
	~NoteMinigameSoundManager();

	void PlaySound(NoteMinigameSound id);
	void StopSound();
};