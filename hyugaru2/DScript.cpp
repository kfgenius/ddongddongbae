//2007. 12. 30
#include <windows.h>

#include <string>

#include "DScript.h"
#include "calc.h"
#include "DlgEncoder.h"

//화면 효과
#define SE_FADE					0
#define SE_WHITE				1
#define SE_GRAY					2
#define SE_HUE					3
#define SE_RED					4
#define SE_GREEN				5
#define SE_BLUE					6
#define SE_QUAKE				7

//간략화를 위한 매크로
#define DELETE_COMMAND			it->id=-1	//명령문 폐기

//커맨드 완료 상황
#define RUN_NOTEND				-1
#define RUN_WELL				0
#define RUN_END					1
#define RUN_CLEAR				99
#define SCRIPT_END				-99

//해시
#define HASH(x)					jdd->GetHash(x)

//////////////////////////////////////////
//전역 함수

//변수가 섞였는지 검사
BOOL IncludeValue(char* str)
{
	if(strchr(str, '[')!=NULL || strchr(str, '{')!=NULL)return TRUE;
		else return FALSE;
}

//대문자를 소문자로
void ToSmallCase(char* str)
{
	size_t strmax = strlen(str);
	for(size_t i=0; i<strmax; ++i)
		if(str[i] >= 'A' && str[i] <= 'Z')str[i]+=0x20;
}

//시스템 사운드
HSNDOBJ sound[2];

/////////////////////////////////////////////////////
//스크립트 명령 메소드
CScriptCommand::CScriptCommand()
{
	//값 초기화
	ZeroMemory(value, sizeof(int)*SCRIPT_VALUE_MAX);
	text=NULL;
	strcpy(buffer,"");
	first=false;
}

//대화 파일 불러오기
CScript::CScript(HSNDOBJ* m_snd, map<int, int> *m_valuemap, map<int, char*> *m_textmap)
{	
	CreateCommandMap();

	//사운드
	if(SoundOBJ)m_ogg = new COgg(SoundOBJ);
		else m_ogg = NULL;
	
	this->m_snd = m_snd;

	//변수맵
	this->m_valuemap = m_valuemap;
	this->m_textmap = m_textmap;

	//기타
	debug_mode = false;
	enable = FALSE;
}

//대화내용 비우기
CScript::~CScript()
{
	//디버그 파일 닫기
	if(debug_mode)fclose(debug_fp);

	//스크립트 파일 언로드
	if(enable)UnLoad();

	//명령문 제거
	m_script.clear();

	//명령문 맵 해체
	RunCommand.clear();

	//음악 플레이어 제거
	if(m_ogg)
	{
		delete m_ogg;
		m_ogg = NULL;
	}
}

void CScript::Load(char* script_file)
{
	if(enable)UnLoad();	//이미 로드되어 있으면 이전 스크립트 언로드

	char tmp_file[256];
	//dlg로 읽기
	sprintf(tmp_file, "%s.dlg", script_file);
	if(fp=fopen(tmp_file, "rb"))
	{
		strcpy(this->script_file, script_file);
		//대사 수 읽기
		fread(&snrs,sizeof(int),1,fp);
		//책갈피 읽기
		int mark_count;
		fread(&mark_count,sizeof(int),1,fp);
		for(int i=0; i<mark_count; ++i)
		{
			int temp_bookmark, temp_no;
			fread(&temp_no,sizeof(int),1,fp);
			fread(&temp_bookmark,sizeof(int),1,fp);
			m_BookmarkHash[temp_no]=temp_bookmark;
		}
		//대사내용 읽기
		snr=new char*[snrs];
		for(int snr_no=0; snr_no<snrs; ++snr_no)
		{			
			//파일에서 읽기
			int text_size;
			fread(&text_size,sizeof(int),1,fp);
			snr[snr_no]=new char[text_size/2+1];

			char* buffer=new char[text_size+1];
			fread(buffer,text_size,1,fp);
			buffer[text_size]=NULL;

			//해독
			int i;
			for(i=0; i<text_size/2; ++i)
			{
				char b1, b2;
				b1=buffer[i*2];
				b2=buffer[i*2+1];

				snr[snr_no][i]=DlgDecode(b1, b2);
				if(snr[snr_no][i]=='\n' || snr[snr_no][i]=='\r')break;	//캐리지 리턴 걸러내기
			}
			snr[snr_no][i]=NULL;
			delete[] buffer;
		}

		fclose(fp);

		enable=TRUE;
	}
	else
	{
		//텍스트로 읽기
		sprintf(tmp_file, "%s.txt", script_file);
		if(fp=fopen(tmp_file, "rb"))
		{
			strcpy(this->script_file, script_file);
			LoadFromTxt();			
		}
		//파일을 찾지 못 했을 때
		else
		{	
			DebugMode();
			char error_msg[256];
			sprintf(error_msg, "%s 스크립트 파일을 찾을 수 없습니다!\n", script_file);
			fputs(error_msg, debug_fp);

			enable=FALSE;;

			return;
		}		
	}

	//스크립트 읽는 위치 초기화
	script_no = 0;
	script_end = TRUE;

	//화면효과 초기화
	screen_effect[0] = 0;
	screen_effect[1] = 0;
}

void CScript::LoadFromTxt()
{
	string dlg[10000];
	int dlg_no=0, mark_no;

	//사이즈 알아내기
	fseek(fp, 0, SEEK_END);
	long max=ftell(fp);
	fseek(fp, 0, SEEK_SET);

	bool note=false;
	bool content=false;
	int cp=0;

	bool is_text=false;	//""안의 내용인지 확인

	while(ftell(fp) < max)
	{
		char t;
		fread(&t, sizeof(char), 1, fp);
		//새 대사
		if(t=='\n')
		{
			if(content)
			{
				dlg_no++;
				content=note=false;
			}

			cp=0;
			is_text=false;

			continue;
		}
		else if(!content && (t==' ' || t==0x09 || t=='\r'))
		{
			continue;	//내용 없음
		}
		//책갈피
		else if(cp==0 && t=='^')
		{
			mark_no=0;
			bool ignore=false;
			do{
				fread(&t, sizeof(char), 1, fp);

				if(!ignore)
				{
					if(t>=48 && t<58)
					{
						mark_no*=10;
						mark_no+=t-48;
					}
					else
					{
						ignore=true;
					}
				}
			}while(t!='\n' && ftell(fp) < max);
			m_BookmarkHash[mark_no]=dlg_no;
			continue;
		}

		content=true;	//내용 있음

		//내용 복사
		if(!note)
		{
			if(t=='/' && !is_text)
			{
				char old_t=t;
				fread(&t, sizeof(char), 1, fp);
				//주석
				if(t=='/')
				{
					if(cp==0)dlg_no--;
					note=true;
				}
				//주석 아님, 그리고 줄 바꿈
				else if(t=='\n')
				{
					dlg[dlg_no]+=old_t;

					dlg_no++;
					content=note=false;
					cp=0;
					is_text=false;
				}
				//주석 아님
				else
				{
					if(t!='\r' && t!=0x09)	//탭과 캐리지 리턴은 무시
					{
						dlg[dlg_no]+=old_t;
						dlg[dlg_no]+=t;
						cp++;
					}
				}
			}
			else
			{
				if(t=='"')is_text=!is_text;
				if(t!='\r' && t!=0x09)dlg[dlg_no]+=t;
			}

			cp++;
		}
	}

	//마지막 명령이 내용이 있는지 검사
	if(dlg_no>=0)
	{
		bool content = false;
		size_t strmax = dlg[dlg_no].length();
		for(size_t i=0; i<strmax; i++)
		{
			if(dlg[dlg_no][i] != ' ' && dlg[dlg_no][i] != 0x09)
			{
				content = true;
				break;
			}
		}

		if(!content)--dlg_no;
	}
	else
	{
		fclose(fp);
		enable=FALSE;
		return;
	}

	snrs=dlg_no+1;

	//대사내용 읽기
	snr=new char*[snrs];
	for(int snr_no=0; snr_no<snrs; ++snr_no)
	{			
		//파일에서 읽기
		size_t text_size = dlg[snr_no].length();
		snr[snr_no]=new char[text_size+1];
		strcpy(snr[snr_no], dlg[snr_no].data());

		/*char debug[256];
		sprintf(debug, "%d:%s\n",snr_no,snr[snr_no]);
		OutputDebugString(debug); 디버그용*/
	}

	fclose(fp);

	enable=TRUE;

	//메모리 제거
	for(int i=0; i<10000; ++i)dlg[i].clear();
}

void CScript::UnLoad()
{
	//스크립트 메모리 해체
	if(snr)
	{
		for(int i=0; i<snrs; ++i)
			if(snr[i]!=NULL)
			{
				delete[] snr[i];
				snr[i]=NULL;
			}

		delete[] snr;
		snr = NULL;
	}

	//책갈피 해체
	m_BookmarkHash.clear();

	//픽쳐 해체
	m_pic.clear();
	m_move_x.clear();
	m_move_y.clear();

	//리턴 지점 비우기
	return_no.clear();

	//화면효과 초기화
	screen_effect[0] = 0;
	screen_effect[1] = 0;

	//실행 불가능 상태로 변경
	enable=FALSE;
}

//페이지 설정
BOOL CScript::SetPage(int script_no)
{
	//페이지가 존재하지 않거나 스크립트에 문제가 있을 때
	if(!enable || m_BookmarkHash.find(script_no) == m_BookmarkHash.end())
	{
		script_end = TRUE;
		return FALSE;
	}
	//이상이 없을 때
	else
	{
		this->script_no=m_BookmarkHash[script_no];
		script_end = FALSE;
		return TRUE;
	}
}

//스크립트 처리
BOOL CScript::Script()
{
	if(!enable || script_end)return FALSE;

	//스크립트 실행
	if(m_script.empty())
	{
		BringScript();
	}
	//스크립트 처리
	else
	{
		RunScript();
	}

	return !script_end;
}

void CScript::BringScript()
{
	BOOL read_next;	//계속해서 다음 명령을 읽는지 여부

	do{
		//스크립트 종료
		if(script_no >= snrs)
		{
			script_end = TRUE;
			break;
		}

		read_next=ReadScript();
		++script_no;
	}while(read_next);
}

//명령문 맵 생성
void CScript::CreateCommandMap()
{	
	//텍스트 관련 명령문
	RunCommand[HASH("출력")]=&CScript::ComPrint;
	RunCommand[HASH("선택")]=&CScript::ComSelect;
	RunCommand[HASH("숫자입력")]=&CScript::ComInputNum;
	RunCommand[HASH("글자입력")]=&CScript::ComInputText;

	//텍스트 설정 명령문
	RunCommand[HASH("대화창")]=&CScript::ComDlgBox;
	RunCommand[HASH("입력창")]=&CScript::ComInputBox;
	RunCommand[HASH("창불투명도")]=&CScript::ComWindowOpacity;
	RunCommand[HASH("글자색")]=&CScript::ComTextColor;
	RunCommand[HASH("그림자표시")]=&CScript::ComShowShadow;
	RunCommand[HASH("그림자숨김")]=&CScript::ComHideShadow;

	//그림 관련 명령문
	RunCommand[HASH("그림")]=&CScript::ComPicture;
	RunCommand[HASH("그림표시")]=&CScript::ComShowPicture;
	RunCommand[HASH("그림이동")]=&CScript::ComMovePicture;
	RunCommand[HASH("그림숨김")]=&CScript::ComHidePicture;
	RunCommand[HASH("그림불투명도")]=&CScript::ComPictureOpacity;
	RunCommand[HASH("좌우반전")]=&CScript::ComHFlip;
	RunCommand[HASH("상하반전")]=&CScript::ComVFlip;
	RunCommand[HASH("화면효과")]=&CScript::ComScreenEffect;

	//사운드 관련 명령문
	RunCommand[HASH("음악재생")]=&CScript::ComPlayBGM;
	RunCommand[HASH("음악정지")]=&CScript::ComStopBGM;
	RunCommand[HASH("소리재생")]=&CScript::ComPlaySound;
	RunCommand[HASH("AVI")]=&CScript::ComAVI;

	//연산
	RunCommand[HASH("랜덤")]=&CScript::ComRandom;

	//스크립트 흐름 제어
	RunCommand[HASH("대기")]=&CScript::ComWait;
	RunCommand[HASH("분기")]=&CScript::ComCase;
	RunCommand[HASH("이동")]=&CScript::ComGoto;
	RunCommand[HASH("돌아감")]=&CScript::ComReturn;
	RunCommand[HASH("끝")]=&CScript::ComEnd;
}

void CScript::ConvertString(char *result, char* source)
{
	size_t cp=0;			//읽어들이는 포인트
	char buffer[80];		//변수 이름 버퍼
	char num_buffer[80];	//변수 값을 가져 올 버퍼
	int depth=0;			//안에 또 변수가 있는지 여부

	size_t strmax = strlen(source);

	for(size_t i=0; i<strmax; ++i)
	{
		//숫자 변수
		if(source[i] == '[')
		{
			//변수 부분을 읽어 들임
			size_t start_point=++i;
			for(; i < strmax; ++i)
			{
				if(source[i]==']')	//이중, 삼중 괄호인지 검사
				{
					if(depth<=0)break;
						else --depth;
				}
				else if(source[i]=='[')++depth;

				buffer[i-start_point] = source[i];
			}
			buffer[i-start_point] = NULL;

			//변수 안의 변수 여부
			if(IncludeValue(buffer))
			{
				char tmp_buffer[80];
				strcpy(tmp_buffer, buffer);
				ConvertString(buffer, tmp_buffer);
			}

			//숫자와 합치기
			_itoa_s((*m_valuemap)[HASH(buffer)], num_buffer, 40, 10);
			result[cp]=NULL;
			strcat(result, num_buffer);
			cp = strlen(result);
		}
		//문자 변수
		else if(source[i] == '{')
		{
			//변수 부분을 읽어 들임
			size_t start_point=++i;
			for(; i < strmax; ++i)
			{
				if(source[i]=='}')	//이중, 삼중 괄호인지 검사
				{
					if(depth<=0)break;
						else --depth;
				}
				else if(source[i]=='{')++depth;

				buffer[i-start_point] = source[i];
			}
			buffer[i-start_point] = NULL;

			//변수 안의 변수 여부
			if(IncludeValue(buffer))
			{
				char tmp_buffer[80];
				strcpy(tmp_buffer, buffer);
				ConvertString(buffer, tmp_buffer);
			}

			//문자 합치기
			result[cp]=NULL;
			strcat(result, (*m_textmap)[HASH(buffer)]);
			cp = strlen(result);
		}
		else
		{
			result[cp] = source[i];
			++cp;
		}
	}

	//NULL이 안 들어갔을 경우를 대비해서
	result[cp] = NULL;
}

//명령문 읽어들이기
BOOL CScript::ReadScript()	//리턴하는 값은 계속해서 스크립트를 읽느냐 하는 것
{
	CScriptCommand temp_command;
	BOOL read_next=FALSE;
	char buffer[80];	//다용도 버퍼

	temp_command.snr_no = script_no;

	//변수 설정
	int depth = 0;
	if(snr[script_no][0]=='[')
	{
		//변수 이름 읽어들이기
		int i;
		for(i=1; snr[script_no][i] != NULL; ++i)
		{
			if(snr[script_no][i]==']')
			{
				if(depth<=0)break;
					else --depth;
			}
			else if(snr[script_no][i]=='[')++depth;

			buffer[i-1] = snr[script_no][i];
		}

		buffer[i-1] = NULL;

		//변수 이름 안에 변수가 있을 경우
		if(IncludeValue(buffer))
		{
			char tmp_buffer[80];
			strcpy(tmp_buffer, buffer);
			ConvertString(buffer, tmp_buffer);
		}

		//단순 계산인지 다른 변수를 이용한 계산인지 확인
		char* calc = NULL;
		char text_buffer[1024];

		if(IncludeValue(&snr[script_no][i+1]))	//다른 변수가 섞여 있을 때
		{
			ConvertString(text_buffer, &snr[script_no][i+1]);
			calc = text_buffer;
		}
		else	//단순 계산식일 때
		{
			calc = strchr(snr[script_no], ' ');
		}

		//변수에 값 넣기
		CEvaluator ev;
		EVALTYPE vals[26]={0,};
		EVALTYPE vr;

		if(calc != NULL)
		{
			int px = ev.Interpret(calc);
			if(px)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, script_no, snr[script_no]);
				fputs("잘못된 계산식입니다.\n", debug_fp);
				
				fprintf(debug_fp, "1: %s\n", calc);
			}
			else
			{
				px=ev.Evaluate(vr, vals);
				if(px)
				{
					DebugMode();
					fprintf(debug_fp, "%s(%d) - %s", script_file, script_no, snr[script_no]);
					fputs("잘못된 계산식입니다.\n", debug_fp);
				}
				else (*m_valuemap)[HASH(buffer)] = (int)vr;
			}
		}
		else if(debug_mode)
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, script_no, snr[script_no]);
			fputs("계산식이 존재하지 않습니다.\n", debug_fp);
		}

		return TRUE;
	}
	//문자 변수 설정
	else if(snr[script_no][0]=='{')
	{
		//문자 변수 이름 읽어들이기
		int i;
		for(i=1; snr[script_no][i] != NULL; ++i)
		{
			if(snr[script_no][i]=='}')
			{
				if(depth<=0)break;
					else --depth;
			}
			else if(snr[script_no][i]=='{')++depth;

			buffer[i-1] = snr[script_no][i];
		}
		buffer[i-1] = NULL;

		//변수 이름 안에 변수가 있을 경우
		if(IncludeValue(buffer))
		{
			char tmp_buffer[80];
			strcpy(tmp_buffer, buffer);
			ConvertString(buffer, tmp_buffer);
		}

		//시작점
		char* start_point = strchr(snr[script_no], '"');
		
		if(start_point == NULL)	//잘못된 변수 대입
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, script_no, snr[script_no]);
			fputs(" : \"\"안에 내용을 적으십시오.\n", debug_fp);
			return TRUE;	
		}

		//끝
		char* end_point = strrchr(snr[script_no], '"');

		if(end_point==start_point)	// "가 하나만 있음
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, script_no, snr[script_no]);
			fputs(" : 닫히는 \"가 없습니다.\n", debug_fp);
			return TRUE;
		}

		//문자 생성
		int value_length=(int)(end_point - start_point) - 1;

		if(value_length >= 0)
		{
			char tmp_buffer[80];
			if(value_length==0)	//빈 문자
			{
				strcpy(tmp_buffer, "");
			}
			else	//내용 복사
			{
				strncpy(tmp_buffer, start_point+1, value_length);
				tmp_buffer[value_length] = NULL;
			}

			if(IncludeValue(tmp_buffer))	//변수가 섞여 있을 때
			{
				char tmp_buffer2[80];
				strcpy(tmp_buffer2, tmp_buffer);
				ConvertString(tmp_buffer, tmp_buffer2);
			}

			//문자변수 넣기
			int text_id = HASH(buffer);
			if((*m_textmap)[text_id] != NULL)delete[] (*m_textmap)[text_id];
			(*m_textmap)[text_id] = new char[strlen(tmp_buffer) + 1];
			strcpy((*m_textmap)[text_id], tmp_buffer);
		}

		return TRUE;
	}
	//일반 대화
	else if(snr[script_no][0]!='@' && snr[script_no][0]!='~')
	{
		temp_command.id=HASH("출력");
		
		//변수가 있어 변환이 필요할 때는 버퍼사용, 아니면 포인터 사용
		if(IncludeValue(snr[script_no]))
		{
			ConvertString(temp_command.buffer, snr[script_no]);
		}
		else
		{
			temp_command.text=snr[script_no];
		}

		m_script.push_back(temp_command);
		return FALSE;
	}

	//다음 명령을 계속 읽는지 여부
	if(snr[script_no][0]=='~')read_next=TRUE;

	//명령을 읽어 들이기
	int value_id=0;		//상수일 경우 몇번째인지 확인
	size_t i;			//현재 읽고 있는 위치
	size_t strmax = strlen(snr[script_no]);	//문자열 크기
	for(i=1; i < strmax; ++i)
	{
		if(snr[script_no][i]==' ' || snr[script_no][i]==0x09)break;

		buffer[i-1]=snr[script_no][i];	//내용 복사
	}

	//명령어의 ID 알아내기
	buffer[i-1]=NULL;
	temp_command.id=HASH(buffer);

	//출력문, 선택문 경우 무조건 출력
	if(temp_command.id == HASH("출력") || temp_command.id == HASH("선택"))
	{
		temp_command.text=&snr[script_no][i+1];
		m_script.push_back(temp_command);
		return FALSE;
	}

	//값 읽어 들이기
	for(; i < strmax; ++i)
	{
		//상수 읽기
		if(snr[script_no][i]>='0' && snr[script_no][i]<='9' || snr[script_no][i]=='-')
		{
			int value_length=1;
			char* start_point = &snr[script_no][i++];

			for(; i < strmax && snr[script_no][i]>='0' && snr[script_no][i]<='9'; ++i)++value_length;	//길이 계산

			//숫자 복사
			strncpy(buffer, start_point, value_length);
			buffer[value_length] = NULL;

			//입력할 수 있는 변수 한계를 넘으면 입력 불가
			if(value_id < SCRIPT_VALUE_MAX)
			{
				temp_command.value[value_id]=atoi(buffer);
				++value_id;
			}
		}
		//문자
		else if(snr[script_no][i]=='"')
		{
			int value_length=0;
			char* start_point = &snr[script_no][++i];

			for(;  i < strmax && snr[script_no][i]!='"'; ++i)++value_length;	//길이 계산

			//문자 복사
			strncpy(temp_command.buffer, start_point, value_length);
			temp_command.buffer[value_length] = NULL;

			//변수나 문자 변환
			if(IncludeValue(temp_command.buffer))
			{
				char temp[80];
				strcpy(temp, temp_command.buffer);
				ConvertString(temp_command.buffer, temp);
			}
		}
		//변수 읽기
		else if(snr[script_no][i]=='[')
		{
			int value_length=0;
			//i++;
			char* start_point = &snr[script_no][++i];

			for(; i < strmax && snr[script_no][i]!=']'; ++i)++value_length;	//길이 계산

			//변수 이름 복사
			strncpy(buffer, start_point, value_length);
			buffer[value_length] = NULL;

			//변수 값을 찾아서 넘김
			if(value_id < SCRIPT_VALUE_MAX)
			{
				temp_command.value[value_id]=(*m_valuemap)[HASH(buffer)];
				++value_id;
			}
		}
		//문자열 읽기
		else if(snr[script_no][i]!=' ' && snr[script_no][i]!=0x09 && snr[script_no][i]!=',')
		{
			//단순 문자열인지 변수를 섞은 문자열인지 확인
			if(temp_command.id!=HASH("출력") && IncludeValue(&snr[script_no][i]))	//다른 변수가 섞여 있을 때
			{
				ConvertString(temp_command.buffer, &snr[script_no][i]);
			}
			else	//단순 계산식일 때
			{
				temp_command.text=&snr[script_no][i];	//문자 참조 위치
			}

			break;
		}
	}

	//디폴트 값
	if(temp_command.id==HASH("그림표시"))
	{
		if(value_id<4)temp_command.value[3] = 100;		//불투명도를 적지 않으면 기본 100
		if(value_id<5)temp_command.value[4] = 1;		//반복 수를 적지 않으면 기본 1
	}
	else if(temp_command.id==HASH("그림숨김"))
	{
		if(value_id<1)temp_command.value[0] = -1;	//그림 숨기기 번호를 적지 않으면 모두 숨김
	}
	else if(temp_command.id==HASH("클릭"))
	{
		if(value_id<2)		//영역을 설정 안하면 전체 영역
		{
			temp_command.value[1] = 0;
			temp_command.value[2] = 0;
			temp_command.value[3] = 640;
			temp_command.value[4] = 480;
		}
	}
	else if(temp_command.id==HASH("모험"))
	{
		if(value_id<1)temp_command.value[0] = -1;	//좌표를 안 적으면 자동 지정
	}

	//명령문을 큐에 넣음
	m_script.push_back(temp_command);
	return read_next;
}

void CScript::DebugMode()
{
	if(!debug_mode)
	{
		if(debug_fp = fopen("bug_repot.txt", "wt"))debug_mode = true;
	}
}

//==================== 스크립트 처리 =======================//
void CScript::RunScript()
{
	if(m_script.empty())return;	//아무 작업도 안 들어 있을 때

	//그림 표시
	std::map<int, ScriptPicture>::iterator pit;
	for(pit=m_pic.begin(); pit!=m_pic.end(); ++pit)
	{
		if((pit->second).show)
		{
			int px = (pit->second).x;
			int py = (pit->second).y;
			JPictureInfo pi;
			pi.SetColorKey((pit->second).key);
			pi.SetOpacity((pit->second).opacity);
			jdd->SetPictureInfo((pit->second).id, &pi);

			for(int i=0; i<pit->second.loop; ++i)
			{
				if((pit->second).ani)	//애니메이션인 경우
				{
					//뒤집기
					if((pit->second).flip)
					{
						animation.GetAni((pit->second).id)->DrawEx(jdd->GetBackBufferID(), (pit->second).id, px, py, (pit->second).flip);
					}
					//일반 출력
					else
					{
						animation.GetAni((pit->second).id)->Draw(jdd->GetBackBufferID(), (pit->second).id, px, py);
					}
				}
				else	//일반 그림일 경우
				{
					//뒤집기
					if((pit->second).flip)
					{
						jdd->DrawPictureEx(jdd->GetBackBufferID(), (pit->second).id, px, py, NULL, (pit->second).flip);
					}
					//일반 출력
					else
					{
						jdd->DrawPicture(jdd->GetBackBufferID(), (pit->second).id, px, py, NULL);
					}
				}

				//여러장 출력일 때는 좌표를 이동하며 출력
				px += (pit->second).gap_x;
				py += (pit->second).gap_y;
			}
		}
	}

	animation.Process();

	//화면 효과
	if(screen_effect[1] > 0)
	{
		if(screen_effect[0] == SE_FADE)jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_fadeout(screen_effect[1]));
			else if(screen_effect[0] == SE_WHITE)jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_whiteout(screen_effect[1]));
			else if(screen_effect[0] == SE_GRAY)jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_partial_gray(screen_effect[1]));
			else if(screen_effect[0] == SE_HUE)jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_hue_change(screen_effect[1]*3));
			else if(screen_effect[0] == SE_RED)
			{
				ColorMatrix2 CM_red={{{256,0,0,0},{0,256-screen_effect[1],0,0},{0,0,256-screen_effect[1],0}}};
				jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_red);
			}
			else if(screen_effect[0] == SE_GREEN)
			{
				ColorMatrix2 CM_green={{{256-screen_effect[1],0,0,0},{0,256,0,0},{0,0,256-screen_effect[1],0}}};
				jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_green);
			}
			else if(screen_effect[0] == SE_BLUE)
			{
				ColorMatrix2 CM_blue={{{256-screen_effect[1],0,0,0},{0,256-screen_effect[1],0,0},{0,0,256,0}}};
				jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_blue);
			}
	}

	//큐 안의 모든 작업이 끝났는지 검사하는 플래그, 일단 한번에 끝나는 작업이 대부분이므로 디폴트는 TRUE
	BOOL done=TRUE;

	//명령 실행
	std::vector<CScriptCommand>::iterator it;
	for(it=m_script.begin(); it!=m_script.end(); ++it)
	{
		if(RunCommand.find(it->id) != RunCommand.end())
		{
			int result = (this->*RunCommand[it->id])(it);
			
			//명령이 종료되지 않음
			if(result == RUN_NOTEND)
			{
				done=FALSE;
			}
			//명령이 종료되었음
			else if(result == RUN_END)
			{
				DELETE_COMMAND;
			}
			//나머지 명령까지 종료
			else if(result == RUN_CLEAR)
			{
				done=TRUE;
				break;
			}
			//스크립트 종료
			else if(result == SCRIPT_END)
			{
				done=TRUE;
				script_end = TRUE;
				break;
			}
		}
	}

	//모든 명령이 완료되었으면 큐 삭제
	if(done)m_script.clear();
}

/////////////////////////////////////////////////////////////////
//보조 명령문

//페이지 이동
void CScript::Goto(int goto_page, int return_page)
{
	return_no.push_back(return_page);	//돌아올 주소 저장
	script_no = m_BookmarkHash[goto_page];	//이동
}

//그림 ID 받아오기
ScriptPicture* CScript::CheckPicture(COMMAND_PTR it)
{
	//그림이 존재하는지 검사
	if(!it->first && m_pic.find(it->value[0]) == m_pic.end())
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : 그림을 찾을 수 없습니다.\n", debug_fp);

		return NULL;
	}

	return &m_pic[it->value[0]];	//사용중인 그림 포인터
}

//버퍼에 글자를 적었는지 검사
BOOL CScript::CheckBuffer(COMMAND_PTR it)
{
	//변수를 적었는지 검사
	if(!it->first && strcmp(it->buffer, "")==0)
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : 문자변수 이름를 적지 않았습니다.\n", debug_fp);

		return FALSE;
	}
	else return TRUE;
}

/////////////////////////////////////////////////////////////////
//명령문 처리

//출력
int CScript::ComPrint(COMMAND_PTR it)
{
	if(!it->first)	//맨 처음은 대화를 생성
	{
		//버퍼 내용이 있을 때는 버퍼로 생성, 아니면 문자열 포인터로 생성
		if(strcmp(it->buffer, "")==0)m_dlg.MakeText(it->text);
			else m_dlg.MakeText(it->buffer);

		it->first = true;
	}

	//대화 출력이 끝나면 종료
	int result=m_dlg.Printing();
	if(result==999 || result==-1)return RUN_END;
		else return RUN_NOTEND;
}

//선택
int CScript::ComSelect(COMMAND_PTR it)
{
	//선택창 생성
	if(!it->first)
	{
		if(strcmp(it->buffer, "")==0)m_select.MakeSelection(it->text);
			else m_select.MakeSelection(it->buffer);
		m_select.SetSelect(0);	//위치 초기화

		it->first = true;
	}

	//선택이 끝날 때까지 선택창 표시
	int result=m_select.Selecting();
	if(result==-1 || result==-999)return RUN_NOTEND;
	else
	{
		select_no=result;
		return RUN_END;
	}
}

//숫자 입력
int CScript::ComInputNum(COMMAND_PTR it)
{
	if(!CheckBuffer(it))return RUN_END;	//버퍼에 문자를 적지 않았음

	//입력창 환경 설정
	if(!it->first)
	{
		m_input.SetFlag(FLAG_NUMBER);
		m_input.Clear();
		it->first=true;
	}

	//입력
	int result=m_input.Inputing();
	if(result==0)return RUN_NOTEND;
	else
	{
		strcpy(input_buffer, m_input.input_text);

		if(strcmp(it->buffer,"") == 0)
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
			fputs(" : 값을 넣을 변수를 적지 않았습니다.\n", debug_fp);
		}
		else
		{
			(*m_valuemap)[HASH(it->buffer)] = atoi(input_buffer);
		}

		return RUN_END;
	}
}

//문자 입력
int CScript::ComInputText(COMMAND_PTR it)
{
	if(!CheckBuffer(it))return RUN_END;	//버퍼에 문자를 적지 않았음

	//입력창 환경 설정
	if(!it->first)
	{
		m_input.SetFlag(FLAG_TEXT | FLAG_NUMBER | FLAG_MARK);
		m_input.Clear();
		it->first=true;
	}

	//입력
	int result=m_input.Inputing();
	if(result==0)return RUN_NOTEND;
	else
	{
		strcpy(input_buffer, m_input.input_text);

		if(strcmp(it->buffer,"") == 0)
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
			fputs(" : 값을 넣을 변수를 적지 않았습니다.\n", debug_fp);
		}
		else
		{
			strcpy((*m_textmap)[HASH(it->buffer)], input_buffer);
		}

		return RUN_END;
	}
}

//글자색
int CScript::ComTextColor(COMMAND_PTR it)
{
	m_dlg.SetColor(it->value[0], it->value[1], it->value[2]);
	m_select.SetColor(it->value[0], it->value[1], it->value[2]);
	m_input.SetColor(it->value[0], it->value[1], it->value[2]);

	return RUN_END;
}

//그림자 보이기
int CScript::ComShowShadow(COMMAND_PTR it)
{
	m_dlg.ShowShadow(true);
	m_select.ShowShadow(true);
	m_input.ShowShadow(true);

	return RUN_END;
}

//그림자 숨기기
int CScript::ComHideShadow(COMMAND_PTR it)
{
	m_dlg.ShowShadow(false);
	m_select.ShowShadow(false);
	m_input.ShowShadow(false);

	return RUN_END;
}

//대화창 설정
int CScript::ComDlgBox(COMMAND_PTR it)
{
	//선택장, 대화창은 같은 속성으로 설정
	m_dlg.SetDlg(it->value[0], it->value[1], it->value[2], it->value[3]);
	m_select.SetDlg(it->value[0], it->value[1], it->value[2], it->value[3]);

	//대화창 그림 생성
	m_dlg.MakeDlgBox("_textbox");
	m_select.SetDlgBox("_textbox");
	
	return RUN_END;
}

//입력창 설정
int CScript::ComInputBox(COMMAND_PTR it)
{
	m_input.SetDlg(it->value[0],it->value[1],it->value[2]);
	
	//입력창 그림 생성
	JPictureInfo pi;
	if(jdd->GetPictureInfo("_inputbox", &pi))jdd->DeleteSurface("_inputbox");
	m_input.MakeDlgBox("_inputbox");

	return RUN_END;
}

//창 불투명도
int CScript::ComWindowOpacity(COMMAND_PTR it)
{
	if(it->value[0] >= 0 && it->value[0] <= 100)
	{
		m_dlg.SetOpacity((float)it->value[0] / 100);
		m_select.SetOpacity((float)it->value[0] / 100);
		m_input.SetOpacity((float)it->value[0] / 100);
	}
	else
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : 잘못된 투명도를 입력했습니다.\n", debug_fp);
	}

	return RUN_END;
}

//그림 읽기
int CScript::ComPicture(COMMAND_PTR it)
{
	//잘못된 ID 입력
	if(it->value[0] < 0)
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : 잘못된 그림번호를 입력했습니다.\n", debug_fp);

		return RUN_END;
	}

	bool load_first = false;	//맨 처음으로 그림을 읽어오는지 여부
	if(m_pic.find(it->value[0]) == m_pic.end())load_first=true;

	JPictureInfo pi;
	if(!jdd->GetPictureInfo(it->buffer, &pi))
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : 그림을 찾을 수 없습니다.\n", debug_fp);

		return RUN_END;
	}
	ScriptPicture* p_pic = &m_pic[it->value[0]];	//사용중인 그림 포인터
	p_pic->id = jdd->GetID(it->buffer);
	strcpy(p_pic->src, it->buffer);	//그림 이름 복사

	//그림 정보 저장해 두기
	p_pic->key = pi.GetColorKey();
	p_pic->opacity = pi.GetOpacity();
	p_pic->flip = 0;	//뒤집지 않은 상태가 기본

	//애니에션인 경우
	if(it->value[1] > 0)
	{
		int ani_x = pi.GetWidth() / (it->value[1]);	//프레임 수에 맞게 가로 길이 나누기
		int ani_y = pi.GetHeight();					//세로는 무조건 그림 크기에 맞춤

		//디폴트 지연시간
		it->value[2] = Max(it->value[2], 1);

		//애니메이션 형식 확인
		if(it->value[3] == 0)animation.CreateAnimation(p_pic->id, ani_x, ani_y, ani_loop, it->value[1], it->value[2]);
		else if(it->value[3] == 2)animation.CreateAnimation(p_pic->id, ani_x, ani_y, ani_once, it->value[1], it->value[2]);
		else animation.CreateAnimation(p_pic->id, ani_x, ani_y, ani_exchange, it->value[1], it->value[2]);

		p_pic->frame = it->value[1];
		p_pic->delay = it->value[2];
		p_pic->anitype = it->value[3];
		p_pic->ani=TRUE;
	}
	else p_pic->ani=FALSE;

	if(load_first)p_pic->show=FALSE;	//처음 읽어 올 경우에만 그림을 보이지 않는 것으로 초기화

	return RUN_END;
}

//그림 보이기
int CScript::ComShowPicture(COMMAND_PTR it)
{
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//그림이 존재하지 않음

	//기본 속성
	p_pic->show=TRUE;
	p_pic->x=it->value[1];
	p_pic->y=it->value[2];
	
	//투명도
	JPictureInfo pi;
	p_pic->opacity=((float)it->value[3])/100;
	pi.SetColorKey(p_pic->key);
	pi.SetOpacity(p_pic->opacity);
	jdd->SetPictureInfo(p_pic->id, &pi);

	//반복출력
	p_pic->loop = it->value[4];
	p_pic->gap_x = it->value[5];
	p_pic->gap_y = it->value[6];

	//애니메이션일 경우 되감기
	if(p_pic->ani)animation.GetAni(p_pic->id)->Reset();

	return RUN_END;
}

//그림 이동
int CScript::ComMovePicture(COMMAND_PTR it)
{
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//그림이 존재하지 않음

	//변화치 계산
	if(!it->first)
	{
		it->value[3] = Max(it->value[3], 1);	//지연시간은 최소 1
		m_move_x[it->value[0]] = (double)it->value[1] / (double)it->value[3];
		m_move_y[it->value[0]] = (double)it->value[2] / (double)it->value[3];
		p_pic->rx = (double)p_pic->x;
		p_pic->ry = (double)p_pic->y;
		it->first = true;
	}

	//이동
	if(it->value[3]>0)
	{
		p_pic->rx+=m_move_x[it->value[0]];
		p_pic->ry+=m_move_y[it->value[0]];
		p_pic->x = (int)p_pic->rx;
		p_pic->y = (int)p_pic->ry;

		--(it->value[3]);

		return RUN_NOTEND;
	}
	else return RUN_END;
}

//그림 숨기기
int CScript::ComHidePicture(COMMAND_PTR it)
{
	//그림 모두 숨기기(특수 명령)
	if(it->value[0] == -1)
	{
		std::map<int, ScriptPicture>::iterator git;
		for(git = m_pic.begin(); git != m_pic.end(); ++git)git->second.show = FALSE;
		
		return RUN_END;
	}

	//하나만 숨기기
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//그림이 존재하지 않음

	p_pic->show=FALSE;

	return RUN_END;
}

//그림 투명도
int CScript::ComPictureOpacity(COMMAND_PTR it)
{
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//그림이 존재하지 않음

	//변화치 계산
	static float change;
	if(!it->first)
	{
		it->value[2] = Max(it->value[2], 1);	//지연시간은 최소 1
		change = (((float)it->value[1]/100) - p_pic->opacity)/(float)it->value[2];
		it->first = true;
	}

	if(it->value[2]>0)
	{
		//투명도
		JPictureInfo pi;
		p_pic->opacity+=change;
		pi.SetOpacity(p_pic->opacity);
		pi.SetColorKey(p_pic->key);
		jdd->SetPictureInfo(p_pic->id, &pi);

		--it->value[2];

		return RUN_NOTEND;
	}
	else return RUN_END;
}

//상하 반전
int CScript::ComVFlip(COMMAND_PTR it)
{
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//그림이 존재하지 않음

	if(p_pic->flip & DPX_VFLIP)p_pic->flip &= (0xffff - DPX_VFLIP);
		else p_pic->flip |= DPX_VFLIP;
	
	return RUN_END;
}

//좌우 반전
int CScript::ComHFlip(COMMAND_PTR it)
{
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//그림이 존재하지 않음

	if(p_pic->flip & DPX_HFLIP)p_pic->flip &= (0xffff - DPX_HFLIP);
		else p_pic->flip |= DPX_HFLIP;

	return RUN_END;
}

//화면 효과
int CScript::ComScreenEffect(COMMAND_PTR it)
{
	//시작 전 계산할 작업
	static double now_screen, screen_change;
	if(!it->first)
	{
		//화면 효과 설정
		screen_effect[0]=it->value[0];
		screen_effect[1]=it->value[1];

		//변화치 계산
		it->value[3] = Max(it->value[3], 1);	//지연시간은 최소 1
		screen_change = (double)(it->value[2] - it->value[1])/(double)it->value[3];
		now_screen = (double)it->value[1];
		it->first = true;
	}

	//변화
	if(it->value[3]>0)
	{
		now_screen += screen_change;
		screen_effect[1] = (int)now_screen;

		--it->value[3];

		return RUN_NOTEND;
	}
	else return RUN_END;
}

//배경음 재생
int CScript::ComPlayBGM(COMMAND_PTR it)
{
	ComStopBGM(it);
	ProcessMessage();

	char ext[20];
	strcpy(ext, strrchr(it->buffer, '.'));
	ToSmallCase(ext);

	if(ext == NULL)
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : 올바른 음악파일 이름이 아닙니다.\n", debug_fp);
	}
	else if(strcmp(ext, ".mid") == 0)
	{
		if(!_MidiPlay(it->buffer))
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
			fputs(" : MIDI 파일을 연주할 수 없습니다.\n", debug_fp);
		}
	}
	else if(strcmp(ext, ".ogg") == 0)
	{
		if(!m_ogg || !m_ogg->OpenOgg(it->buffer))
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
			fputs(" : OGG 파일을 읽을 수 없습니다.\n", debug_fp);
		}
		else m_ogg->PlayOgg(true);
	}
	else
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : 지원하지 않는 음악 형식입니다.\n", debug_fp);
	}
	
	return RUN_END;
}

//배경음 정지
int CScript::ComStopBGM(COMMAND_PTR it)
{
	_MidiStop();
	if(m_ogg)m_ogg->StopOgg();

	return RUN_END;
}

//사운드 재생
int CScript::ComPlaySound(COMMAND_PTR it)
{
	_Play(m_snd[it->value[0]]);

	return RUN_END;
}

//AVI 재생
int CScript::ComAVI(COMMAND_PTR it)
{
	_PlayAVI(it->buffer);

	return RUN_END;
}

//랜덤 변수 발생
int CScript::ComRandom(COMMAND_PTR it)
{
	(*m_valuemap)[HASH(it->buffer)] = (rand()%(abs(it->value[1] - it->value[0]))+1) + it->value[0];

	return RUN_END;
}

//대기
int CScript::ComWait(COMMAND_PTR it)
{
	if(it->value[0]>0)
	{
		--(it->value[0]);
		return RUN_NOTEND;
	}
	else return RUN_END;
}

//선택지에 따른 이동
int CScript::ComCase(COMMAND_PTR it)
{
	if(select_no < SCRIPT_VALUE_MAX && it->value[select_no] >= 0)
	{
		Goto(it->value[select_no], script_no);
	}

	return RUN_END;
}

//이동
int CScript::ComGoto(COMMAND_PTR it)
{
	//마이너스 값은 무시
	if(it->value[0] >= 0)Goto(it->value[0], script_no);

	return RUN_END;
}

//돌아가기
int CScript::ComReturn(COMMAND_PTR it)
{
	//벡터에서 돌아갈 주소를 마지막에서 가져옴
	if(!return_no.empty())
	{
		std::vector<int>::iterator rit = return_no.end();
		rit--;
		script_no = (*rit);

		//가져오고 지움
		return_no.pop_back();
	}

	return RUN_END;
}

//계속 대기
int CScript::ComEnd(COMMAND_PTR it)
{
	return SCRIPT_END;
}