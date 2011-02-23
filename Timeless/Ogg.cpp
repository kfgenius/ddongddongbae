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
// 공개용 함수
//////////////////////////////////////////////////////////////////////////
//note : 클래스 사용에 필요한 객체생성
//note : 객체소거

//note : 사운드 파일 로드/Wave 컨버전
BOOL COgg::OpenOgg(char *filename)
{
	ReleaseOgg();

	LPDIRECTSOUNDBUFFER  pTemp;	// 임시버퍼

    FILE *f;
    f = fopen(filename, "rb");
	// File Loading 실패시
    if (!f) return FALSE;

	// vorbis의 ov_open함수를 사용항 Ogg 파일 오픈
    ov_open(f, &m_sVF, NULL, 0);

    // vorbis의 ov_info()함수를 이용 Ogg 정보 얻어오기
    vorbis_info *vi = ov_info(&m_sVF,-1);

    // DX SDK에서 지원하는 wave헤더 구조체
    WAVEFORMATEX        wfm;

	// wave header 셋팅 후 Ogg파일에서 가져온 정보를 이용하여 채워준다
    memset(&wfm, 0, sizeof(wfm));
    wfm.cbSize          = sizeof(wfm);
    wfm.nChannels       = vi->channels;
    wfm.wBitsPerSample  = 16;									// ogg vorbis는 항상 16bit
    wfm.nSamplesPerSec  = vi->rate;
    wfm.nAvgBytesPerSec = wfm.nSamplesPerSec*wfm.nChannels*2;	// 테이터사이즈 셋팅
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
    
    // 위에서 채워논 Ogg->wav 컨버전 헤더파일과
	// vorbis의 ov_read함수를 이용 Ogg->wav파일로 컨버팅 한다
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

	// vorbis를 이용 Ogg정보를 다 사용한후 삭제해준다.
	ov_clear(&m_sVF);
	fclose(f);

	buf = NULL;

    return TRUE;
}
//note : Ogg 파일재생
VOID COgg::PlayOgg(bool loop_flag)
{
	if(m_pDSBuffer==NULL)return;

	if(loop_flag)
		m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING);	// 반복재생
	else
		m_pDSBuffer->Play( 0, 0, NULL);	// 한번 재생
}
//note : Ogg 파일정지
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