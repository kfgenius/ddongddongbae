//흥크립트 Ver0.5

#include <stdio.h>

#include <map>
#include <vector>
#include <deque>

using namespace std;

#include "donglib.h"

#define SCRIPT_VALUE_MAX		8	//스크립트에서 쓰는 변수의 수 한계

/////////////////////////////////////////////////////
//그림 구조체
struct ScriptPicture
{
	int id;			//그림 ID
	int x, y;		//그림 좌표
	double rx, ry;	//실좌표
	BOOL show;		//보이는지 숨기는지 여부
	BOOL ani;		//애니메이션인지 여부
	float opacity;	//투명도
	JColor key;		//칼라키
	DWORD filp;		//반전정보
	int loop;		//반복 표시 여부
	int gap_x;		//출력간격X
	int gap_y;		//출력간격Y
		
	//복원시 필요한 정보
	char src[80];	//그래픽 소스 이름
	int frame;		//애니메이션 프레임수
	int delay;		//애니메이션 딜레이
	int anitype;	//애니메이션 타입
};

/////////////////////////////////////////////////////
//문자 구조체
struct ScriptText
{
	int x, y;		//그림 좌표
	double rx, ry;	//실좌표
	BOOL show;		//보이는지 숨기는지 여부
	JColor color;	//글자색
	bool shadow;	//그림자
	
	//복원시 필요한 정보
	char buffer[80];	//글 내용
};

/////////////////////////////////////////////////////
//사운드 구조체
struct ScriptSound
{
	HSNDOBJ sound;	//사운드 오브젝트
	char src[80];	//사운드 소스 이름
};

///////////////////////////////////////////
//명령 클래스
class CScriptCommand 
{
public:
	int	id;							//명령문 번호
	int value[SCRIPT_VALUE_MAX];	//변수
	char* text;						//문자열 포인터
	char buffer[1024];				//문자열 버퍼
	bool first;						//처음에 해둬야 할 일을 체크하는 플래그

	int snr_no;						//자신의 시나리오 번호(디버그 용)

	CScriptCommand();
};

//전역함수 : 변수맵 클리어
void ClearValueMap();

//스크립트 읽기 기능
class CScript
{
private:
	FILE *fp;
	FILE *debug_fp;

	int snrs;			//시나리오 한계
	char** snr;			//시나리오 포인터

	bool script_end;		//스크립트 끝내기
	int script_no;			//처리 중인 명령줄
	vector<int> return_no;	//돌아가야할 곳

	void CreateCommandMap();

	BOOL IsBookMark(int no);
	int GetCommandID(int idx);

	int get_char_num(char chr);
	char DlgDecode(char b1, char b2);

	BOOL ReadScript(vector<CScriptCommand> *m_script, int no);		//스크립트를 읽어서 큐에 넣는 함수
	void BringScript();		//실행할 스크립트를 가져옴
	void RunScript();		//가져온 스크립트를 실행
	void ConvertString(char* result, char* source);	//문자에 섞여있는 변수를 숫자로 변환
	void DebugMode();		//디버그 모드인지 검사한 후 디버그 모드 시작
	void SystemMenu(bool save=true);	//시스템 메뉴
	void GetSystemValue(char* dest, char* src);		//시스템 변수 얻기

	char mlc_file[80];				//mlc 파일 이름(Unload때 필요)
	char script_file[80];			//게임 로드시 필요
	int screen_effect[2];			//화면효과
	BOOL enable;					//제대로 로드되어 사용가능한지 확인
	int select_value;				//선택지 변수를 임시로 저장하는 곳
	char input_buffer[80];			//입력된 글자를 임시로 저장하는 곳
	bool debug_mode;				//디버그 모드
	char chapter_name[80];			//세이브시 표시될 챕터 이름

	map<int, int> m_BookmarkHash;		//북마크 해쉬
	map<int, int> m_CommandMap;			//명령문 해쉬
	vector<CScriptCommand> m_script;	//스크립트 저장 공간

	//배경음
	char bgm_name[80];
	int music_type;

	bool on_savemenu, on_loadmenu;	//메뉴 호출 여부

	//글자색, 그림자
	int text_rgb[3];
	bool text_shadow;

	bool enable_save;	//세이브 가능 여부

	//영역 선택 정보
	bool area_select;
	char area_cursor[80];
	int area_x, area_y;

	//출력
	CTextDlg m_dlg;
	CSelectDlg m_select;
	CInputDlg m_input;

	//그림
	CAnimation animation;
	map<int, ScriptPicture> m_pic;

	//사운드
	map<int, ScriptSound> m_snd;
	COgg* m_ogg;					//OGG 플레이어
	CDirectMusic m_music;			//DirectMusic

public:
	void Script(int script_no);			//스크립트를 모두 실행

	//로드&언로드
	void Load(char* script_file);
	void LoadFromTxt();
	void UnLoad();

	//생성자 소멸자
	CScript(char* script_file);
	~CScript();
};