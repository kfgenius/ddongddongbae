// Sound.h: interface for the CSound class.
// 설명   : 한곡재생을 위한 클래스이며
//          여러곳을 로드하여 관리하기위해서는
//          한곡당 필요한 버퍼(m_pDSBuffer)를 생성관리하여
//          사운드 기능을 만든다.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUND_H__297F644C_C097_49AB_9D33_C13343AB1A75__INCLUDED_)
#define AFX_SOUND_H__297F644C_C097_49AB_9D33_C13343AB1A75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include    <windows.h>
#include    <dsound.h>

template< typename PD3DOBJECT >
VOID D3DSAFE_RELEASE( PD3DOBJECT &pD3DObject )
{
	if( pD3DObject ){ pD3DObject->Release(); }
	pD3DObject = NULL;
}

//note : OggVorbis 헤더파일
#include    "vorbis/codec.h"
#include    "vorbis/vorbisfile.h"

    
#pragma comment(lib, "winmm")
#pragma comment(lib, "dsound")

#ifdef _DEBUG
	// 디버깅 모드
	#pragma comment(lib, "lib/ogg_static_d")
	#pragma comment(lib, "lib/vorbis_static_d")
	#pragma comment(lib, "lib/vorbisfile_static_d")
#else
	// 릴리즈 모드
	#pragma comment(lib, "lib/ogg_static")
	#pragma comment(lib, "lib/vorbis_static")
	#pragma comment(lib, "lib/vorbisfile_static")
#endif

//note : 버퍼크기
#define     BUFSIZE    1048576*60	//6분 분량

class COgg  
{
protected:
	LPDIRECTSOUNDBUFFER	m_pDSBuffer;	// the buffer
	LPDIRECTSOUND		m_pDSound;		// DX Sound
	OggVorbis_File		m_sVF;			// OggVorbis 파일 구조체
	BOOL		        m_bInitDS;		// 초기화 체크

	void ReleaseOgg();

public:
	COgg(LPDIRECTSOUND lpds);
	virtual ~COgg();

	BOOL	OpenOgg(char *filename);		// 사운드 로드
    VOID	PlayOgg(bool loop_flag);		// 재생
    VOID	StopOgg();						// 정지
	VOID	SetVolume(long vol);			// 소리 조절
};

#endif // !defined(AFX_SOUND_H__297F644C_C097_49AB_9D33_C13343AB1A75__INCLUDED_)
