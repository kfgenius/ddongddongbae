//2007. 3. 5 - 스크립트는 개인에 맞게 고치기 위해서 따로 떼어냈음
#include <windows.h>

#include <string>

#include "DScript.h"
#include "Game.h"
#include "calc.h"

//명령문
#define COMMAND_PRINT			1
#define COMMAND_SELECT			2
#define COMMAND_INPUTNUM		3
#define COMMAND_INPUTTEXT		4
#define COMMAND_SHOWTEXT		5
#define COMMAND_HIDETTEXT		6
#define COMMAND_MOVETEXT		7
#define COMMAND_TEXTCOLOR		8
#define COMMAND_SHOWSHADOW		9
#define COMMAND_HIDESHADOW		10
#define COMMAND_FONT			11

#define COMMAND_WINDOW			100
#define COMMAND_WINDOWOPACITY	103

#define COMMAND_PICTURE			200
#define COMMAND_SHOWPICTURE		201
#define COMMAND_MOVEPICTURE		202
#define COMMAND_HIDEPICTURE		203
#define COMMAND_PICTUREOPACITY	204
#define COMMAND_HFLIP			205
#define COMMAND_VFLIP			206
#define COMMAND_SCREENEFFECT	207

#define COMMAND_MUSICPLAY		301
#define COMMAND_MUSICSTOP		302
#define COMMAND_EFFECT			310
#define COMMAND_EFFECTPLAY		311
#define COMMAND_AVIPLAY			323

#define COMMAND_KEY_A			400
#define COMMAND_KEY_S			401
#define COMMAND_KEY_D			402
#define COMMAND_KEY_F			403
#define COMMAND_KEY_UP			404
#define COMMAND_KEY_DOWN		405
#define COMMAND_KEY_LEFT		406
#define COMMAND_KEY_RIGHT		407
#define COMMAND_KEY_ENTER		408

#define COMMAND_RANDOM			500

#define COMMAND_WAIT			901
#define COMMAND_CASE			902
#define COMMAND_CASESUB			903
#define COMMAND_GOTO			904
#define COMMAND_GOSUB			905
#define COMMAND_RETURN			906
#define COMMAND_LOAD			907
#define COMMAND_LOADMENU		908
#define COMMAND_SAVEMENU		909
#define COMMAND_CHAPTER			910
#define COMMAND_ENABLESAVE		911
#define COMMAND_DISABLESAVE		912
#define COMMAND_SELECTAREA		913
#define COMMAND_GOTOAREA		914

#define COMMAND_END				900

//화면 효과
#define SE_FADE					0
#define SE_WHITE				1
#define SE_GRAY					2
#define SE_HUE					3
#define SE_RED					4
#define SE_GREEN				5
#define SE_BLUE					6

//스크립트에서 변수를 읽어들이는 모드
#define READ_NUM				0
#define READ_VAR				1
#define READ_TEXT				2
#define READ_POINT				3

//음악 타입
#define MUSIC_MID				1
#define MUSIC_WAV				2
#define MUSIC_OGG				3

//시스템 사운드
#define CURSOR_SOUND			0
#define SELECT_SOUND			1

//간략화를 위한 매크로
#define DELETE_COMMAND			it->id=-1	//명령문 폐기

//해쉬 함수(문라이브 것 가져옴)
#define HASH_NUMBER 37
#define HASH_SIZE 262139

// 전역 변수
HSNDOBJ sound[2];				//시스템 사운드
map<int, int> m_valuemap;		//숫자 변수
map<int, ScriptText> m_textmap;	//문자 변수

//전역 함수
void ClearValueMap()	//변수 맵 클리어
{
	m_valuemap.clear();
	m_textmap.clear();
}

static int hash(char* sz)	//해쉬값 구하기
{
	if(sz==NULL) return 0;
	unsigned int hashvalue=0;
	for(int lp=0;sz[lp];lp++)
		hashvalue=(hashvalue*HASH_NUMBER+sz[lp])%HASH_SIZE;
	return hashvalue;
}

//스크립트 명령 메소드
CScriptCommand::CScriptCommand()
{
	//값 초기화
	ZeroMemory(value, sizeof(int)*SCRIPT_VALUE_MAX);
	text=NULL;
	strcpy(buffer,"");
	first=false;
}

//변수가 섞였는지 검사
BOOL IncludeValue(char* str)
{
	if(strchr(str, '[')!=NULL || strchr(str, '{')!=NULL || strchr(str, '|')!=NULL)return TRUE;
		else return FALSE;
}

#define KEY1 8			//스크립트 파일 해독을 위한 키1
#define KEY2 4			//스크립트 파일 해독을 위한 키2

/////////////////////////////////////////////////////
//스크립트 메소드
int CScript::get_char_num(char chr)
{
	char CodeTo[76]="dyKp:8jP;[R^FIqN0WsTX4U`a52Z]1HnbuVvlh76\\ti=SArJ@>Q_wgkoB?LCcYD3<M9EmGxeOfz";

	for(int i=0; i<76; ++i)
		if(CodeTo[i]==chr)return i;

	return -1;
}

char CScript::DlgDecode(char b1, char b2)
{	
	b1=get_char_num(b1)-KEY1;
	b2=get_char_num(b2)-30-KEY2;

	return b1*16+b2;
}

//대화 파일 불러오기
CScript::CScript(char* script_file)
{	
	Load(script_file);
	if(!enable)return;	//초기화 실패시 취소

	CreateCommandMap();

	//창 초기화
	m_dlg.SetDlg(5,395,630,4);
	m_select.SetDlg(5, 395, 630, 4);

	//사운드
	m_ogg = new COgg(SoundOBJ);
	sound[CURSOR_SOUND] = SndObjCreate(SoundOBJ, "Sound\\Cursor.wav", 2);
	sound[SELECT_SOUND] = SndObjCreate(SoundOBJ, "Sound\\Select.wav", 2);

	//DirectMusic
	m_music.InitDirectMusic();

	//기타
	debug_mode = false;	
	text_rgb[0]=text_rgb[1]=text_rgb[2]=0;
	text_shadow=false;
	m_input.SetDlg(250,230,140);
}

//대화내용 비우기
CScript::~CScript()
{
	//디버그 파일 닫기
	if(debug_mode)fclose(debug_fp);

	//스크립트 파일 언로드
	UnLoad();

	//명령문 제거
	m_script.clear();

	//명령문 맵 해체
	m_CommandMap.clear();

	//음악 플레이어 제거
	delete m_ogg;

	//DScript가 jdd를 독점할 때만
	jdd->Cleanup();
	delete jdd;
	jdd=NULL;
}

void CScript::Load(char* script_file)
{
	char tmp_file[256];
	//dlg로 읽기
	sprintf(tmp_file, "%s.dlg", script_file);
	if(fp=fopen(tmp_file, "rb"))
	{
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
		strcpy(this->script_file, script_file);
		if(fp=fopen(tmp_file, "rb"))
		{
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

	//그림 자료 읽어오기
	sprintf(mlc_file, "%s.mlc", script_file);
	if(!jre->LoadResource(mlc_file))
	{
		DebugMode();
		fprintf(debug_fp, "%s 파일을 찾을 수 없습니다.\n", mlc_file);
	}

	//챕터명 기억
	char* temp_chapter = strrchr(script_file, '\\');
	if(temp_chapter == NULL)strcpy(chapter_name, script_file);
		else strcpy(chapter_name, temp_chapter+1);

	//화면효과 초기화
	screen_effect[0] = 0;
	screen_effect[1] = 0;

	//음악 정보 초기화
	strcpy(bgm_name, "");

	//기타
	enable_save = true;
	on_savemenu = on_loadmenu = false;
	area_select = false;
	area_x = SCREEN_X/2;
	area_y = SCREEN_Y/2;
	music_type = -1;
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
	for(int i=0; i<snrs; ++i)
		if(snr[i]!=NULL)
		{
			delete[] snr[i];
			snr[i]=NULL;
		}

	//책갈피 해체
	m_BookmarkHash.clear();

	//픽쳐 해체
	std::map<int, ScriptPicture>::iterator it;
	for(it = m_pic.begin(); it!=m_pic.end(); ++it)
	{
		jdd->DeleteSurface((it->second).id);
	}
	m_pic.clear();

	//문자 표시 정보 초기화
	std::map<int, ScriptText>::iterator tit;
	for(tit = m_textmap.begin(); tit!=m_textmap.end(); ++tit)
	{
		(tit->second).show = false;
		(tit->second).shadow = false;
		(tit->second).color = JColor(0,0,0);
	}

	//사운드 해체
	std::map<int, ScriptSound>::iterator it2;
	for(it2 = m_snd.begin(); it2!=m_snd.end(); ++it2)
	{
		SndObjDestroy((it2->second).sound);
		(it2->second).sound=NULL;
	}
	m_snd.clear();

	//리턴 지점 비우기
	return_no.clear();

	//그림 제거
	jre->UnloadResource(mlc_file);

	//화면효과 초기화
	screen_effect[0] = 0;
	screen_effect[1] = 0;

	//음악 소리 제거
	/*if(music_type == MUSIC_MID)_MidiStop();
		else if(music_type == MUSIC_WAV)m_music.StopAudio();
		else if(music_type == MUSIC_OGG)m_ogg->StopOgg();*/
	music_type = -1;
	_MidiStop();
	m_music.StopAudio();
	m_ogg->StopOgg();

	enable=FALSE;
}

#define SYSTEMMENU_MAX	8

void CScript::SystemMenu(bool save)
{
	//세이브 가능한 상황인지 확인
	if(save)
	{
		std::vector<CScriptCommand>::iterator it;
		for(it=m_script.begin(); it!=m_script.end(); ++it)
		{
			if(it->id<0 || it->id==COMMAND_PRINT || it->id>=COMMAND_KEY_A && it->id<=COMMAND_KEY_ENTER);	//상관없는 명령어
			else return;	//아직 안 끝난 명령이 있으면 불가능
		}
	}

	//메뉴 설정
	int selected_menu = 0;
	char menu_name[SYSTEMMENU_MAX][80];
	char save_filename[80];

	if(!enable_save && save)selected_menu=SYSTEMMENU_MAX-2;	//세이브 불가능일 경우

	strcpy(menu_name[SYSTEMMENU_MAX-1], "게임 종료");
	strcpy(menu_name[SYSTEMMENU_MAX-2], "타이틀로");

	//세이브 파일 정보 얻기
	for(int i=0; i<SYSTEMMENU_MAX-2; ++i)
	{
		FILE *info_fp;
		sprintf(save_filename, "Save\\Save%d.sav", i+1);
		if(info_fp = fopen(save_filename, "rb"))
		{
			//흥크립트 세이브 파일인지 확인
			char header[9];
			fread(header, sizeof(char), 8, info_fp);
			header[8]=NULL;
			//챕터 이름 읽기
			if(strcmp(header, "HungSave")==0)
			{
				fseek(info_fp, 80, SEEK_CUR);
				fread(menu_name[i], sizeof(char), 80, info_fp);
			}
			else strcpy(menu_name[i], "빈 세이브 파일");
			
			fclose(info_fp);
		}
		else strcpy(menu_name[i], "빈 세이브 파일");
	}
	
	//시스템 메뉴
	while(1)
	{
		if(!ProcessMessage())break;
		if(GetKey(vkey_esc))break;

		//이동
		int old_pos = selected_menu;
		if(GetKey(vkey_up, 10))--selected_menu;
			else if(GetKey(vkey_down, 10))++selected_menu;

		if(save)
		{
			if(enable_save)selected_menu = MaxMin(selected_menu, 0, SYSTEMMENU_MAX-1);
				else selected_menu = MaxMin(selected_menu, SYSTEMMENU_MAX-2, SYSTEMMENU_MAX-1);
		}
		else selected_menu = MaxMin(selected_menu, 0, SYSTEMMENU_MAX-3);

		if(old_pos != selected_menu)_Play(sound[CURSOR_SOUND]);	//효과음

		//선택
		if(GetKey(vkey_enter))
		{
			//게임 종료
			if(selected_menu == SYSTEMMENU_MAX-1)
			{
				script_end = true;				
				break;
			}
			//타이틀로
			else if(selected_menu == SYSTEMMENU_MAX-2)
			{
				_Play(sound[SELECT_SOUND]);	//효과음

				m_script.clear();
				UnLoad();
				Load("MAIN");
				script_no = 0;				
				break;
			}
			//세이브
			else if(save)
			{
				FILE *save_fp;
				sprintf(save_filename, "Save\\Save%d.sav", selected_menu+1);
				if(save_fp = fopen(save_filename, "wb"))
				{
					_Play(sound[SELECT_SOUND]);	//효과음

					//헤더
					fwrite("HungSave", sizeof(char), 8, save_fp);
					fwrite(script_file, sizeof(char), 80, save_fp);
					fwrite(chapter_name, sizeof(char), 80, save_fp);
					//전역 변수 기록
					fwrite(&script_no, sizeof(int), 1, save_fp);
					fwrite(&text_rgb, sizeof(int), 3, save_fp);
					fwrite(&text_shadow, sizeof(bool), 1, save_fp);
					size_t size=return_no.size();
					fwrite(&size, sizeof(int), 1, save_fp);
					std::vector<int>::iterator rit;
					for(rit=return_no.begin(); rit!=return_no.end(); ++rit)
					{
						fwrite(&(*rit), sizeof(int), 1, save_fp);
					}
					//음악 재생 정보 기록
					fwrite(bgm_name, sizeof(char), 80, save_fp);
					//명령문 기록
					size=m_script.size();
					fwrite(&size, sizeof(int), 1, save_fp);
					std::vector<CScriptCommand>::iterator cit;
					for(cit=m_script.begin(); cit!=m_script.end(); ++cit)
					{
						fwrite(&(*cit), sizeof(CScriptCommand), 1, save_fp);
					}
					//그림 기록
					size=m_pic.size();
					fwrite(&size, sizeof(int), 1, save_fp);
					std::map<int, ScriptPicture>::iterator pit;
					for(pit=m_pic.begin(); pit!=m_pic.end(); ++pit)
					{
						fwrite(&pit->first, sizeof(int), 1, save_fp);
						fwrite(&pit->second, sizeof(ScriptPicture), 1, save_fp);
					}
					//소리 기록
					size=m_snd.size();
					fwrite(&size, sizeof(int), 1, save_fp);
					std::map<int, ScriptSound>::iterator sit;
					for(sit=m_snd.begin(); sit!=m_snd.end(); ++sit)
					{
						fwrite(&sit->first, sizeof(int), 1, save_fp);
						fwrite(&sit->second, sizeof(ScriptSound), 1, save_fp);
					}
					//변수 기록
					size=m_valuemap.size();
					fwrite(&size, sizeof(int), 1, save_fp);
					std::map<int, int>::iterator vit;
					for(vit=m_valuemap.begin(); vit!=m_valuemap.end(); ++vit)
					{
						fwrite(&vit->first, sizeof(int), 1, save_fp);
						fwrite(&vit->second, sizeof(int), 1, save_fp);
					}
					//문자 변수 기록
					size=m_textmap.size();
					fwrite(&size, sizeof(int), 1, save_fp);
					std::map<int, ScriptText>::iterator tit;
					for(tit=m_textmap.begin(); tit!=m_textmap.end(); ++tit)
					{
						fwrite(&tit->first, sizeof(int), 1, save_fp);
						fwrite(&tit->second, sizeof(ScriptText), 1, save_fp);
					}

					fclose(save_fp);

					//기록후 메뉴이름 변경
					strcpy(menu_name[selected_menu], chapter_name);
				}
			}
			//로드
			else
			{
				FILE *load_fp;
				sprintf(save_filename, "Save\\Save%d.sav", selected_menu+1);
				if(load_fp = fopen(save_filename, "rb"))
				{
					//헤더
					char header[9];
					fread(header, sizeof(char), 8, load_fp);
					header[8] = NULL;
					if(strcmp(header, "HungSave")==0)
					{
						_Play(sound[SELECT_SOUND]);	//효과음

						//지금의 스크립트를 언로드하고 기록된 스크립트 읽기
						fread(script_file, sizeof(char), 80, load_fp);
						m_script.clear();
						UnLoad();
						Load(script_file);
						fread(chapter_name, sizeof(char), 80, load_fp);
						//전역 변수 읽기
						fread(&script_no, sizeof(int), 1, load_fp);
						fread(&text_rgb, sizeof(int), 3, load_fp);
						fread(&text_shadow, sizeof(bool), 1, load_fp);
						size_t size;
						fread(&size, sizeof(int), 1, load_fp);
						for(size_t i=0; i<size; ++i)
						{
							int no;
							fread(&no, sizeof(int), 1, load_fp);
							return_no.push_back(no);
						}
						//글자색, 그림자 복원
						m_dlg.SetColor(text_rgb[0], text_rgb[1], text_rgb[2]);
						m_select.SetColor(text_rgb[0], text_rgb[1], text_rgb[2]);
						m_input.SetColor(text_rgb[0], text_rgb[1], text_rgb[2]);
						m_dlg.ShowShadow(text_shadow);
						m_select.ShowShadow(text_shadow);
						m_input.ShowShadow(text_shadow);
						//음악 재생 정보 읽기
						fread(bgm_name, sizeof(char), 80, load_fp);
						if(strlen(bgm_name) > 0)
						{
							char ext[20];
							strcpy(ext, strrchr(bgm_name, '.'));

							if(strcmp(ext, ".mid") == 0)
							{
								_MidiPlay(bgm_name);
								music_type = MUSIC_MID;
							}
							else if(strcmp(ext, ".wav") == 0)
							{
								m_music.LoadSegmentFile(bgm_name);
								m_music.PlayAudio();
								music_type = MUSIC_WAV;
							}
							else if(strcmp(ext, ".ogg") == 0)
							{
								m_ogg->OpenOgg(bgm_name);
								m_ogg->PlayOgg(true);
								music_type = MUSIC_OGG;
							}
						}
						//명령문 읽기
						fread(&size, sizeof(int), 1, load_fp);
						for(size_t i=0; i<size; ++i)
						{
							CScriptCommand tmp_command;
							fread(&tmp_command, sizeof(CScriptCommand), 1, load_fp);

							//출력문의 경우 상태 돌리기
							if(tmp_command.id == COMMAND_PRINT)
							{
								tmp_command.first=false;
								if(snr[tmp_command.snr_no][0]=='@' || snr[tmp_command.snr_no][0]=='~')
								{
									tmp_command.text = strchr(snr[tmp_command.snr_no], ' ') + 1;
								}
								else tmp_command.text = snr[tmp_command.snr_no];
							}
							//0이하의 찌꺼기 명령은 무시
							if(tmp_command.id>=0)m_script.push_back(tmp_command);
						}
						//그림 읽기
						fread(&size, sizeof(int), 1, load_fp);
						for(size_t i=0; i<size; ++i)
						{
							int tmp_id, pid;
							ScriptPicture tmp_pic;
							fread(&tmp_id, sizeof(int), 1, load_fp);
							fread(&tmp_pic, sizeof(ScriptPicture), 1, load_fp);
							m_pic[tmp_id] = tmp_pic;

							//그림 복원
							char* pic_name=StrAdd("ScriptPicture%d", tmp_id);
							jdd->DeleteSurface(pic_name);
							if(!jdd->DuplicateSurface(pic_name, m_pic[tmp_id].src, &pid))continue;

							//투명도 복원
							JPictureInfo pi;
							pi.SetOpacity(m_pic[tmp_id].opacity);
							pi.SetColorKey(m_pic[tmp_id].key);
							jdd->SetPictureInfo(pic_name, &pi);

							//애니에션인 경우
							if(m_pic[tmp_id].ani)
							{
								JPictureInfo pi;
								jdd->GetPictureInfo(m_pic[tmp_id].src, &pi);

								int ani_x = pi.GetWidth() / m_pic[tmp_id].frame;	//프레임 수에 맞게 가로 길이 나누기
								int ani_y = pi.GetHeight();							//세로는 무조건 그림 크기에 맞춤

								if(m_pic[tmp_id].anitype == 0)animation.CreateAnimation(m_pic[tmp_id].id, ani_x, ani_y, ani_loop, m_pic[tmp_id].frame, m_pic[tmp_id].delay);
								else if(m_pic[tmp_id].anitype == 2)animation.CreateAnimation(m_pic[tmp_id].id, ani_x, ani_y, ani_once, m_pic[tmp_id].frame, m_pic[tmp_id].delay);
								else animation.CreateAnimation(m_pic[tmp_id].id, ani_x, ani_y, ani_exchange, m_pic[tmp_id].frame, m_pic[tmp_id].delay);
							}
						}
						//소리 읽기
						fread(&size, sizeof(int), 1, load_fp);
						for(size_t i=0; i<size; ++i)
						{
							int tmp_id;
							ScriptSound tmp_sound;
							fread(&tmp_id, sizeof(int), 1, load_fp);
							fread(&tmp_sound, sizeof(ScriptSound), 1, load_fp);
							tmp_sound.sound = SndObjCreate(SoundOBJ, tmp_sound.src, 2);			
							m_snd[tmp_id]=tmp_sound;
						}
						//변수 읽기
						fread(&size, sizeof(int), 1, load_fp);
						for(size_t i=0; i<size; ++i)
						{
							int tmp_val[2];
							fread(&tmp_val, sizeof(int), 2, load_fp);
							m_valuemap[tmp_val[0]]=tmp_val[1];
						}
						//문자 변수 읽기
						fread(&size, sizeof(int), 1, load_fp);
						for(size_t i=0; i<size; ++i)
						{
							int tmp_id;
							ScriptText tmp_text;
							fread(&tmp_id, sizeof(int), 1, load_fp);
							fread(&tmp_text, sizeof(ScriptText), 1, load_fp);

							m_textmap[tmp_id] = tmp_text;
						}

						return;
					}
	
					fclose(load_fp);
				}
			}
		}

		//출력 부분
		if(save)jdd->DrawPicture(backbuffer, "SystemSave", 0, 0, NULL);
			else jdd->DrawPicture(backbuffer, "SystemLoad", 0, 0, NULL);
		int put_max = (save)?SYSTEMMENU_MAX:SYSTEMMENU_MAX-2;
		for(int i=0; i<put_max; ++i)
		{
			if(save && !enable_save && i<SYSTEMMENU_MAX-2)continue;		//세이브 불가능일 땐 보여주지 않음

			int menu_y = i*50+60;

			jdd->DrawPicture(backbuffer, "SystemMenu", 128, menu_y, NULL);
			if(selected_menu == i)jdd->DrawPicture(backbuffer, "SystemCursor", 128, menu_y, NULL);

			//파일 정보
			int center=(int)(640-(strlen(menu_name[i])*10))/2;
			jdd->DrawText(backbuffer, menu_name[i], global_font, center, menu_y+10, JColor(0,0,0));
		}		

		jdd->Render();
	}
}

//시스템 변수 얻기
void CScript::GetSystemValue(char* dest, char* src)
{
	char buffer[40];

	if(strcmp(src, "커서X") == 0)
	{
		_itoa_s(area_x, buffer, 40, 10);
		strcpy(dest, buffer);
	}
	else if(strcmp(src, "커서Y") == 0)
	{
		_itoa_s(area_y, buffer, 40, 10);
		strcpy(dest, buffer);
	}
	else if(strcmp(src, "파일명") == 0)
	{
		strcpy(dest, script_file);
	}
	else if(strcmp(src, "선택") == 0)
	{
		_itoa_s(select_value, buffer, 40, 10);
		strcpy(dest, buffer);
	}
	else if(strcmp(src, "챕터명") == 0)
	{
		strcpy(dest, chapter_name);
	}
	else if(strcmp(src, "배경음") == 0)
	{
		strcpy(dest, bgm_name);
	}
	else if(strcmp(src, "커서명") == 0)
	{
		strcpy(dest, area_cursor);
	}
}

//스크립트 처리
void CScript::Script(int no)
{
	if(!enable)return;	//스크립트 파일이 제대로 로딩되지 않았을 때

	if(m_BookmarkHash.find(no) == m_BookmarkHash.end())	//못 찾았을 때
	{
		script_end=true;		
	}
	else
	{
		script_no=m_BookmarkHash[no];	//스크립트 시작점
		script_end = false;
	}

	//스크립트 실행
	while(!script_end)
	{
		BringScript();
		if(script_no < 0)break;	//에러

		while(!m_script.empty() && !script_end)
		{
			if(!ProcessMessage())break;

			RunScript();	//스크립트 처리

			//화면 효과
			if(screen_effect[1] >= 0)
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
			jdd->Render();

			//로드 메뉴
			if(on_loadmenu)
			{
				SystemMenu(false);
				on_loadmenu=false;
			}
			//세이브 메뉴
			else if(GetKey(vkey_esc) || on_savemenu)
			{
				SystemMenu();
				on_savemenu=false;
			}
		}
		
		//스크립트 끝 or 에러 검사
		if(script_no < 0 || script_no >= snrs || !enable)break;
	}
}

void CScript::BringScript()
{
	//스크립트 파일이 제대로 로딩되지 않았을 때
	if(!enable)
	{
		script_no = -1;
		return;
	}

	BOOL read_next;	//계속해서 다음 명령을 읽는지 여부

	do{
		read_next=ReadScript(&m_script, script_no);
		++script_no;
	}while(read_next);
}

//책갈피가 꽂힌 것인지 확인
BOOL CScript::IsBookMark(int no)
{
	std::map<int,int>::iterator it;
	for(it=m_BookmarkHash.begin(); it!=m_BookmarkHash.end(); ++it)
	{
		if(no==it->second)return TRUE;
	}

	return FALSE;
}

//명령문 맵 생성
void CScript::CreateCommandMap()
{
	//텍스트 관련 명령문
	m_CommandMap[hash("출력")]=COMMAND_PRINT;
	m_CommandMap[hash("선택")]=COMMAND_SELECT;
	m_CommandMap[hash("숫자입력")]=COMMAND_INPUTNUM;
	m_CommandMap[hash("글자입력")]=COMMAND_INPUTTEXT;
	m_CommandMap[hash("글자표시")]=COMMAND_SHOWTEXT;
	m_CommandMap[hash("글자숨김")]=COMMAND_HIDETTEXT;
	m_CommandMap[hash("글자이동")]=COMMAND_MOVETEXT;

	//텍스트 설정 명령문
	m_CommandMap[hash("대화창")]=COMMAND_WINDOW;
	m_CommandMap[hash("창불투명도")]=COMMAND_WINDOWOPACITY;
	m_CommandMap[hash("글자색")]=COMMAND_TEXTCOLOR;
	m_CommandMap[hash("그림자표시")]=COMMAND_SHOWSHADOW;
	m_CommandMap[hash("그림자숨김")]=COMMAND_HIDESHADOW;
	m_CommandMap[hash("글꼴")]=COMMAND_FONT;

	//그림 관련 명령문
	m_CommandMap[hash("그림")]=COMMAND_PICTURE;
	m_CommandMap[hash("그림표시")]=COMMAND_SHOWPICTURE;
	m_CommandMap[hash("그림이동")]=COMMAND_MOVEPICTURE;
	m_CommandMap[hash("그림숨김")]=COMMAND_HIDEPICTURE;
	m_CommandMap[hash("그림불투명도")]=COMMAND_PICTUREOPACITY;
	m_CommandMap[hash("좌우반전")]=COMMAND_HFLIP;
	m_CommandMap[hash("상하반전")]=COMMAND_VFLIP;
	m_CommandMap[hash("화면효과")]=COMMAND_SCREENEFFECT;

	//사운드 관련 명령문
	m_CommandMap[hash("음악재생")]=COMMAND_MUSICPLAY;
	m_CommandMap[hash("음악정지")]=COMMAND_MUSICSTOP;
	m_CommandMap[hash("소리")]=COMMAND_EFFECT;
	m_CommandMap[hash("소리재생")]=COMMAND_EFFECTPLAY;
	m_CommandMap[hash("AVI")]=COMMAND_AVIPLAY;

	//키입력 명령문
	m_CommandMap[hash("A키")]=COMMAND_KEY_A;
	m_CommandMap[hash("S키")]=COMMAND_KEY_S;
	m_CommandMap[hash("D키")]=COMMAND_KEY_D;
	m_CommandMap[hash("F키")]=COMMAND_KEY_F;
	m_CommandMap[hash("상키")]=COMMAND_KEY_UP;
	m_CommandMap[hash("하키")]=COMMAND_KEY_DOWN;
	m_CommandMap[hash("좌키")]=COMMAND_KEY_LEFT;
	m_CommandMap[hash("우키")]=COMMAND_KEY_RIGHT;
	m_CommandMap[hash("엔터키")]=COMMAND_KEY_ENTER;

	//연산
	m_CommandMap[hash("랜덤")]=COMMAND_RANDOM;

	//스크립트 흐름 제어
	m_CommandMap[hash("대기")]=COMMAND_WAIT;
	m_CommandMap[hash("분기")]=COMMAND_CASE;
	m_CommandMap[hash("이동")]=COMMAND_GOTO;
	m_CommandMap[hash("돌아감")]=COMMAND_RETURN;
	m_CommandMap[hash("로드")]=COMMAND_LOAD;
	m_CommandMap[hash("로드메뉴")]=COMMAND_LOADMENU;
	m_CommandMap[hash("세이브메뉴")]=COMMAND_SAVEMENU;
	m_CommandMap[hash("챕터")]=COMMAND_CHAPTER;
	m_CommandMap[hash("세이브가능")]=COMMAND_ENABLESAVE;
	m_CommandMap[hash("세이브불가")]=COMMAND_DISABLESAVE;
	m_CommandMap[hash("영역선택")]=COMMAND_SELECTAREA;
	m_CommandMap[hash("영역분기")]=COMMAND_GOTOAREA;

	m_CommandMap[hash("끝")]=COMMAND_END;
}

int CScript::GetCommandID(int idx)
{
	return m_CommandMap[idx];
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
			_itoa_s(m_valuemap[hash(buffer)], num_buffer, 40, 10);
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
			strcat(result, m_textmap[hash(buffer)].buffer);
			cp = strlen(result);
		}
		//시스템 변수
		else if(source[i] == '|')
		{
			//변수 부분을 읽어 들임
			size_t start_point=++i;
			for(; i < strmax; ++i)
			{
				if(source[i]=='|')break;

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

			//시스템 변수와 합치기
			GetSystemValue(buffer, buffer);

			result[cp]=NULL;
			strcat(result, buffer);
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
BOOL CScript::ReadScript(vector<CScriptCommand> *m_script, int no)	//리턴하는 값은 계속해서 스크립트를 읽느냐 하는 것
{
	CScriptCommand temp_command;
	BOOL read_next=FALSE;
	char buffer[80];	//다용도 버퍼

	temp_command.snr_no = no;

	//변수 설정
	if(snr[no][0]=='[')
	{
		//변수 이름 읽어들이기
		int i;
		for(i=1; snr[no][i]!=']' && snr[no][i] != NULL; ++i)buffer[i-1] = snr[no][i];
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

		if(IncludeValue(&snr[no][i+1]))	//다른 변수가 섞여 있을 때
		{
			ConvertString(text_buffer, &snr[no][i+1]);
			calc = text_buffer;
		}
		else	//단순 계산식일 때
		{
			calc = strchr(snr[no], ' ');
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
				fprintf(debug_fp, "%s(%d) - %s", script_file, no, snr[no]);
				fputs("잘못된 계산식입니다.\n", debug_fp);
			}
			else
			{
				px=ev.Evaluate(vr, vals);
				if(px)
				{
					DebugMode();
					fprintf(debug_fp, "%s(%d) - %s", script_file, no, snr[no]);
					fputs("잘못된 계산식입니다.\n", debug_fp);
				}
				else m_valuemap[hash(buffer)] = (int)vr;
			}
		}
		else if(debug_mode)
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, no, snr[no]);
			fputs("계산식이 존재하지 않습니다.\n", debug_fp);
		}

		return TRUE;
	}
	//문자 변수 설정
	else if(snr[no][0]=='{')
	{
		//문자 변수 이름 읽어들이기
		int i;
		for(i=1; snr[no][i]!='}' && snr[no][i] != NULL; ++i)buffer[i-1] = snr[no][i];
		buffer[i-1] = NULL;

		//변수 이름 안에 변수가 있을 경우
		if(IncludeValue(buffer))
		{
			char tmp_buffer[80];
			strcpy(tmp_buffer, buffer);
			ConvertString(buffer, tmp_buffer);
		}

		//내용 읽기
		char* start_point = strchr(snr[no], '"');
		
		if(start_point == NULL)	//잘못된 변수 대입
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, no, snr[no]);
			fputs(" : \"\"안에 내용을 적으십시오.\n", debug_fp);
			return TRUE;	
		}

		int value_length=(int)(strrchr(snr[no], '"') - start_point) - 1;

		if(value_length > 0)
		{
			//문자 복사
			char tmp_buffer[80];
			strncpy(tmp_buffer, start_point+1, value_length);
			tmp_buffer[value_length] = NULL;

			if(IncludeValue(tmp_buffer))	//변수가 섞여 있을 때
			{
				char tmp_buffer2[80];
				strcpy(tmp_buffer2, tmp_buffer);
				ConvertString(tmp_buffer, tmp_buffer2);
			}

			//디폴트 값
			int text_id = hash(buffer);
			if(m_textmap.find(text_id)==m_textmap.end())
			{
				m_textmap[text_id].show = false;
				m_textmap[text_id].shadow = false;
				m_textmap[text_id].color = JColor(0,0,0);
			}

			strcpy(m_textmap[text_id].buffer, tmp_buffer);
		}
		else	// "가 하나만 있음
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, no, snr[no]);
			fputs(" : \"\"안에 내용을 적으십시오.\n", debug_fp);
			return TRUE;
		}

		return TRUE;
	}
	//일반 대화
	else if(snr[no][0]!='@' && snr[no][0]!='~')
	{
		temp_command.id=COMMAND_PRINT;
		
		//변수가 있어 변환이 필요할 때는 버퍼사용, 아니면 포인터 사용
		if(IncludeValue(snr[no]))
		{
			ConvertString(temp_command.buffer, snr[no]);
		}
		else
		{
			temp_command.text=snr[no];
		}

		m_script->push_back(temp_command);
		return FALSE;
	}

	//다음 명령을 계속 읽는지 여부
	if(snr[no][0]=='~')read_next=TRUE;

	//명령을 읽어 들이기
	int value_id=0;		//상수일 경우 몇번째인지 확인
	size_t i;			//현재 읽고 있는 위치
	size_t strmax = strlen(snr[no]);	//문자열 크기
	for(i=1; i < strmax; ++i)
	{
		if(snr[no][i]==' ' || snr[no][i]==0x09)break;

		buffer[i-1]=snr[no][i];	//내용 복사
	}

	//명령어의 ID 알아내기
	buffer[i-1]=NULL;
	temp_command.id=GetCommandID(hash(buffer));

	//출력문 경우 무조건 출력
	if(temp_command.id == COMMAND_PRINT)
	{
		temp_command.text=&snr[no][i+1];
		m_script->push_back(temp_command);
		return FALSE;
	}

	//값 읽어 들이기
	for(; i < strmax; ++i)
	{
		//상수 읽기
		if(snr[no][i]>='0' && snr[no][i]<='9' || snr[no][i]=='-')
		{
			int value_length=1;
			char* start_point = &snr[no][i++];

			for(; i < strmax && snr[no][i]>='0' && snr[no][i]<='9'; ++i)++value_length;	//길이 계산

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
		else if(snr[no][i]=='"')
		{
			int value_length=0;
			char* start_point = &snr[no][++i];

			for(;  i < strmax && snr[no][i]!='"'; ++i)++value_length;	//길이 계산

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
		else if(snr[no][i]=='[')
		{
			int value_length=0;
			//i++;
			char* start_point = &snr[no][++i];

			for(; i < strmax && snr[no][i]!=']'; ++i)++value_length;	//길이 계산

			//변수 이름 복사
			strncpy(buffer, start_point, value_length);
			buffer[value_length] = NULL;

			//변수 값을 찾아서 넘김
			if(value_id < SCRIPT_VALUE_MAX)
			{
				temp_command.value[value_id]=m_valuemap[hash(buffer)];
				++value_id;
			}
		}
		//문자열 읽기
		else if(snr[no][i]!=' ' && snr[no][i]!=0x09 && snr[no][i]!=',')
		{
			//단순 문자열인지 변수를 섞은 문자열인지 확인
			if(temp_command.id!=COMMAND_PRINT && IncludeValue(&snr[no][i]))	//다른 변수가 섞여 있을 때
			{
				ConvertString(temp_command.buffer, &snr[no][i]);
			}
			else	//단순 계산식일 때
			{
				temp_command.text=&snr[no][i];	//문자 참조 위치
			}

			break;
		}
	}

	//디폴트 값
	if(temp_command.id==COMMAND_SHOWPICTURE)
	{
		if(value_id<4)temp_command.value[3] = 100;		//불투명도를 적지 않으면 기본 100
		if(value_id<5)temp_command.value[4] = 1;		//반복 수를 적지 않으면 기본 1
	}
	else if(temp_command.id==COMMAND_HIDEPICTURE)
	{
		if(value_id<1)temp_command.value[0] = -1;	//그림 숨기기 번호를 적지 않으면 모두 숨김
	}

	//명령문을 큐에 넣음
	m_script->push_back(temp_command);
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

			for(int i=0; i<pit->second.loop; ++i)
			{
				if((pit->second).ani)	//애니메이션인 경우
				{
					//뒤집기
					if((pit->second).filp)
					{
						animation.GetAni((pit->second).id)->DrawEx(jdd->GetBackBufferID(), (pit->second).id, px, py, (pit->second).filp);
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
					if((pit->second).filp)
					{
						jdd->DrawPictureEx(jdd->GetBackBufferID(), (pit->second).id, px, py, NULL, (pit->second).filp);
					}
					//일반 출력
					else
					{
						jdd->DrawPicture(jdd->GetBackBufferID(), (pit->second).id, px, py, NULL);
					}
				}

				px += (pit->second).gap_x;
				py += (pit->second).gap_y;
			}
		}
	}

	animation.Process();

	//문자 표시
	std::map<int, ScriptText>::iterator tit;
	for(tit=m_textmap.begin(); tit!=m_textmap.end(); ++tit)
	{
		if((tit->second).show)
		{
			//그림자가 있는 경우
			if((tit->second).shadow)
			{
				jdd->DrawText(backbuffer, (tit->second).buffer, global_font, (tit->second).x+1, (tit->second).y+1, JColor(0, 0, 0));
			}
			//본 글자
			jdd->DrawText(backbuffer, (tit->second).buffer, global_font, (tit->second).x, (tit->second).y, (tit->second).color);
		}
	}

	//커서 표시
	if(area_select)jdd->DrawPicture(backbuffer, area_cursor, area_x, area_y, NULL);

	//음악 연주
	if(music_type == MUSIC_WAV)
	{
		HRESULT hr;
		if(FAILED(hr = m_music.ProcessDirectMusicMessages())) 
		{
			DXTRACE_ERR_MSGBOX( TEXT("ProcessDirectMusicMessages"), hr );
			return;
		}
	}

	//큐 안의 모든 작업이 끝났는지 검사하는 플래그, 일단 한번에 끝나는 작업이 대부분이므로 디폴트는 TRUE
	BOOL done=TRUE;

	//명령 실행
	std::vector<CScriptCommand>::iterator it;
	for(it=m_script.begin(); it!=m_script.end(); ++it)
	{
		if(it->id<0);	//아무것도 아닌 명령어
		
		//출력
		else if(it->id==COMMAND_PRINT)
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
			if(result!=999 && result!=-1)done=FALSE;
				else DELETE_COMMAND;
		}
		//선택
		else if(it->id==COMMAND_SELECT)
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
			if(result==-1 || result==-999)done=FALSE;
			else
			{
				select_value=result;
				DELETE_COMMAND;
			}
		}
		//숫자 입력
		else if(it->id==COMMAND_INPUTNUM)
		{
			if(!it->first)
			{
				m_input.SetFlag(FLAG_NUMBER);
				m_input.Clear();
				it->first=true;
			}

			int result=m_input.Inputing();
			if(result==0)done=FALSE;
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
					m_valuemap[hash(it->buffer)] = atoi(input_buffer);
				}

				DELETE_COMMAND;
			}
		}
		//문자 입력
		else if(it->id==COMMAND_INPUTTEXT)
		{
			if(!it->first)
			{
				m_input.SetFlag(FLAG_TEXT | FLAG_NUMBER | FLAG_MARK);
				m_input.Clear();
				it->first=true;
			}

			int result=m_input.Inputing();
			if(result==0)done=FALSE;
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
					strcpy(m_textmap[hash(it->buffer)].buffer, input_buffer);
					if(m_textmap.find(hash(it->buffer))==m_textmap.end())
					{
						m_textmap[hash(it->buffer)].show = false;
						m_textmap[hash(it->buffer)].shadow = false;
						m_textmap[hash(it->buffer)].color = JColor(0,0,0);
					}
				}

				DELETE_COMMAND;
			}
		}
		//문자 관련 명령
		else if(it->id >= COMMAND_SHOWTEXT && it->id <= COMMAND_HIDESHADOW)
		{
			//변수를 적었는지 검사
			if(!it->first && strcmp(it->buffer, "")==0 && it->id<=COMMAND_MOVETEXT)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : 문자변수 이름를 적지 않았습니다.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}

			int tmp_id = hash(it->buffer);

			//문자가 존재하는지 검사
			if(!it->first && m_textmap.find(tmp_id) == m_textmap.end() && it->id<=COMMAND_MOVETEXT)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : 해당 문자 변수를 찾을 수 없습니다.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}

			ScriptText* p_text = &m_textmap[tmp_id];	//사용중인 문자 포인터

			//문자 표시
			if(it->id==COMMAND_SHOWTEXT)
			{
				//기본 속성
				p_text->show=TRUE;
				p_text->x=it->value[0];
				p_text->y=it->value[1];
				
				DELETE_COMMAND;
			}
			//문자 숨기기
			else if(it->id==COMMAND_HIDETTEXT)
			{
				m_textmap[tmp_id].show=FALSE;
				DELETE_COMMAND;
			}
			//문자 이동
			else if(it->id==COMMAND_MOVETEXT)
			{
				//변화치 계산
				static double move_x, move_y;	//한번에 움직일 거리
				if(!it->first)
				{
					it->value[3] = Max(it->value[2], 1);	//지연시간은 최소 1
					move_x = (double)it->value[0] / (double)it->value[2];
					move_y = (double)it->value[1] / (double)it->value[2];
					p_text->rx = (double)p_text->x;
					p_text->ry = (double)p_text->y;
					it->first = true;
				}

				//이동
				if(it->value[2]>0)
				{
					p_text->rx+=move_x;
					p_text->ry+=move_y;
					p_text->x = (int)p_text->rx;
					p_text->y = (int)p_text->ry;

					--(it->value[2]);

					done=FALSE;
				}
				else
				{
					DELETE_COMMAND;
				}
			}
			//글자색
			else if(it->id==COMMAND_TEXTCOLOR)
			{
				if(tmp_id == 0)	//전체 글자색 변경
				{
					m_dlg.SetColor(it->value[0], it->value[1], it->value[2]);
					m_select.SetColor(it->value[0], it->value[1], it->value[2]);
					m_input.SetColor(it->value[0], it->value[1], it->value[2]);
					for(int i=0; i<3; ++i)text_rgb[i] = it->value[i];
				}
				else	//특정 글자색 변경
				{
					p_text->color.r = it->value[0];
					p_text->color.g = it->value[1];
					p_text->color.b = it->value[2];
				}

				DELETE_COMMAND;
			}
			//그림자 보이기
			else if(it->id==COMMAND_SHOWSHADOW)
			{
				if(tmp_id == 0)	//전체 글자 그림자
				{
					m_dlg.ShowShadow(true);
					m_select.ShowShadow(true);
					m_input.ShowShadow(true);
					text_shadow=true;
				}
				else	//특정 글자 그림자
				{
					p_text->shadow = true;
				}

				DELETE_COMMAND;
			}
			//그림자 숨기기
			else if(it->id==COMMAND_HIDESHADOW)
			{
				if(tmp_id == 0)	//전체 글자 그림자
				{
					m_dlg.ShowShadow(false);
					m_select.ShowShadow(false);
					m_input.ShowShadow(false);
					text_shadow=false;
				}
				else	//특정 글자 그림자
				{
					p_text->shadow = false;
				}

				DELETE_COMMAND;
			}
		}
		//글꼴 변경
		else if(it->id==COMMAND_FONT)
		{
			jdd->DeleteFont(global_font);
			global_font=jdd->CreateFont(it->buffer, 20);

			DELETE_COMMAND;
		}
		//대화창 설정
		else if(it->id==COMMAND_WINDOW)
		{
			//입력창, 선택장, 대화창 모두 같은 속성으로 설정
			m_dlg.SetDlg(it->value[0], it->value[1], it->value[2], it->value[3]);
			m_select.SetDlg(it->value[0], it->value[1], it->value[2], it->value[3]);

			DELETE_COMMAND;
		}
		//대화창 불투명도
		else if(it->id==COMMAND_WINDOWOPACITY)
		{
			if(it->value[0] >= 0 && it->value[0] <= 100)
			{
				m_dlg.SetOpacity((float)it->value[0] / 100);
			}
			else
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : 잘못된 투명도를 입력했습니다.\n", debug_fp);
			}

			DELETE_COMMAND;
		}
		//그림 읽기
		else if(it->id==COMMAND_PICTURE)
		{
			//잘못된 ID 입력
			if(it->value[0] < 0)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : 잘못된 그림번호를 입력했습니다.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}

			char* pic_name=StrAdd("ScriptPicture%d", it->value[0]);		//Scirpt+그림ID로 그림 생성
			bool load_first = false;	//맨 처음으로 그림을 읽어오는지 여부
			if(m_pic.find(it->value[0]) == m_pic.end())load_first=true;
			ScriptPicture* p_pic = &m_pic[it->value[0]];	//사용중인 그림 포인터

			jdd->DeleteSurface(pic_name);
			if(!jdd->DuplicateSurface(pic_name, it->buffer, &p_pic->id))
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : 그림을 찾을 수 없습니다.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}
			strcpy(p_pic->src, it->buffer);	//그림 이름 복사

			//그림 정보 저장해 두기
			JPictureInfo pi;
			jdd->GetPictureInfo(it->buffer, &pi);
			p_pic->key = pi.GetColorKey();
			p_pic->opacity = pi.GetOpacity();
			p_pic->filp = 0;	//뒤집지 않은 상태가 기본

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

			DELETE_COMMAND;
		}
		//그림 관련 명령
		else if(it->id >= COMMAND_SHOWPICTURE && it->id <= COMMAND_VFLIP)
		{
			//그림 모두 숨기기(특수 명령)
			if(it->id==COMMAND_HIDEPICTURE && it->value[0] == -1)
			{
				std::map<int, ScriptPicture>::iterator git;
				for(git = m_pic.begin(); git != m_pic.end(); ++git)git->second.show = FALSE;
				
				DELETE_COMMAND;
				continue;
			}


			//그림이 존재하는지 검사
			if(!it->first && m_pic.find(it->value[0]) == m_pic.end())
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : 그림을 찾을 수 없습니다.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}

			ScriptPicture* p_pic = &m_pic[it->value[0]];	//사용중인 그림 포인터

			//그림 표시
			if(it->id==COMMAND_SHOWPICTURE)
			{
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

				DELETE_COMMAND;
			}
			//그림 이동
			else if(it->id==COMMAND_MOVEPICTURE)
			{
				//변화치 계산
				static double move_x, move_y;	//한번에 움직일 거리
				if(!it->first)
				{
					it->value[3] = Max(it->value[3], 1);	//지연시간은 최소 1
					move_x = (double)it->value[1] / (double)it->value[3];
					move_y = (double)it->value[2] / (double)it->value[3];
					p_pic->rx = (double)p_pic->x;
					p_pic->ry = (double)p_pic->y;
					it->first = true;
				}

				//이동
				if(it->value[3]>0)
				{
					p_pic->rx+=move_x;
					p_pic->ry+=move_y;
					p_pic->x = (int)p_pic->rx;
					p_pic->y = (int)p_pic->ry;

					--(it->value[3]);

					done=FALSE;
				}
				else
				{
					DELETE_COMMAND;
				}
			}
			//그림 투명도
			else if(it->id==COMMAND_PICTUREOPACITY)
			{
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

					done=FALSE;
				}
				else
				{
					DELETE_COMMAND;
				}
			}
			//그림 숨기기
			else if(it->id==COMMAND_HIDEPICTURE)
			{
				p_pic->show=FALSE;
				DELETE_COMMAND;
			}
			//상하 뒤집기
			else if(it->id==COMMAND_HFLIP)
			{
				if(p_pic->filp & DPX_HFLIP)p_pic->filp -= DPX_HFLIP;
					else p_pic->filp |= DPX_HFLIP;

				DELETE_COMMAND;
			}
			//좌우 뒤집기
			else if(it->id==COMMAND_VFLIP)
			{
				if(p_pic->filp & DPX_VFLIP)p_pic->filp -= DPX_VFLIP;
					else p_pic->filp |= DPX_VFLIP;
				
				DELETE_COMMAND;
			}
		}
		//화면 효과
		else if(it->id==COMMAND_SCREENEFFECT)
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

				done=FALSE;
			}
			else
			{
				DELETE_COMMAND;
			}
		}
		//효과음 불러오기
		else if(it->id==COMMAND_EFFECT)
		{
			//이전 효과음을 지우고 새로운 효과음 생성
			SndObjDestroy(m_snd[it->value[0]].sound);
			m_snd[it->value[0]].sound = SndObjCreate(SoundOBJ, it->buffer, 2);

			if(m_snd[it->value[0]].sound == NULL)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : 사운드 파일을 찾을 수 없습니다.\n", debug_fp);
			}
			else strcpy(m_snd[it->value[0]].src, it->buffer);	//소스 파일 기억

			DELETE_COMMAND;
		}
		//효과음 재생
		else if(it->id==COMMAND_EFFECTPLAY)
		{
			if(m_snd.find(it->value[0]) == m_snd.end())
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" :  소리를 찾을 수 없습니다.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}

			_Play(m_snd[it->value[0]].sound);
			DELETE_COMMAND;
		}
		//배경음 읽기
		else if(it->id==COMMAND_MUSICPLAY)
		{
			char ext[20];
			strcpy(ext, strrchr(it->buffer, '.'));

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
					music_type = MUSIC_MID;
				}
			}
			else if(strcmp(ext, ".wav") == 0)
			{
				if(m_music.LoadSegmentFile(it->buffer) == S_FALSE)
				{
					DebugMode();
					fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
					fputs(" : WAV 파일을 찾을 수 없습니다.\n", debug_fp);
					music_type = MUSIC_WAV;
				}

				m_music.PlayAudio();
			}
			else if(strcmp(ext, ".ogg") == 0)
			{
				if(!m_ogg->OpenOgg(it->buffer))
				{
					DebugMode();
					fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
					fputs(" : OGG 파일을 읽을 수 없습니다.\n", debug_fp);
					music_type = MUSIC_OGG;
				}

				m_ogg->PlayOgg(true);
			}
			
			strcpy(bgm_name, it->buffer);
			DELETE_COMMAND;
		}
		//BGM 중지
		else if(it->id==COMMAND_MUSICSTOP)
		{
			/*if(music_type == MUSIC_MID)_MidiStop();
				else if(music_type == MUSIC_WAV)m_music.StopAudio();
				else if(music_type == MUSIC_OGG)m_ogg->StopOgg();*/
			music_type = -1;
			_MidiStop();
			m_music.StopAudio();
			m_ogg->StopOgg();

			strcpy(bgm_name, "");
			DELETE_COMMAND;
		}
		//동영상 재생
		else if(it->id==COMMAND_AVIPLAY)
		{
			_PlayAVI(it->buffer);

			DELETE_COMMAND;
		}
		//A키
		else if(it->id==COMMAND_KEY_A)
		{
			if(GetKey(vkey_a))
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[0]];
				done=TRUE;
				break;
			}
		}
		//S키
		else if(it->id==COMMAND_KEY_S)
		{
			if(GetKey(vkey_s))
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[0]];
				done=TRUE;
				break;
			}
		}
		//D키
		else if(it->id==COMMAND_KEY_D)
		{
			if(GetKey(vkey_d))
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[0]];
				done=TRUE;
				break;
			}
		}
		//F키
		else if(it->id==COMMAND_KEY_F)
		{
			if(GetKey(vkey_f))
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[0]];
				done=TRUE;
				break;
			}
		}
		//위 화살표
		else if(it->id==COMMAND_KEY_UP)
		{
			if(GetKey(vkey_up))
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[0]];
				done=TRUE;
				break;
			}
		}
		//아래 화살표
		else if(it->id==COMMAND_KEY_DOWN)
		{
			if(GetKey(vkey_down))
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[0]];
				done=TRUE;
				break;
			}
		}
		//왼쪽 화살표
		else if(it->id==COMMAND_KEY_LEFT)
		{
			if(GetKey(vkey_left))
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[0]];
				done=TRUE;
				break;
			}
		}
		//오른쪽 화살표
		else if(it->id==COMMAND_KEY_RIGHT)
		{
			if(GetKey(vkey_right))
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[0]];
				done=TRUE;
				break;
			}
		}
		//엔터키
		else if(it->id==COMMAND_KEY_ENTER)
		{
			if(GetKey(vkey_enter))
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[0]];
				done=TRUE;
				break;
			}
		}
		//랜덤
		else if(it->id==COMMAND_RANDOM)
		{
			m_valuemap[hash(it->buffer)] = (rand()%(abs(it->value[1] - it->value[0]))+1) + it->value[0];

			DELETE_COMMAND;
		}
		//잠시 대기
		else if(it->id==COMMAND_WAIT)
		{
			if(it->value[0]>0)
			{
				--(it->value[0]);
				done=FALSE;
			}
			else DELETE_COMMAND;
		}
		//이동
		else if(it->id==COMMAND_GOTO)
		{
			if(it->value[0] >= 0)	//마이너스 값은 무시
			{
				return_no.push_back(script_no);	//돌아올 주소 저장
				script_no = m_BookmarkHash[it->value[0]];	//이동
			}

			DELETE_COMMAND;
		}
		//분기
		else if(it->id==COMMAND_CASE)
		{
			if(select_value < SCRIPT_VALUE_MAX && it->value[select_value] >= 0)
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[select_value]];
				select_value = 0;	//선택지 초기화
			}

			DELETE_COMMAND;
		}
		//돌아가기
		else if(it->id==COMMAND_RETURN)
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
			DELETE_COMMAND;
		}
		//다른 스크립트 로드
		else if(it->id==COMMAND_LOAD)
		{
			int start_no = it->value[0]; //시작 주소 외워두기
			UnLoad();
			Load(it->buffer);
			script_no = m_BookmarkHash[start_no];
			done=TRUE;
			break;
		}
		//로드 메뉴 부르기
		else if(it->id==COMMAND_LOADMENU)
		{
			on_loadmenu=true;
			DELETE_COMMAND;
		}
		//세이브 메뉴 부르기
		else if(it->id==COMMAND_SAVEMENU)
		{
			on_savemenu=true;
			DELETE_COMMAND;
		}
		//챕터명 변경
		else if(it->id==COMMAND_CHAPTER)
		{
			if(strcmp(it->buffer, "")==0)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : 챕터명을 적지 않았습니다.\n", debug_fp);
			}

			strcpy(chapter_name, it->buffer);
			DELETE_COMMAND;
		}
		//세이브 가능, 불가능
		else if(it->id==COMMAND_ENABLESAVE)
		{
			enable_save = true;
			DELETE_COMMAND;
		}
		else if(it->id==COMMAND_DISABLESAVE)
		{
			enable_save = false;
			DELETE_COMMAND;
		}
		//영역선택
		else if(it->id==COMMAND_SELECTAREA)
		{
			//초기 설정
			if(!it->first)
			{
				area_select = true;
				strcpy(area_cursor, it->buffer);

				it->first = true;				
			}

			done=false;

			//좌우이동
			if(GetKey(vkey_left, 1))
			{
				area_x = Max(area_x-5, 0);
			}
			else if(GetKey(vkey_right, 1))
			{
				area_x = Min(area_x+5, SCREEN_X);
			}
			//상하이동
			if(GetKey(vkey_up, 1))
			{
				area_y = Max(area_y-5, 0);
			}
			else if(GetKey(vkey_down, 1))
			{
				area_y = Min(area_y+5, SCREEN_Y);
			}
			//선택
			if(GetKey(vkey_enter))
			{
				area_select = false;
				done=true;
				DELETE_COMMAND;
			}
		}
		//영역선택에 의한 분기
		else if(it->id==COMMAND_GOTOAREA)
		{
			if(area_x >= it->value[1] && area_x <= it->value[3] && area_y >= it->value[2] && area_y <= it->value[4])
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[select_value]];
			}

			DELETE_COMMAND;
		}
		//끝내기
		else if(it->id==COMMAND_END)
		{
			script_end = true;
			DELETE_COMMAND;
		}
	}

	//모든 명령이 완료되었으면 큐 삭제
	if(done)m_script.clear();
}