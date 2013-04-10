// Sound.h: interface for the CSound class.
// ����   : �Ѱ������ ���� Ŭ�����̸�
//          �������� �ε��Ͽ� �����ϱ����ؼ���
//          �Ѱ�� �ʿ��� ����(m_pDSBuffer)�� ���������Ͽ�
//          ���� ����� �����.
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

//note : OggVorbis �������
#include    "vorbis/codec.h"
#include    "vorbis/vorbisfile.h"

    
#pragma comment(lib, "winmm")
#pragma comment(lib, "dsound")

#ifdef _DEBUG
	// ����� ���
	#pragma comment(lib, "lib/ogg_static_d")
	#pragma comment(lib, "lib/vorbis_static_d")
	#pragma comment(lib, "lib/vorbisfile_static_d")
#else
	// ������ ���
	#pragma comment(lib, "lib/ogg_static")
	#pragma comment(lib, "lib/vorbis_static")
	#pragma comment(lib, "lib/vorbisfile_static")
#endif

//note : ����ũ��
#define     BUFSIZE    1048576*60	//6�� �з�

class COgg  
{
protected:
	LPDIRECTSOUNDBUFFER	m_pDSBuffer;	// the buffer
	LPDIRECTSOUND		m_pDSound;		// DX Sound
	OggVorbis_File		m_sVF;			// OggVorbis ���� ����ü
	BOOL		        m_bInitDS;		// �ʱ�ȭ üũ

	void ReleaseOgg();

public:
	COgg(LPDIRECTSOUND lpds);
	virtual ~COgg();

	BOOL	OpenOgg(char *filename);		// ���� �ε�
    VOID	PlayOgg(bool loop_flag);		// ���
    VOID	StopOgg();						// ����
	VOID	SetVolume(long vol);			// �Ҹ� ����
};

#endif // !defined(AFX_SOUND_H__297F644C_C097_49AB_9D33_C13343AB1A75__INCLUDED_)
