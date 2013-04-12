// Sound.cpp: implementation of the CSound class.
//
//////////////////////////////////////////////////////////////////////

#include "Ogg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COgg::COgg(LPDIRECTSOUND lpds)
{
	m_pDSBuffer	= NULL;
	m_pDSound	= lpds;
}

COgg::~COgg()
{
	ReleaseOgg();
}

VOID COgg::ReleaseOgg()
{
	D3DSAFE_RELEASE( m_pDSBuffer);
}

//////////////////////////////////////////////////////////////////////////
// ������ �Լ�
//////////////////////////////////////////////////////////////////////////
//note : Ŭ���� ��뿡 �ʿ��� ��ü����
//note : ��ü�Ұ�

//note : ���� ���� �ε�/Wave ������
BOOL COgg::OpenOgg(char *filename)
{
	ReleaseOgg();

	LPDIRECTSOUNDBUFFER  pTemp;	// �ӽù���

    FILE *f;
    f = fopen(filename, "rb");
	// File Loading ���н�
    if (!f) return FALSE;

	// vorbis�� ov_open�Լ��� ����� Ogg ���� ����
    ov_open(f, &m_sVF, NULL, 0);

    // vorbis�� ov_info()�Լ��� �̿� Ogg ���� ������
    vorbis_info *vi = ov_info(&m_sVF,-1);

    // DX SDK���� �����ϴ� wave��� ����ü
    WAVEFORMATEX        wfm;

	// wave header ���� �� Ogg���Ͽ��� ������ ������ �̿��Ͽ� ä���ش�
    memset(&wfm, 0, sizeof(wfm));
    wfm.cbSize          = sizeof(wfm);
    wfm.nChannels       = vi->channels;
    wfm.wBitsPerSample  = 16;									// ogg vorbis�� �׻� 16bit
    wfm.nSamplesPerSec  = vi->rate;
    wfm.nAvgBytesPerSec = wfm.nSamplesPerSec*wfm.nChannels*2;	// �����ͻ����� ����
    wfm.nBlockAlign     = 2*wfm.nChannels;
    wfm.wFormatTag      = 1;

    // set up the buffer
    DSBUFFERDESC desc;

    desc.dwSize         = sizeof(desc);
    desc.dwFlags        = 0;
    desc.lpwfxFormat    = &wfm;
	desc.dwReserved     = 0;
    desc.dwBufferBytes  = BUFSIZE;


	// CREATE TEMPORARY BUFFER
    m_pDSound->CreateSoundBuffer(&desc, &pTemp, NULL );

	DWORD   size = BUFSIZE;
    DWORD   pos = 0;
    int     sec = 0;
    int     ret = 1;

    char    *buf;
    char    *buf2;

    pTemp->Lock(0, size, (LPVOID*)&buf, &size, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    
    // ������ ä���� Ogg->wav ������ ������ϰ�
	// vorbis�� ov_read�Լ��� �̿� Ogg->wav���Ϸ� ������ �Ѵ�
    while(ret && pos<size)
    {
        ret = ov_read(&m_sVF, buf+pos, size-pos, 0, 2, 1, &sec);
        pos += ret;
    }

    pTemp->Unlock( buf, size, NULL, NULL );

	//CREATE BUFFER TO MATCH ACTUAL SIZE OF FILE
    desc.dwBufferBytes  = pos;
	
    m_pDSound->CreateSoundBuffer(&desc, &m_pDSBuffer, NULL );
	m_pDSBuffer->Lock(0, pos, (LPVOID*)&buf2, &pos, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    CopyMemory(buf2,buf,pos);
	m_pDSBuffer->Unlock( buf2, size, NULL, NULL );
	
	//DUMP THE TEMP BUFFER
	pTemp->Release();

	// vorbis�� �̿� Ogg������ �� ������� �������ش�.
	ov_clear(&m_sVF);
	fclose(f);

	buf = NULL;

    return TRUE;
}
//note : Ogg �������
VOID COgg::PlayOgg(bool loop_flag)
{
	if(m_pDSBuffer==NULL)return;

	if(loop_flag)
		m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING);	// �ݺ����
	else
		m_pDSBuffer->Play( 0, 0, NULL);	// �ѹ� ���
}
//note : Ogg ��������
VOID COgg::StopOgg( )
{
	if(m_pDSBuffer==NULL)return;

	m_pDSBuffer->Stop();
}

VOID COgg::SetVolume(long vol)
{
	if(m_pDSBuffer==NULL)return;

	vol=vol*100-10000;
	m_pDSBuffer->SetVolume(vol);
}