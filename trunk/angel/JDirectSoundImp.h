#ifndef __JDIRECTSOUNDIMP_H__
#define __JDIRECTSOUNDIMP_H__

#include "JDirectSound.h"

#include <dsound.h>
#include <dshow.h>

#define HASH_NUMBER 37
#define HASH_SIZE 262139

class JDirectSoundImp : public JDirectSound
{
protected:
	HWND hwnd;
	LPDIRECTSOUND lpds;
	HRESULT lasterr;
	enum soundtype
	{
		DirectSound,
		DirectShow
	};
	struct SoundTable
	{
		soundtype type;
		union
		{
			struct
			{
				LPDIRECTSOUNDBUFFER buff;
				DSBUFFERDESC dsbd;
			};
			struct
			{
				IMediaControl* pmc;
				IMediaEventEx* pme;
				IMediaPosition* pmp;
				IBasicAudio* pba;
			};
		};
		JSoundInfo info;
	} *table[HASH_SIZE];
public:
	JDirectSoundImp();
	virtual ~JDirectSoundImp();

	virtual bool Initialize(uint devid,HWND focuswnd);
	virtual bool Cleanup();

	virtual bool LoadSound(char* name,char* filename,JSoundInfo* sndinfo=NULL);
	virtual bool LoadSound(char* name,IStream* stream,JSoundInfo* sndinfo=NULL);

	virtual bool Play(char* name,bool wait=false);
	virtual bool Pause(char* name);
	virtual bool SetPosition(char* name,double pos=0.0f);
	virtual bool GetPosition(char* name,double* pos);
	virtual bool Stop(char* name);

	virtual bool GetSoundInfo(char* name,JSoundInfo* buff);
	virtual bool SetSoundInfo(char* name,JSoundInfo* buff);

	virtual bool DeleteSound(char* name);

	virtual HRESULT GetLastError();
};

#endif //__JDIRECTSOUNDIMP_H__