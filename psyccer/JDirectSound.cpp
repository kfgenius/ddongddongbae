#include "JDirectSound.h"

#include <crtdbg.h>
#include <dxerr9.h>

#define DDC(x) if(FAILED(lasterr=x)) { _RPTF3(_CRT_WARN,"%s(0x%08X) while %s\n",DXGetErrorString9(lasterr),lasterr,#x); return false; }

GUID ds_devguids[10];
char ds_devdesc[10][100];
uint ds_devcount;

BOOL WINAPI DSEnumCallBack(GUID FAR* lpGUID,LPCSTR szDeviceDesc,LPCSTR,LPVOID)
{
	if(lpGUID==NULL)
		ds_devguids[ds_devcount].Data1=0;
	else
		ds_devguids[ds_devcount]=*lpGUID;
	strcpy(ds_devdesc[ds_devcount],szDeviceDesc);
	ds_devcount++;
	return TRUE;
}

void DSEnumDevices()
{
	ds_devcount=0;
	DirectSoundEnumerate(DSEnumCallBack,NULL);
}

JDirectSound::~JDirectSound() {}

uint JDirectSound::GetDeviceCount()
{
	return ds_devcount;
}

uint JDirectSound::GetDeviceDesc(uint idx,char* buffer)
{
	strcpy(buffer,ds_devdesc[idx]);
	return strlen(buffer);
}

#define INITGUID
#include "JDirectSoundImp.h"

#include <memory.h>

JDirectSound* CreateDirectSound() { return new JDirectSoundImp; }

static int hash(char* sz)
{
	unsigned int hashvalue=0;
	for(int lp=0;sz[lp];lp++)
		hashvalue=(hashvalue*HASH_NUMBER+sz[lp])%HASH_SIZE;
	return hashvalue;
}

void DSEnumDevices();

JDirectSoundImp::JDirectSoundImp() : lpds(NULL), hwnd(NULL)
{
	DSEnumDevices();
	memset(table,0,sizeof(table));
	CoInitialize(NULL);
}

JDirectSoundImp::~JDirectSoundImp()
{
	Cleanup();
	CoUninitialize();
}

struct winthdata
{
	HWND hwnd;
	HANDLE event;
};

#define WM_JDSNOTIFY WM_APP+1

static LRESULT CALLBACK MsgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_JDSNOTIFY:
		{
			IMediaEventEx* pme;
			long evcode,p1,p2;
			((IMediaControl*)lParam)->QueryInterface(IID_IMediaEventEx,(void**)&pme);
			pme->GetEvent(&evcode,&p1,&p2,0);
			if(evcode==EC_COMPLETE)
			{
				IMediaPosition* pmp;
				((IMediaControl*)lParam)->QueryInterface(IID_IMediaPosition,(void**)&pmp);
				pmp->put_CurrentPosition(0.0f);
				pmp->Release();
				((IMediaControl*)lParam)->Run();
			}
			pme->FreeEventParams(evcode,p1,p2);
			pme->Release();
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

DWORD WINAPI winthread(LPVOID param)
{
	WNDCLASS wc={0};
	wc.lpszClassName="WindowLess";
	wc.lpszMenuName=NULL;
	wc.lpfnWndProc=MsgProc;
	wc.hInstance=(HINSTANCE)0x00400000;
	RegisterClass(&wc);
	winthdata* wt=(winthdata*)param;
	wt->hwnd=CreateWindow("WindowLess","WindowLess",WS_POPUP,0,0,0,0,NULL,NULL,(HINSTANCE)0x00400000,NULL);
	SetEvent(wt->event);
	MSG msg;
	while(GetMessage(&msg,NULL,0,0)>0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

bool JDirectSoundImp::Initialize(uint devid,HWND focuswnd)
{
	if(lpds) return false;
	winthdata wt;
	wt.hwnd=NULL;
	wt.event=CreateEvent(NULL,false,false,NULL);
	DWORD tid;
	HANDLE th=CreateThread(NULL,0,winthread,&wt,0,&tid);
	WaitForSingleObject(wt.event,INFINITE);
	CloseHandle(th);
	CloseHandle(wt.event);
	hwnd=wt.hwnd;
	DDC(DirectSoundCreate(devid==0?NULL:&ds_devguids[devid],&lpds,NULL));
	lpds->SetCooperativeLevel(focuswnd,DSSCL_NORMAL);
	return true;
}

bool JDirectSoundImp::Cleanup()
{
	if(!lpds) return false;
	for(int lp=0;lp<HASH_SIZE;lp++) if(table[lp])
	{
		switch(table[lp]->type)
		{
		case DirectSound:
			DDC(table[lp]->buff->Release());
			delete table[lp]->dsbd.lpwfxFormat;
			break;
		case DirectShow:
			DDC(table[lp]->pmc->Release());
			DDC(table[lp]->pme->Release());
			DDC(table[lp]->pmp->Release());
			DDC(table[lp]->pba->Release());
			break;
		}
		delete table[lp];
		table[lp]=NULL;
	}
	DDC(lpds->Release());
	DestroyWindow(hwnd);
	return true;
}

bool JDirectSoundImp::LoadSound(char* name,char* filename,JSoundInfo* sndinfo)
{
	int key=hash(name);
	if(table[key]) return false;
	char* ext=strrchr(filename,'.');
	if(ext && !stricmp(ext,".wav"))
	{
#		define DDC2(x) if(FAILED(lasterr=x)) { CloseHandle(hfile); delete table[key]; table[key]=NULL; free(WaveFmt); return false; }
#		define ROLLBACK { delete table[key]; table[key]=NULL; CloseHandle(hfile); return false; }
		table[key]=new SoundTable;
		table[key]->type=DirectSound;

		HANDLE hfile=CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hfile==INVALID_HANDLE_VALUE) { delete table[key]; table[key]=NULL; return false; }
		DWORD fileid,dataid;
		struct ChunkInfo
		{
			DWORD id,size;
		} chunkinfo;

		LONG filesize;
		ULONG br;
		WAVEFORMATEX* WaveFmt=NULL;

		if(hfile==NULL) return false;
		// Verify that the first four chars is "RIFF", 0x46464952 in hex
		ReadFile(hfile,&fileid,4,&br,NULL);
		if(fileid!=0x46464952) ROLLBACK;
		// Read the entire size of file
		ReadFile(hfile,&filesize,4,&br,NULL);
		// Read chunks 
		//   - Verify that the ID of chunk is "WAVE", 0x45564157 in hex
		ReadFile(hfile,&dataid,4,&br,NULL);
		if(dataid!=0x45564157) ROLLBACK;
		//   - Read the format chunk of "WAVE" chunk
		//      - Read chunk information
		ReadFile(hfile,&chunkinfo,sizeof(ChunkInfo),&br,NULL);
		//      - Verify that the first four chars of format chunk is "fmt ", 0xXX746D66 in hex
		if((chunkinfo.id&(0x00FFFFFF))!=0x00746D66) ROLLBACK;
		//      - Read the format chunk
		WaveFmt=(WAVEFORMATEX*)malloc(chunkinfo.size);
		ReadFile(hfile,WaveFmt,chunkinfo.size,&br,NULL);
		//   - Read the wave chunk next to format chunk
		//      - Verify that the first four chars of format chunk is "wave", 0x61746164 in hex
		ReadFile(hfile,&chunkinfo,sizeof(ChunkInfo),&br,NULL);
		//      - if the ID of the chunk is not "wave',
		while(chunkinfo.id!=0x61746164)
		{
			LONG next;
			//  - Get position of the next chunk
			next=SetFilePointer(hfile,0,NULL,FILE_CURRENT)+chunkinfo.size;
			//  - If the next chunk`s position is over the file, fail to read wave, and exit;
			if(next>filesize) ROLLBACK;
			//  - Skip the chunk
			SetFilePointer(hfile,next,NULL,FILE_BEGIN);
			//  - Read the ID of next chunk
			ReadFile(hfile,&chunkinfo.id,4,&br,NULL);
		}

		DSBUFFERDESC dsbd={0};
		dsbd.dwSize=sizeof(dsbd);
		dsbd.dwBufferBytes=chunkinfo.size;
		dsbd.lpwfxFormat=WaveFmt;
		dsbd.dwFlags=DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN|DSBCAPS_GLOBALFOCUS;

		LPDIRECTSOUNDBUFFER lpdsb;

		DDC2(lpds->CreateSoundBuffer(&dsbd,&lpdsb,NULL));

		LPVOID dt;
		DDC2(lpdsb->Lock(0,0,&dt,&chunkinfo.size,NULL,0,DSBLOCK_ENTIREBUFFER));

		ReadFile(hfile,dt,chunkinfo.size,&br,NULL);
		DDC2(lpdsb->Unlock(dt,chunkinfo.size,NULL,0));

		table[key]->buff=lpdsb;
		table[key]->dsbd=dsbd;
#		undef DDC2
	} else {
#		define DDC2(x) if(FAILED(lasterr=x)) { delete table[key]; table[key]=NULL; return false; }
		table[key]=new SoundTable;
		table[key]->type=DirectShow;
		IGraphBuilder* pgb;
		DDC2(CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC,IID_IGraphBuilder,(void**)&pgb));
		static WCHAR temp[1001];
		MultiByteToWideChar(CP_ACP,0,filename,-1,temp,1000);
		DDC2(pgb->RenderFile(temp,NULL));
		DDC2(pgb->QueryInterface(IID_IMediaControl,(void**)&table[key]->pmc));
		DDC2(pgb->QueryInterface(IID_IMediaEventEx,(void**)&table[key]->pme));
		DDC2(pgb->QueryInterface(IID_IMediaPosition,(void**)&table[key]->pmp));
		DDC2(pgb->QueryInterface(IID_IBasicAudio,(void**)&table[key]->pba));
		DDC2(pgb->Release());
#		undef DDC2
	}
	SetSoundInfo(name,sndinfo);
	return true;
}


bool JDirectSoundImp::LoadSound(char* name,IStream* stream,JSoundInfo* sndinfo)
{
	int key=hash(name);
	if(table[key]) return false;
	if(!stream) return false;

#	define DDC2(x) if(FAILED(lasterr=x)) { delete table[key]; free(WaveFmt); return false; }
	table[key]=new SoundTable;
	table[key]->type=DirectSound;

	DWORD fileid,dataid;
	struct ChunkInfo
	{
		DWORD id,size;
	} chunkinfo;

	LONG filesize;
	ULONG br;
	WAVEFORMATEX* WaveFmt=NULL;

	// Verify that the first four chars is "RIFF", 0x46464952 in hex
	stream->Read(&fileid,4,&br);
	if(fileid!=0x46464952) return false;
	// Read the entire size of file
	stream->Read(&filesize,4,&br);
	// Read chunks 
	//   - Verify that the ID of chunk is "WAVE", 0x45564157 in hex
	stream->Read(&dataid,4,&br);
	if(dataid!=0x45564157) return false;
	//   - Read the format chunk of "WAVE" chunk
	//      - Read chunk information
	stream->Read(&chunkinfo,sizeof(ChunkInfo),&br);
	//      - Verify that the first four chars of format chunk is "fmt ", 0xXX746D66 in hex
	if((chunkinfo.id&(0x00FFFFFF))!=0x00746D66) return false;
	//      - Read the format chunk
	WaveFmt=(WAVEFORMATEX*)malloc(chunkinfo.size);
	stream->Read(WaveFmt,chunkinfo.size,&br);
	//   - Read the wave chunk next to format chunk
	//      - Verify that the first four chars of format chunk is "wave", 0x61746164 in hex
	stream->Read(&chunkinfo,sizeof(ChunkInfo),&br);
	//      - if the ID of the chunk is not "wave',
	while(chunkinfo.id!=0x61746164)
	{
		LARGE_INTEGER next={0};
		//  - Get position of the next chunk
		stream->Seek(next,STREAM_SEEK_CUR,(ULARGE_INTEGER*)&next);
		next.QuadPart+=chunkinfo.size;
		//  - If the next chunk`s position is over the file, fail to read wave, and exit;
		if(next.QuadPart>filesize) return false;
		//  - Skip the chunk
		stream->Seek(next,STREAM_SEEK_CUR,NULL);
		//  - Read the ID of next chunk
		stream->Read(&chunkinfo.id,4,&br);
	}

	DSBUFFERDESC dsbd={0};
	dsbd.dwSize=sizeof(dsbd);
	dsbd.dwBufferBytes=chunkinfo.size;
	dsbd.lpwfxFormat=WaveFmt;
	dsbd.dwFlags=DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN|DSBCAPS_GLOBALFOCUS;

	LPDIRECTSOUNDBUFFER lpdsb;

	DDC2(lpds->CreateSoundBuffer(&dsbd,&lpdsb,NULL));

	LPVOID dt;
	DDC2(lpdsb->Lock(0,0,&dt,&chunkinfo.size,NULL,0,DSBLOCK_ENTIREBUFFER));

	stream->Read(dt,chunkinfo.size,&br);
	DDC2(lpdsb->Unlock(dt,chunkinfo.size,NULL,0));

	table[key]->buff=lpdsb;
	table[key]->dsbd=dsbd;
#	undef DDC2
	SetSoundInfo(name,sndinfo);
	return true;
}

bool JDirectSoundImp::Play(char* name,bool wait)
{
	int key=hash(name);
	_RPT3(_CRT_WARN,"%s %d 0x%08x\n",name,key,table[key]);
	if(!table[key]) return false;
	switch(table[key]->type)
	{
	case DirectSound:
		_RPT1(_CRT_WARN,"0x%08x\n",table[key]->buff);
		if(table[key]->info.GetLoopState())
		{
			if(wait) return false;
			DDC(table[key]->buff->Play(0,0,DSBPLAY_LOOPING));
		} else {
			DDC(table[key]->buff->Play(0,0,0));
			if(wait)
			{
				int time=table[key]->dsbd.dwBufferBytes*1000/table[key]->dsbd.lpwfxFormat->nAvgBytesPerSec;
				Sleep(time);
			}
		}
		break;
	case DirectShow:
		_RPT4(_CRT_WARN,"0x%08x 0x%08x 0x%08x 0x%08x\n",
			table[key]->pme,
			table[key]->pmc,
			table[key]->pmp,
			table[key]->pba);
		if(table[key]->info.GetLoopState())
		{
			if(wait) return false;
			table[key]->pme->SetNotifyWindow((OAHWND)hwnd,WM_JDSNOTIFY,(long)table[key]->pmc);
		} else {
			table[key]->pme->SetNotifyWindow(NULL,0,0);
		}
		DDC(table[key]->pmc->Run());
		if(wait)
		{
			long dummy;
			DDC(table[key]->pme->WaitForCompletion(INFINITE,&dummy));
		}
		break;
	}
	return true;
}

bool JDirectSoundImp::Pause(char* name)
{
	int key=hash(name);
	if(!table[key]) return false;
	switch(table[key]->type)
	{
	case DirectSound:
		DDC(table[key]->buff->Stop());
		break;
	case DirectShow:
		DDC(table[key]->pmc->Pause());
		break;
	}
	return true;
}

bool JDirectSoundImp::SetPosition(char* name,double pos)
{
	int key=hash(name);
	if(!table[key]) return false;
	switch(table[key]->type)
	{
	case DirectSound:
		{
			DWORD offset=(DWORD)(table[key]->dsbd.dwBufferBytes*pos);
			int align=table[key]->dsbd.lpwfxFormat->nBlockAlign;
			offset=offset/align*align;
			DDC(table[key]->buff->SetCurrentPosition(offset));
		}
		break;
	case DirectShow:
		{
			double len;
			DDC(table[key]->pmp->get_Duration(&len));
			DDC(table[key]->pmp->put_CurrentPosition(pos*len));
		}
		break;
	}
	return true;
}

bool JDirectSoundImp::GetPosition(char* name,double* pos)
{
	int key=hash(name);
	if(!table[key]) return false;
	if(!pos) return false;
	switch(table[key]->type)
	{
	case DirectSound:
		{
			DWORD offset;
			DDC(table[key]->buff->GetCurrentPosition(&offset,NULL));
			*pos=offset/(double)table[key]->dsbd.dwBufferBytes;
		}
		break;
	case DirectShow:
		{
			double len;
			DDC(table[key]->pmp->get_Duration(&len));
			DDC(table[key]->pmp->get_CurrentPosition(pos));
			*pos/=len;
		}
		break;
	}
	return true;
}

bool JDirectSoundImp::Stop(char* name)
{
	int key=hash(name);
	if(!table[key]) return false;
	switch(table[key]->type)
	{
	case DirectSound:
		DDC(table[key]->buff->Stop());
		break;
	case DirectShow:
		DDC(table[key]->pmc->Stop());
		DDC(table[key]->pmp->put_CurrentPosition(0));
		break;
	}
	return true;
}

bool JDirectSoundImp::GetSoundInfo(char* name,JSoundInfo* buff)
{
	int key=hash(name);
	if(!table[key]) return false;
	if(!buff) return false;
	*buff=table[key]->info;
	return true;
}

bool JDirectSoundImp::SetSoundInfo(char* name,JSoundInfo* buff)
{
	int key=hash(name);
	if(!table[key]) return false;
	if(!buff) return false;
	table[key]->info=*buff;
	switch(table[key]->type)
	{
	case DirectSound:
		table[key]->buff->SetVolume((LONG)((1.0f-buff->GetVolume())*10000));
		table[key]->buff->SetPan((LONG)(buff->GetBalance()*10000));
		break;
	case DirectShow:
		table[key]->pba->put_Volume((long)((1.0f-buff->GetVolume())*10000));
		table[key]->pba->put_Balance((long)(buff->GetBalance()*10000));
		break;
	}
	return true;
}

bool JDirectSoundImp::DeleteSound(char* name)
{
	int key=hash(name);
	if(!table[key]) return false;
	switch(table[key]->type)
	{
	case DirectSound:
		DDC(table[key]->buff->Release());
		break;
	case DirectShow:
		DDC(table[key]->pmc->Release());
		DDC(table[key]->pme->Release());
		DDC(table[key]->pmp->Release());
		DDC(table[key]->pba->Release());
		break;
	}
	delete table[key];
	table[key]=NULL;
	return true;
}

HRESULT JDirectSoundImp::GetLastError()
{
	return lasterr;
}
