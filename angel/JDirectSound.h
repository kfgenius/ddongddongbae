#ifndef __JDIRECTSOUND_H__
#define __JDIRECTSOUND_H__

#include <dsound.h>

#ifndef PURE
#define PURE =0
#endif

#ifndef UINT_DEFINED
#define UINT_DEFINED
typedef unsigned int uint;
#endif

class JSoundInfo
{
protected:
	bool m_loop;
	double m_volume;
	double m_balance;
public:
	JSoundInfo() : m_loop(false), m_volume(1.0f), m_balance(0.0f) {}

	void SetLoopState(bool loop=false) { m_loop=loop; }
	void SetVolume(double volume=1.0f) { m_volume=volume; }
	void SetBalance(double balance=0.0f) { m_balance=balance; }
	
	bool GetLoopState() { return m_loop; }
	double GetVolume() { return m_volume; }
	double GetBalance() { return m_balance; }
};

struct JDirectSound
{
	static uint GetDeviceCount();
	static uint GetDeviceDesc(uint idx,char* buffer);

	virtual ~JDirectSound();

	virtual bool Initialize(uint devid,HWND focuswnd) PURE;
	virtual bool Cleanup() PURE;

	virtual bool LoadSound(char* name,char* filename,JSoundInfo* sndinfo=NULL) PURE;
	virtual bool LoadSound(char* name,IStream* stream,JSoundInfo* sndinfo=NULL) PURE;

	virtual bool Play(char* name,bool wait=false) PURE;
	virtual bool Pause(char* name) PURE;

	// 0.0f = begin, 1.0f = end
	virtual bool SetPosition(char* name,double pos=0.0f) PURE;
	virtual bool GetPosition(char* name,double* pos) PURE;
	virtual bool Stop(char* name) PURE;

	virtual bool GetSoundInfo(char* name,JSoundInfo* buff) PURE;
	virtual bool SetSoundInfo(char* name,JSoundInfo* buff) PURE;

	virtual bool DeleteSound(char* name) PURE;

	virtual HRESULT GetLastError() PURE;
};

JDirectSound* CreateDirectSound();

#endif //__JDIRECTSOUND_H__