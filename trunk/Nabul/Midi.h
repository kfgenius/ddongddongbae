// Midi.h

#ifndef __MIDI_H__
#define __MIDI_H__

#include <mmsystem.h>

class CMidi
{

public:
	CMidi();
	~CMidi();

	int DeviceCount( void );

	BOOL Open( const char * );
	BOOL Close( void );

	BOOL Play( void );
	BOOL Stop( void );
	BOOL Pause( void );
	BOOL IsPlaying( void );

	BOOL GetLength( int *, int * );
	int GetMinutes( void );
	int GetSeconds( void );

	BOOL SeekTo( int, int );
	BOOL SkipForward( int );
	BOOL SkipBack( int );

	int GetTempo( void );
	BOOL SetTempo( int );

protected:
	BOOL m_bOpened, m_bPaused, m_bPlaying;
	WORD m_wDeviceID;
	int m_nDevices;

};

#endif
