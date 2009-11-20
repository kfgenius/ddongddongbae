//흥크립트 2008. 05. 22

#include <stdio.h>

#include <map>
#include <vector>
#include <deque>

using namespace std;

#include "donglib.h"

#define SCRIPT_VALUE_MAX	8	//스크립트에서 쓰는 변수의 수 한계
#define COMMAND_PTR			vector<CScriptCommand>::iterator

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
	DWORD flip;		//반전정보
	int loop;		//반복 표시 여부
	int gap_x;		//출력간격X
	int gap_y;		//출력간격Y
		
	//복원시 필요한 정보
	char src[80];	//그래픽 소스 이름
	int frame;		//애니메이션 프레임수
	int delay;		//애니메이션 딜레이
	int anitype;	//애니메이션 타입
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

//명령 함수 포인터 형식
class CScript;
typedef int (CScript::*FUNC)(vector<CScriptCommand>::iterator);

//스크립트 읽기 기능
class CScript
{
private:
	FILE *fp;
	FILE *debug_fp;

	int snrs;			//시나리오 한계
	char** snr;			//시나리오 포인터

	int script_no;			//처리 중인 명령줄
	vector<int> return_no;	//돌아가야할 곳

	void CreateCommandMap();

	//스크립트 처리
	BOOL ReadScript();		//스크립트를 읽어서 큐에 넣는 함수
	void BringScript();		//실행할 스크립트를 가져옴
	void RunScript();		//가져온 스크립트를 실행
	void ConvertString(char* result, char* source);	//문자에 섞여있는 변수를 숫자로 변환
	void LoadFromTxt();
	void DebugMode();		//디버그 모드인지 검사한 후 디버그 모드 시작


	BOOL enable;					//제대로 로드되어 사용가능한지 확인
	BOOL script_end;				//스크립트가 끝났는지 알아봄
	char script_file[80];			//게임 로드시 필요
	int screen_effect[2];			//화면효과
	int select_no;					//선택한 번호
	char input_buffer[80];			//입력된 글자를 임시로 저장하는 곳
	bool debug_mode;				//디버그 모드

	map<int, int> m_BookmarkHash;		//북마크 해쉬
	map<int, int> m_CommandMap;			//명령문 해쉬
	vector<CScriptCommand> m_script;	//스크립트 저장 공간

	//변수
	map<int, int> *m_valuemap;			//숫자 변수
	map<int, char*> *m_textmap;	//문자 변수

	//대화창, 선택창, 입력창
	CTextDlg m_dlg;
	CSelectDlg m_select;
	CInputDlg m_input;

	//그림
	CAnimation animation;
	map<int, ScriptPicture> m_pic;
	map<int, double> m_move_x;
	map<int, double> m_move_y;

	//사운드
	COgg* m_ogg;					//OGG 플레이어
	HSNDOBJ* m_snd;

	//보조 명령
	void Goto(int goto_page, int return_page);
	ScriptPicture* CheckPicture(COMMAND_PTR it);
	BOOL CheckBuffer(COMMAND_PTR it);

	///////////////////////////////////////////////
	//명령들
	map<int, FUNC> RunCommand;

	//문자 관련
	int ComPrint(COMMAND_PTR);
	int ComSelect(COMMAND_PTR);
	int ComInputNum(COMMAND_PTR);
	int ComInputText(COMMAND_PTR);

	//문자 속성 관련
	int ComTextColor(COMMAND_PTR);
	int ComShowShadow(COMMAND_PTR);
	int ComHideShadow(COMMAND_PTR);
	int ComDlgBox(COMMAND_PTR);
	int ComInputBox(COMMAND_PTR);
	int ComWindowOpacity(COMMAND_PTR);

	//그림 관련
	int ComPicture(COMMAND_PTR);
	int ComShowPicture(COMMAND_PTR);
	int ComMovePicture(COMMAND_PTR);
	int ComHidePicture(COMMAND_PTR);
	int ComPictureOpacity(COMMAND_PTR);
	int ComVFlip(COMMAND_PTR);
	int ComHFlip(COMMAND_PTR);
	int ComScreenEffect(COMMAND_PTR);

	//사운드 관련
	int ComPlayBGM(COMMAND_PTR);
	int ComStopBGM(COMMAND_PTR);
	int ComPlaySound(COMMAND_PTR);
	int ComAVI(COMMAND_PTR);

	//연산 관련
	int ComRandom(COMMAND_PTR);

	//흐름제어 관련
	int ComWait(COMMAND_PTR);
	int ComCase(COMMAND_PTR);
	int ComGoto(COMMAND_PTR);
	int ComReturn(COMMAND_PTR);
	int ComEnd(COMMAND_PTR);

public:
	//로드&언로드
	void Load(char* script_file);
	void UnLoad();

	//스크립트 처리 명령
	BOOL SetPage(int script_no);
	BOOL Script();

	//생성자&소멸자
	CScript(HSNDOBJ* m_snd, map<int, int> *m_valuemap, map<int, char*> *m_textmap);
	~CScript();
};