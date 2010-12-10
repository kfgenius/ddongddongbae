//2007. 3. 5 - ��ũ��Ʈ�� ���ο� �°� ��ġ�� ���ؼ� ���� �������
#include <windows.h>

#include <string>

#include "DScript.h"
#include "Game.h"
#include "calc.h"

//��ɹ�
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

//ȭ�� ȿ��
#define SE_FADE					0
#define SE_WHITE				1
#define SE_GRAY					2
#define SE_HUE					3
#define SE_RED					4
#define SE_GREEN				5
#define SE_BLUE					6

//��ũ��Ʈ���� ������ �о���̴� ���
#define READ_NUM				0
#define READ_VAR				1
#define READ_TEXT				2
#define READ_POINT				3

//���� Ÿ��
#define MUSIC_MID				1
#define MUSIC_WAV				2
#define MUSIC_OGG				3

//�ý��� ����
#define CURSOR_SOUND			0
#define SELECT_SOUND			1

//����ȭ�� ���� ��ũ��
#define DELETE_COMMAND			it->id=-1	//��ɹ� ���

//�ؽ� �Լ�(�����̺� �� ������)
#define HASH_NUMBER 37
#define HASH_SIZE 262139

// ���� ����
HSNDOBJ sound[2];				//�ý��� ����
map<int, int> m_valuemap;		//���� ����
map<int, ScriptText> m_textmap;	//���� ����

//���� �Լ�
void ClearValueMap()	//���� �� Ŭ����
{
	m_valuemap.clear();
	m_textmap.clear();
}

static int hash(char* sz)	//�ؽ��� ���ϱ�
{
	if(sz==NULL) return 0;
	unsigned int hashvalue=0;
	for(int lp=0;sz[lp];lp++)
		hashvalue=(hashvalue*HASH_NUMBER+sz[lp])%HASH_SIZE;
	return hashvalue;
}

//��ũ��Ʈ ��� �޼ҵ�
CScriptCommand::CScriptCommand()
{
	//�� �ʱ�ȭ
	ZeroMemory(value, sizeof(int)*SCRIPT_VALUE_MAX);
	text=NULL;
	strcpy(buffer,"");
	first=false;
}

//������ �������� �˻�
BOOL IncludeValue(char* str)
{
	if(strchr(str, '[')!=NULL || strchr(str, '{')!=NULL || strchr(str, '|')!=NULL)return TRUE;
		else return FALSE;
}

#define KEY1 8			//��ũ��Ʈ ���� �ص��� ���� Ű1
#define KEY2 4			//��ũ��Ʈ ���� �ص��� ���� Ű2

/////////////////////////////////////////////////////
//��ũ��Ʈ �޼ҵ�
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

//��ȭ ���� �ҷ�����
CScript::CScript(char* script_file)
{	
	Load(script_file);
	if(!enable)return;	//�ʱ�ȭ ���н� ���

	CreateCommandMap();

	//â �ʱ�ȭ
	m_dlg.SetDlg(5,395,630,4);
	m_select.SetDlg(5, 395, 630, 4);

	//����
	m_ogg = new COgg(SoundOBJ);
	sound[CURSOR_SOUND] = SndObjCreate(SoundOBJ, "Sound\\Cursor.wav", 2);
	sound[SELECT_SOUND] = SndObjCreate(SoundOBJ, "Sound\\Select.wav", 2);

	//DirectMusic
	m_music.InitDirectMusic();

	//��Ÿ
	debug_mode = false;	
	text_rgb[0]=text_rgb[1]=text_rgb[2]=0;
	text_shadow=false;
	m_input.SetDlg(250,230,140);
}

//��ȭ���� ����
CScript::~CScript()
{
	//����� ���� �ݱ�
	if(debug_mode)fclose(debug_fp);

	//��ũ��Ʈ ���� ��ε�
	UnLoad();

	//��ɹ� ����
	m_script.clear();

	//��ɹ� �� ��ü
	m_CommandMap.clear();

	//���� �÷��̾� ����
	delete m_ogg;

	//DScript�� jdd�� ������ ����
	jdd->Cleanup();
	delete jdd;
	jdd=NULL;
}

void CScript::Load(char* script_file)
{
	char tmp_file[256];
	//dlg�� �б�
	sprintf(tmp_file, "%s.dlg", script_file);
	if(fp=fopen(tmp_file, "rb"))
	{
		//��� �� �б�
		fread(&snrs,sizeof(int),1,fp);
		//å���� �б�
		int mark_count;
		fread(&mark_count,sizeof(int),1,fp);
		for(int i=0; i<mark_count; ++i)
		{
			int temp_bookmark, temp_no;
			fread(&temp_no,sizeof(int),1,fp);
			fread(&temp_bookmark,sizeof(int),1,fp);
			m_BookmarkHash[temp_no]=temp_bookmark;
		}
		//��系�� �б�
		snr=new char*[snrs];
		for(int snr_no=0; snr_no<snrs; ++snr_no)
		{			
			//���Ͽ��� �б�
			int text_size;
			fread(&text_size,sizeof(int),1,fp);
			snr[snr_no]=new char[text_size/2+1];

			char* buffer=new char[text_size+1];
			fread(buffer,text_size,1,fp);
			buffer[text_size]=NULL;

			//�ص�
			int i;
			for(i=0; i<text_size/2; ++i)
			{
				char b1, b2;
				b1=buffer[i*2];
				b2=buffer[i*2+1];

				snr[snr_no][i]=DlgDecode(b1, b2);
				if(snr[snr_no][i]=='\n' || snr[snr_no][i]=='\r')break;	//ĳ���� ���� �ɷ�����
			}
			snr[snr_no][i]=NULL;
			delete[] buffer;
		}

		fclose(fp);

		enable=TRUE;
	}
	else
	{
		//�ؽ�Ʈ�� �б�
		sprintf(tmp_file, "%s.txt", script_file);
		strcpy(this->script_file, script_file);
		if(fp=fopen(tmp_file, "rb"))
		{
			LoadFromTxt();			
		}
		//������ ã�� �� ���� ��
		else
		{	
			DebugMode();
			char error_msg[256];
			sprintf(error_msg, "%s ��ũ��Ʈ ������ ã�� �� �����ϴ�!\n", script_file);
			fputs(error_msg, debug_fp);

			enable=FALSE;;

			return;
		}		
	}

	//�׸� �ڷ� �о����
	sprintf(mlc_file, "%s.mlc", script_file);
	if(!jre->LoadResource(mlc_file))
	{
		DebugMode();
		fprintf(debug_fp, "%s ������ ã�� �� �����ϴ�.\n", mlc_file);
	}

	//é�͸� ���
	char* temp_chapter = strrchr(script_file, '\\');
	if(temp_chapter == NULL)strcpy(chapter_name, script_file);
		else strcpy(chapter_name, temp_chapter+1);

	//ȭ��ȿ�� �ʱ�ȭ
	screen_effect[0] = 0;
	screen_effect[1] = 0;

	//���� ���� �ʱ�ȭ
	strcpy(bgm_name, "");

	//��Ÿ
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

	//������ �˾Ƴ���
	fseek(fp, 0, SEEK_END);
	long max=ftell(fp);
	fseek(fp, 0, SEEK_SET);

	bool note=false;
	bool content=false;
	int cp=0;

	bool is_text=false;	//""���� �������� Ȯ��

	while(ftell(fp) < max)
	{
		char t;
		fread(&t, sizeof(char), 1, fp);
		//�� ���
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
			continue;	//���� ����
		}
		//å����
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

		content=true;	//���� ����

		//���� ����
		if(!note)
		{
			if(t=='/' && !is_text)
			{
				char old_t=t;
				fread(&t, sizeof(char), 1, fp);
				//�ּ�
				if(t=='/')
				{
					if(cp==0)dlg_no--;
					note=true;
				}
				//�ּ� �ƴ�, �׸��� �� �ٲ�
				else if(t=='\n')
				{
					dlg[dlg_no]+=old_t;

					dlg_no++;
					content=note=false;
					cp=0;
					is_text=false;
				}
				//�ּ� �ƴ�
				else
				{
					if(t!='\r' && t!=0x09)	//�ǰ� ĳ���� ������ ����
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

	//������ ����� ������ �ִ��� �˻�
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

	//��系�� �б�
	snr=new char*[snrs];
	for(int snr_no=0; snr_no<snrs; ++snr_no)
	{			
		//���Ͽ��� �б�
		size_t text_size = dlg[snr_no].length();
		snr[snr_no]=new char[text_size+1];
		strcpy(snr[snr_no], dlg[snr_no].data());

		/*char debug[256];
		sprintf(debug, "%d:%s\n",snr_no,snr[snr_no]);
		OutputDebugString(debug); ����׿�*/
	}

	fclose(fp);

	enable=TRUE;

	//�޸� ����
	for(int i=0; i<10000; ++i)dlg[i].clear();
}

void CScript::UnLoad()
{
	//��ũ��Ʈ �޸� ��ü
	for(int i=0; i<snrs; ++i)
		if(snr[i]!=NULL)
		{
			delete[] snr[i];
			snr[i]=NULL;
		}

	//å���� ��ü
	m_BookmarkHash.clear();

	//���� ��ü
	std::map<int, ScriptPicture>::iterator it;
	for(it = m_pic.begin(); it!=m_pic.end(); ++it)
	{
		jdd->DeleteSurface((it->second).id);
	}
	m_pic.clear();

	//���� ǥ�� ���� �ʱ�ȭ
	std::map<int, ScriptText>::iterator tit;
	for(tit = m_textmap.begin(); tit!=m_textmap.end(); ++tit)
	{
		(tit->second).show = false;
		(tit->second).shadow = false;
		(tit->second).color = JColor(0,0,0);
	}

	//���� ��ü
	std::map<int, ScriptSound>::iterator it2;
	for(it2 = m_snd.begin(); it2!=m_snd.end(); ++it2)
	{
		SndObjDestroy((it2->second).sound);
		(it2->second).sound=NULL;
	}
	m_snd.clear();

	//���� ���� ����
	return_no.clear();

	//�׸� ����
	jre->UnloadResource(mlc_file);

	//ȭ��ȿ�� �ʱ�ȭ
	screen_effect[0] = 0;
	screen_effect[1] = 0;

	//���� �Ҹ� ����
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
	//���̺� ������ ��Ȳ���� Ȯ��
	if(save)
	{
		std::vector<CScriptCommand>::iterator it;
		for(it=m_script.begin(); it!=m_script.end(); ++it)
		{
			if(it->id<0 || it->id==COMMAND_PRINT || it->id>=COMMAND_KEY_A && it->id<=COMMAND_KEY_ENTER);	//������� ��ɾ�
			else return;	//���� �� ���� ����� ������ �Ұ���
		}
	}

	//�޴� ����
	int selected_menu = 0;
	char menu_name[SYSTEMMENU_MAX][80];
	char save_filename[80];

	if(!enable_save && save)selected_menu=SYSTEMMENU_MAX-2;	//���̺� �Ұ����� ���

	strcpy(menu_name[SYSTEMMENU_MAX-1], "���� ����");
	strcpy(menu_name[SYSTEMMENU_MAX-2], "Ÿ��Ʋ��");

	//���̺� ���� ���� ���
	for(int i=0; i<SYSTEMMENU_MAX-2; ++i)
	{
		FILE *info_fp;
		sprintf(save_filename, "Save\\Save%d.sav", i+1);
		if(info_fp = fopen(save_filename, "rb"))
		{
			//��ũ��Ʈ ���̺� �������� Ȯ��
			char header[9];
			fread(header, sizeof(char), 8, info_fp);
			header[8]=NULL;
			//é�� �̸� �б�
			if(strcmp(header, "HungSave")==0)
			{
				fseek(info_fp, 80, SEEK_CUR);
				fread(menu_name[i], sizeof(char), 80, info_fp);
			}
			else strcpy(menu_name[i], "�� ���̺� ����");
			
			fclose(info_fp);
		}
		else strcpy(menu_name[i], "�� ���̺� ����");
	}
	
	//�ý��� �޴�
	while(1)
	{
		if(!ProcessMessage())break;
		if(GetKey(vkey_esc))break;

		//�̵�
		int old_pos = selected_menu;
		if(GetKey(vkey_up, 10))--selected_menu;
			else if(GetKey(vkey_down, 10))++selected_menu;

		if(save)
		{
			if(enable_save)selected_menu = MaxMin(selected_menu, 0, SYSTEMMENU_MAX-1);
				else selected_menu = MaxMin(selected_menu, SYSTEMMENU_MAX-2, SYSTEMMENU_MAX-1);
		}
		else selected_menu = MaxMin(selected_menu, 0, SYSTEMMENU_MAX-3);

		if(old_pos != selected_menu)_Play(sound[CURSOR_SOUND]);	//ȿ����

		//����
		if(GetKey(vkey_enter))
		{
			//���� ����
			if(selected_menu == SYSTEMMENU_MAX-1)
			{
				script_end = true;				
				break;
			}
			//Ÿ��Ʋ��
			else if(selected_menu == SYSTEMMENU_MAX-2)
			{
				_Play(sound[SELECT_SOUND]);	//ȿ����

				m_script.clear();
				UnLoad();
				Load("MAIN");
				script_no = 0;				
				break;
			}
			//���̺�
			else if(save)
			{
				FILE *save_fp;
				sprintf(save_filename, "Save\\Save%d.sav", selected_menu+1);
				if(save_fp = fopen(save_filename, "wb"))
				{
					_Play(sound[SELECT_SOUND]);	//ȿ����

					//���
					fwrite("HungSave", sizeof(char), 8, save_fp);
					fwrite(script_file, sizeof(char), 80, save_fp);
					fwrite(chapter_name, sizeof(char), 80, save_fp);
					//���� ���� ���
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
					//���� ��� ���� ���
					fwrite(bgm_name, sizeof(char), 80, save_fp);
					//��ɹ� ���
					size=m_script.size();
					fwrite(&size, sizeof(int), 1, save_fp);
					std::vector<CScriptCommand>::iterator cit;
					for(cit=m_script.begin(); cit!=m_script.end(); ++cit)
					{
						fwrite(&(*cit), sizeof(CScriptCommand), 1, save_fp);
					}
					//�׸� ���
					size=m_pic.size();
					fwrite(&size, sizeof(int), 1, save_fp);
					std::map<int, ScriptPicture>::iterator pit;
					for(pit=m_pic.begin(); pit!=m_pic.end(); ++pit)
					{
						fwrite(&pit->first, sizeof(int), 1, save_fp);
						fwrite(&pit->second, sizeof(ScriptPicture), 1, save_fp);
					}
					//�Ҹ� ���
					size=m_snd.size();
					fwrite(&size, sizeof(int), 1, save_fp);
					std::map<int, ScriptSound>::iterator sit;
					for(sit=m_snd.begin(); sit!=m_snd.end(); ++sit)
					{
						fwrite(&sit->first, sizeof(int), 1, save_fp);
						fwrite(&sit->second, sizeof(ScriptSound), 1, save_fp);
					}
					//���� ���
					size=m_valuemap.size();
					fwrite(&size, sizeof(int), 1, save_fp);
					std::map<int, int>::iterator vit;
					for(vit=m_valuemap.begin(); vit!=m_valuemap.end(); ++vit)
					{
						fwrite(&vit->first, sizeof(int), 1, save_fp);
						fwrite(&vit->second, sizeof(int), 1, save_fp);
					}
					//���� ���� ���
					size=m_textmap.size();
					fwrite(&size, sizeof(int), 1, save_fp);
					std::map<int, ScriptText>::iterator tit;
					for(tit=m_textmap.begin(); tit!=m_textmap.end(); ++tit)
					{
						fwrite(&tit->first, sizeof(int), 1, save_fp);
						fwrite(&tit->second, sizeof(ScriptText), 1, save_fp);
					}

					fclose(save_fp);

					//����� �޴��̸� ����
					strcpy(menu_name[selected_menu], chapter_name);
				}
			}
			//�ε�
			else
			{
				FILE *load_fp;
				sprintf(save_filename, "Save\\Save%d.sav", selected_menu+1);
				if(load_fp = fopen(save_filename, "rb"))
				{
					//���
					char header[9];
					fread(header, sizeof(char), 8, load_fp);
					header[8] = NULL;
					if(strcmp(header, "HungSave")==0)
					{
						_Play(sound[SELECT_SOUND]);	//ȿ����

						//������ ��ũ��Ʈ�� ��ε��ϰ� ��ϵ� ��ũ��Ʈ �б�
						fread(script_file, sizeof(char), 80, load_fp);
						m_script.clear();
						UnLoad();
						Load(script_file);
						fread(chapter_name, sizeof(char), 80, load_fp);
						//���� ���� �б�
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
						//���ڻ�, �׸��� ����
						m_dlg.SetColor(text_rgb[0], text_rgb[1], text_rgb[2]);
						m_select.SetColor(text_rgb[0], text_rgb[1], text_rgb[2]);
						m_input.SetColor(text_rgb[0], text_rgb[1], text_rgb[2]);
						m_dlg.ShowShadow(text_shadow);
						m_select.ShowShadow(text_shadow);
						m_input.ShowShadow(text_shadow);
						//���� ��� ���� �б�
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
						//��ɹ� �б�
						fread(&size, sizeof(int), 1, load_fp);
						for(size_t i=0; i<size; ++i)
						{
							CScriptCommand tmp_command;
							fread(&tmp_command, sizeof(CScriptCommand), 1, load_fp);

							//��¹��� ��� ���� ������
							if(tmp_command.id == COMMAND_PRINT)
							{
								tmp_command.first=false;
								if(snr[tmp_command.snr_no][0]=='@' || snr[tmp_command.snr_no][0]=='~')
								{
									tmp_command.text = strchr(snr[tmp_command.snr_no], ' ') + 1;
								}
								else tmp_command.text = snr[tmp_command.snr_no];
							}
							//0������ ��� ����� ����
							if(tmp_command.id>=0)m_script.push_back(tmp_command);
						}
						//�׸� �б�
						fread(&size, sizeof(int), 1, load_fp);
						for(size_t i=0; i<size; ++i)
						{
							int tmp_id, pid;
							ScriptPicture tmp_pic;
							fread(&tmp_id, sizeof(int), 1, load_fp);
							fread(&tmp_pic, sizeof(ScriptPicture), 1, load_fp);
							m_pic[tmp_id] = tmp_pic;

							//�׸� ����
							char* pic_name=StrAdd("ScriptPicture%d", tmp_id);
							jdd->DeleteSurface(pic_name);
							if(!jdd->DuplicateSurface(pic_name, m_pic[tmp_id].src, &pid))continue;

							//���� ����
							JPictureInfo pi;
							pi.SetOpacity(m_pic[tmp_id].opacity);
							pi.SetColorKey(m_pic[tmp_id].key);
							jdd->SetPictureInfo(pic_name, &pi);

							//�ִϿ����� ���
							if(m_pic[tmp_id].ani)
							{
								JPictureInfo pi;
								jdd->GetPictureInfo(m_pic[tmp_id].src, &pi);

								int ani_x = pi.GetWidth() / m_pic[tmp_id].frame;	//������ ���� �°� ���� ���� ������
								int ani_y = pi.GetHeight();							//���δ� ������ �׸� ũ�⿡ ����

								if(m_pic[tmp_id].anitype == 0)animation.CreateAnimation(m_pic[tmp_id].id, ani_x, ani_y, ani_loop, m_pic[tmp_id].frame, m_pic[tmp_id].delay);
								else if(m_pic[tmp_id].anitype == 2)animation.CreateAnimation(m_pic[tmp_id].id, ani_x, ani_y, ani_once, m_pic[tmp_id].frame, m_pic[tmp_id].delay);
								else animation.CreateAnimation(m_pic[tmp_id].id, ani_x, ani_y, ani_exchange, m_pic[tmp_id].frame, m_pic[tmp_id].delay);
							}
						}
						//�Ҹ� �б�
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
						//���� �б�
						fread(&size, sizeof(int), 1, load_fp);
						for(size_t i=0; i<size; ++i)
						{
							int tmp_val[2];
							fread(&tmp_val, sizeof(int), 2, load_fp);
							m_valuemap[tmp_val[0]]=tmp_val[1];
						}
						//���� ���� �б�
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

		//��� �κ�
		if(save)jdd->DrawPicture(backbuffer, "SystemSave", 0, 0, NULL);
			else jdd->DrawPicture(backbuffer, "SystemLoad", 0, 0, NULL);
		int put_max = (save)?SYSTEMMENU_MAX:SYSTEMMENU_MAX-2;
		for(int i=0; i<put_max; ++i)
		{
			if(save && !enable_save && i<SYSTEMMENU_MAX-2)continue;		//���̺� �Ұ����� �� �������� ����

			int menu_y = i*50+60;

			jdd->DrawPicture(backbuffer, "SystemMenu", 128, menu_y, NULL);
			if(selected_menu == i)jdd->DrawPicture(backbuffer, "SystemCursor", 128, menu_y, NULL);

			//���� ����
			int center=(int)(640-(strlen(menu_name[i])*10))/2;
			jdd->DrawText(backbuffer, menu_name[i], global_font, center, menu_y+10, JColor(0,0,0));
		}		

		jdd->Render();
	}
}

//�ý��� ���� ���
void CScript::GetSystemValue(char* dest, char* src)
{
	char buffer[40];

	if(strcmp(src, "Ŀ��X") == 0)
	{
		_itoa_s(area_x, buffer, 40, 10);
		strcpy(dest, buffer);
	}
	else if(strcmp(src, "Ŀ��Y") == 0)
	{
		_itoa_s(area_y, buffer, 40, 10);
		strcpy(dest, buffer);
	}
	else if(strcmp(src, "���ϸ�") == 0)
	{
		strcpy(dest, script_file);
	}
	else if(strcmp(src, "����") == 0)
	{
		_itoa_s(select_value, buffer, 40, 10);
		strcpy(dest, buffer);
	}
	else if(strcmp(src, "é�͸�") == 0)
	{
		strcpy(dest, chapter_name);
	}
	else if(strcmp(src, "�����") == 0)
	{
		strcpy(dest, bgm_name);
	}
	else if(strcmp(src, "Ŀ����") == 0)
	{
		strcpy(dest, area_cursor);
	}
}

//��ũ��Ʈ ó��
void CScript::Script(int no)
{
	if(!enable)return;	//��ũ��Ʈ ������ ����� �ε����� �ʾ��� ��

	if(m_BookmarkHash.find(no) == m_BookmarkHash.end())	//�� ã���� ��
	{
		script_end=true;		
	}
	else
	{
		script_no=m_BookmarkHash[no];	//��ũ��Ʈ ������
		script_end = false;
	}

	//��ũ��Ʈ ����
	while(!script_end)
	{
		BringScript();
		if(script_no < 0)break;	//����

		while(!m_script.empty() && !script_end)
		{
			if(!ProcessMessage())break;

			RunScript();	//��ũ��Ʈ ó��

			//ȭ�� ȿ��
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

			//�ε� �޴�
			if(on_loadmenu)
			{
				SystemMenu(false);
				on_loadmenu=false;
			}
			//���̺� �޴�
			else if(GetKey(vkey_esc) || on_savemenu)
			{
				SystemMenu();
				on_savemenu=false;
			}
		}
		
		//��ũ��Ʈ �� or ���� �˻�
		if(script_no < 0 || script_no >= snrs || !enable)break;
	}
}

void CScript::BringScript()
{
	//��ũ��Ʈ ������ ����� �ε����� �ʾ��� ��
	if(!enable)
	{
		script_no = -1;
		return;
	}

	BOOL read_next;	//����ؼ� ���� ����� �д��� ����

	do{
		read_next=ReadScript(&m_script, script_no);
		++script_no;
	}while(read_next);
}

//å���ǰ� ���� ������ Ȯ��
BOOL CScript::IsBookMark(int no)
{
	std::map<int,int>::iterator it;
	for(it=m_BookmarkHash.begin(); it!=m_BookmarkHash.end(); ++it)
	{
		if(no==it->second)return TRUE;
	}

	return FALSE;
}

//��ɹ� �� ����
void CScript::CreateCommandMap()
{
	//�ؽ�Ʈ ���� ��ɹ�
	m_CommandMap[hash("���")]=COMMAND_PRINT;
	m_CommandMap[hash("����")]=COMMAND_SELECT;
	m_CommandMap[hash("�����Է�")]=COMMAND_INPUTNUM;
	m_CommandMap[hash("�����Է�")]=COMMAND_INPUTTEXT;
	m_CommandMap[hash("����ǥ��")]=COMMAND_SHOWTEXT;
	m_CommandMap[hash("���ڼ���")]=COMMAND_HIDETTEXT;
	m_CommandMap[hash("�����̵�")]=COMMAND_MOVETEXT;

	//�ؽ�Ʈ ���� ��ɹ�
	m_CommandMap[hash("��ȭâ")]=COMMAND_WINDOW;
	m_CommandMap[hash("â������")]=COMMAND_WINDOWOPACITY;
	m_CommandMap[hash("���ڻ�")]=COMMAND_TEXTCOLOR;
	m_CommandMap[hash("�׸���ǥ��")]=COMMAND_SHOWSHADOW;
	m_CommandMap[hash("�׸��ڼ���")]=COMMAND_HIDESHADOW;
	m_CommandMap[hash("�۲�")]=COMMAND_FONT;

	//�׸� ���� ��ɹ�
	m_CommandMap[hash("�׸�")]=COMMAND_PICTURE;
	m_CommandMap[hash("�׸�ǥ��")]=COMMAND_SHOWPICTURE;
	m_CommandMap[hash("�׸��̵�")]=COMMAND_MOVEPICTURE;
	m_CommandMap[hash("�׸�����")]=COMMAND_HIDEPICTURE;
	m_CommandMap[hash("�׸�������")]=COMMAND_PICTUREOPACITY;
	m_CommandMap[hash("�¿����")]=COMMAND_HFLIP;
	m_CommandMap[hash("���Ϲ���")]=COMMAND_VFLIP;
	m_CommandMap[hash("ȭ��ȿ��")]=COMMAND_SCREENEFFECT;

	//���� ���� ��ɹ�
	m_CommandMap[hash("�������")]=COMMAND_MUSICPLAY;
	m_CommandMap[hash("��������")]=COMMAND_MUSICSTOP;
	m_CommandMap[hash("�Ҹ�")]=COMMAND_EFFECT;
	m_CommandMap[hash("�Ҹ����")]=COMMAND_EFFECTPLAY;
	m_CommandMap[hash("AVI")]=COMMAND_AVIPLAY;

	//Ű�Է� ��ɹ�
	m_CommandMap[hash("AŰ")]=COMMAND_KEY_A;
	m_CommandMap[hash("SŰ")]=COMMAND_KEY_S;
	m_CommandMap[hash("DŰ")]=COMMAND_KEY_D;
	m_CommandMap[hash("FŰ")]=COMMAND_KEY_F;
	m_CommandMap[hash("��Ű")]=COMMAND_KEY_UP;
	m_CommandMap[hash("��Ű")]=COMMAND_KEY_DOWN;
	m_CommandMap[hash("��Ű")]=COMMAND_KEY_LEFT;
	m_CommandMap[hash("��Ű")]=COMMAND_KEY_RIGHT;
	m_CommandMap[hash("����Ű")]=COMMAND_KEY_ENTER;

	//����
	m_CommandMap[hash("����")]=COMMAND_RANDOM;

	//��ũ��Ʈ �帧 ����
	m_CommandMap[hash("���")]=COMMAND_WAIT;
	m_CommandMap[hash("�б�")]=COMMAND_CASE;
	m_CommandMap[hash("�̵�")]=COMMAND_GOTO;
	m_CommandMap[hash("���ư�")]=COMMAND_RETURN;
	m_CommandMap[hash("�ε�")]=COMMAND_LOAD;
	m_CommandMap[hash("�ε�޴�")]=COMMAND_LOADMENU;
	m_CommandMap[hash("���̺�޴�")]=COMMAND_SAVEMENU;
	m_CommandMap[hash("é��")]=COMMAND_CHAPTER;
	m_CommandMap[hash("���̺갡��")]=COMMAND_ENABLESAVE;
	m_CommandMap[hash("���̺�Ұ�")]=COMMAND_DISABLESAVE;
	m_CommandMap[hash("��������")]=COMMAND_SELECTAREA;
	m_CommandMap[hash("�����б�")]=COMMAND_GOTOAREA;

	m_CommandMap[hash("��")]=COMMAND_END;
}

int CScript::GetCommandID(int idx)
{
	return m_CommandMap[idx];
}

void CScript::ConvertString(char *result, char* source)
{
	size_t cp=0;			//�о���̴� ����Ʈ
	char buffer[80];		//���� �̸� ����
	char num_buffer[80];	//���� ���� ���� �� ����
	int depth=0;			//�ȿ� �� ������ �ִ��� ����

	size_t strmax = strlen(source);

	for(size_t i=0; i<strmax; ++i)
	{
		//���� ����
		if(source[i] == '[')
		{
			//���� �κ��� �о� ����
			size_t start_point=++i;
			for(; i < strmax; ++i)
			{
				if(source[i]==']')	//����, ���� ��ȣ���� �˻�
				{
					if(depth<=0)break;
						else --depth;
				}
				else if(source[i]=='[')++depth;

				buffer[i-start_point] = source[i];
			}
			buffer[i-start_point] = NULL;

			//���� ���� ���� ����
			if(IncludeValue(buffer))
			{
				char tmp_buffer[80];
				strcpy(tmp_buffer, buffer);
				ConvertString(buffer, tmp_buffer);
			}

			//���ڿ� ��ġ��
			_itoa_s(m_valuemap[hash(buffer)], num_buffer, 40, 10);
			result[cp]=NULL;
			strcat(result, num_buffer);
			cp = strlen(result);
		}
		//���� ����
		else if(source[i] == '{')
		{
			//���� �κ��� �о� ����
			size_t start_point=++i;
			for(; i < strmax; ++i)
			{
				if(source[i]=='}')	//����, ���� ��ȣ���� �˻�
				{
					if(depth<=0)break;
						else --depth;
				}
				else if(source[i]=='{')++depth;

				buffer[i-start_point] = source[i];
			}
			buffer[i-start_point] = NULL;

			//���� ���� ���� ����
			if(IncludeValue(buffer))
			{
				char tmp_buffer[80];
				strcpy(tmp_buffer, buffer);
				ConvertString(buffer, tmp_buffer);
			}

			//���� ��ġ��
			result[cp]=NULL;
			strcat(result, m_textmap[hash(buffer)].buffer);
			cp = strlen(result);
		}
		//�ý��� ����
		else if(source[i] == '|')
		{
			//���� �κ��� �о� ����
			size_t start_point=++i;
			for(; i < strmax; ++i)
			{
				if(source[i]=='|')break;

				buffer[i-start_point] = source[i];
			}
			buffer[i-start_point] = NULL;

			//���� ���� ���� ����
			if(IncludeValue(buffer))
			{
				char tmp_buffer[80];
				strcpy(tmp_buffer, buffer);
				ConvertString(buffer, tmp_buffer);
			}

			//�ý��� ������ ��ġ��
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

	//NULL�� �� ���� ��츦 ����ؼ�
	result[cp] = NULL;
}

//��ɹ� �о���̱�
BOOL CScript::ReadScript(vector<CScriptCommand> *m_script, int no)	//�����ϴ� ���� ����ؼ� ��ũ��Ʈ�� �д��� �ϴ� ��
{
	CScriptCommand temp_command;
	BOOL read_next=FALSE;
	char buffer[80];	//�ٿ뵵 ����

	temp_command.snr_no = no;

	//���� ����
	if(snr[no][0]=='[')
	{
		//���� �̸� �о���̱�
		int i;
		for(i=1; snr[no][i]!=']' && snr[no][i] != NULL; ++i)buffer[i-1] = snr[no][i];
		buffer[i-1] = NULL;

		//���� �̸� �ȿ� ������ ���� ���
		if(IncludeValue(buffer))
		{
			char tmp_buffer[80];
			strcpy(tmp_buffer, buffer);
			ConvertString(buffer, tmp_buffer);
		}

		//�ܼ� ������� �ٸ� ������ �̿��� ������� Ȯ��
		char* calc = NULL;
		char text_buffer[1024];

		if(IncludeValue(&snr[no][i+1]))	//�ٸ� ������ ���� ���� ��
		{
			ConvertString(text_buffer, &snr[no][i+1]);
			calc = text_buffer;
		}
		else	//�ܼ� ������ ��
		{
			calc = strchr(snr[no], ' ');
		}

		//������ �� �ֱ�
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
				fputs("�߸��� �����Դϴ�.\n", debug_fp);
			}
			else
			{
				px=ev.Evaluate(vr, vals);
				if(px)
				{
					DebugMode();
					fprintf(debug_fp, "%s(%d) - %s", script_file, no, snr[no]);
					fputs("�߸��� �����Դϴ�.\n", debug_fp);
				}
				else m_valuemap[hash(buffer)] = (int)vr;
			}
		}
		else if(debug_mode)
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, no, snr[no]);
			fputs("������ �������� �ʽ��ϴ�.\n", debug_fp);
		}

		return TRUE;
	}
	//���� ���� ����
	else if(snr[no][0]=='{')
	{
		//���� ���� �̸� �о���̱�
		int i;
		for(i=1; snr[no][i]!='}' && snr[no][i] != NULL; ++i)buffer[i-1] = snr[no][i];
		buffer[i-1] = NULL;

		//���� �̸� �ȿ� ������ ���� ���
		if(IncludeValue(buffer))
		{
			char tmp_buffer[80];
			strcpy(tmp_buffer, buffer);
			ConvertString(buffer, tmp_buffer);
		}

		//���� �б�
		char* start_point = strchr(snr[no], '"');
		
		if(start_point == NULL)	//�߸��� ���� ����
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, no, snr[no]);
			fputs(" : \"\"�ȿ� ������ �����ʽÿ�.\n", debug_fp);
			return TRUE;	
		}

		int value_length=(int)(strrchr(snr[no], '"') - start_point) - 1;

		if(value_length > 0)
		{
			//���� ����
			char tmp_buffer[80];
			strncpy(tmp_buffer, start_point+1, value_length);
			tmp_buffer[value_length] = NULL;

			if(IncludeValue(tmp_buffer))	//������ ���� ���� ��
			{
				char tmp_buffer2[80];
				strcpy(tmp_buffer2, tmp_buffer);
				ConvertString(tmp_buffer, tmp_buffer2);
			}

			//����Ʈ ��
			int text_id = hash(buffer);
			if(m_textmap.find(text_id)==m_textmap.end())
			{
				m_textmap[text_id].show = false;
				m_textmap[text_id].shadow = false;
				m_textmap[text_id].color = JColor(0,0,0);
			}

			strcpy(m_textmap[text_id].buffer, tmp_buffer);
		}
		else	// "�� �ϳ��� ����
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, no, snr[no]);
			fputs(" : \"\"�ȿ� ������ �����ʽÿ�.\n", debug_fp);
			return TRUE;
		}

		return TRUE;
	}
	//�Ϲ� ��ȭ
	else if(snr[no][0]!='@' && snr[no][0]!='~')
	{
		temp_command.id=COMMAND_PRINT;
		
		//������ �־� ��ȯ�� �ʿ��� ���� ���ۻ��, �ƴϸ� ������ ���
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

	//���� ����� ��� �д��� ����
	if(snr[no][0]=='~')read_next=TRUE;

	//����� �о� ���̱�
	int value_id=0;		//����� ��� ���°���� Ȯ��
	size_t i;			//���� �а� �ִ� ��ġ
	size_t strmax = strlen(snr[no]);	//���ڿ� ũ��
	for(i=1; i < strmax; ++i)
	{
		if(snr[no][i]==' ' || snr[no][i]==0x09)break;

		buffer[i-1]=snr[no][i];	//���� ����
	}

	//��ɾ��� ID �˾Ƴ���
	buffer[i-1]=NULL;
	temp_command.id=GetCommandID(hash(buffer));

	//��¹� ��� ������ ���
	if(temp_command.id == COMMAND_PRINT)
	{
		temp_command.text=&snr[no][i+1];
		m_script->push_back(temp_command);
		return FALSE;
	}

	//�� �о� ���̱�
	for(; i < strmax; ++i)
	{
		//��� �б�
		if(snr[no][i]>='0' && snr[no][i]<='9' || snr[no][i]=='-')
		{
			int value_length=1;
			char* start_point = &snr[no][i++];

			for(; i < strmax && snr[no][i]>='0' && snr[no][i]<='9'; ++i)++value_length;	//���� ���

			//���� ����
			strncpy(buffer, start_point, value_length);
			buffer[value_length] = NULL;

			//�Է��� �� �ִ� ���� �Ѱ踦 ������ �Է� �Ұ�
			if(value_id < SCRIPT_VALUE_MAX)
			{
				temp_command.value[value_id]=atoi(buffer);
				++value_id;
			}
		}
		//����
		else if(snr[no][i]=='"')
		{
			int value_length=0;
			char* start_point = &snr[no][++i];

			for(;  i < strmax && snr[no][i]!='"'; ++i)++value_length;	//���� ���

			//���� ����
			strncpy(temp_command.buffer, start_point, value_length);
			temp_command.buffer[value_length] = NULL;

			//������ ���� ��ȯ
			if(IncludeValue(temp_command.buffer))
			{
				char temp[80];
				strcpy(temp, temp_command.buffer);
				ConvertString(temp_command.buffer, temp);
			}
		}
		//���� �б�
		else if(snr[no][i]=='[')
		{
			int value_length=0;
			//i++;
			char* start_point = &snr[no][++i];

			for(; i < strmax && snr[no][i]!=']'; ++i)++value_length;	//���� ���

			//���� �̸� ����
			strncpy(buffer, start_point, value_length);
			buffer[value_length] = NULL;

			//���� ���� ã�Ƽ� �ѱ�
			if(value_id < SCRIPT_VALUE_MAX)
			{
				temp_command.value[value_id]=m_valuemap[hash(buffer)];
				++value_id;
			}
		}
		//���ڿ� �б�
		else if(snr[no][i]!=' ' && snr[no][i]!=0x09 && snr[no][i]!=',')
		{
			//�ܼ� ���ڿ����� ������ ���� ���ڿ����� Ȯ��
			if(temp_command.id!=COMMAND_PRINT && IncludeValue(&snr[no][i]))	//�ٸ� ������ ���� ���� ��
			{
				ConvertString(temp_command.buffer, &snr[no][i]);
			}
			else	//�ܼ� ������ ��
			{
				temp_command.text=&snr[no][i];	//���� ���� ��ġ
			}

			break;
		}
	}

	//����Ʈ ��
	if(temp_command.id==COMMAND_SHOWPICTURE)
	{
		if(value_id<4)temp_command.value[3] = 100;		//�������� ���� ������ �⺻ 100
		if(value_id<5)temp_command.value[4] = 1;		//�ݺ� ���� ���� ������ �⺻ 1
	}
	else if(temp_command.id==COMMAND_HIDEPICTURE)
	{
		if(value_id<1)temp_command.value[0] = -1;	//�׸� ����� ��ȣ�� ���� ������ ��� ����
	}

	//��ɹ��� ť�� ����
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

//==================== ��ũ��Ʈ ó�� =======================//
void CScript::RunScript()
{
	if(m_script.empty())return;	//�ƹ� �۾��� �� ��� ���� ��

	//�׸� ǥ��
	std::map<int, ScriptPicture>::iterator pit;
	for(pit=m_pic.begin(); pit!=m_pic.end(); ++pit)
	{
		if((pit->second).show)
		{
			int px = (pit->second).x;
			int py = (pit->second).y;

			for(int i=0; i<pit->second.loop; ++i)
			{
				if((pit->second).ani)	//�ִϸ��̼��� ���
				{
					//������
					if((pit->second).filp)
					{
						animation.GetAni((pit->second).id)->DrawEx(jdd->GetBackBufferID(), (pit->second).id, px, py, (pit->second).filp);
					}
					//�Ϲ� ���
					else
					{
						animation.GetAni((pit->second).id)->Draw(jdd->GetBackBufferID(), (pit->second).id, px, py);
					}
				}
				else	//�Ϲ� �׸��� ���
				{
					//������
					if((pit->second).filp)
					{
						jdd->DrawPictureEx(jdd->GetBackBufferID(), (pit->second).id, px, py, NULL, (pit->second).filp);
					}
					//�Ϲ� ���
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

	//���� ǥ��
	std::map<int, ScriptText>::iterator tit;
	for(tit=m_textmap.begin(); tit!=m_textmap.end(); ++tit)
	{
		if((tit->second).show)
		{
			//�׸��ڰ� �ִ� ���
			if((tit->second).shadow)
			{
				jdd->DrawText(backbuffer, (tit->second).buffer, global_font, (tit->second).x+1, (tit->second).y+1, JColor(0, 0, 0));
			}
			//�� ����
			jdd->DrawText(backbuffer, (tit->second).buffer, global_font, (tit->second).x, (tit->second).y, (tit->second).color);
		}
	}

	//Ŀ�� ǥ��
	if(area_select)jdd->DrawPicture(backbuffer, area_cursor, area_x, area_y, NULL);

	//���� ����
	if(music_type == MUSIC_WAV)
	{
		HRESULT hr;
		if(FAILED(hr = m_music.ProcessDirectMusicMessages())) 
		{
			DXTRACE_ERR_MSGBOX( TEXT("ProcessDirectMusicMessages"), hr );
			return;
		}
	}

	//ť ���� ��� �۾��� �������� �˻��ϴ� �÷���, �ϴ� �ѹ��� ������ �۾��� ��κ��̹Ƿ� ����Ʈ�� TRUE
	BOOL done=TRUE;

	//��� ����
	std::vector<CScriptCommand>::iterator it;
	for(it=m_script.begin(); it!=m_script.end(); ++it)
	{
		if(it->id<0);	//�ƹ��͵� �ƴ� ��ɾ�
		
		//���
		else if(it->id==COMMAND_PRINT)
		{
			if(!it->first)	//�� ó���� ��ȭ�� ����
			{
				//���� ������ ���� ���� ���۷� ����, �ƴϸ� ���ڿ� �����ͷ� ����
				if(strcmp(it->buffer, "")==0)m_dlg.MakeText(it->text);
					else m_dlg.MakeText(it->buffer);

				it->first = true;
			}

			//��ȭ ����� ������ ����
			int result=m_dlg.Printing();
			if(result!=999 && result!=-1)done=FALSE;
				else DELETE_COMMAND;
		}
		//����
		else if(it->id==COMMAND_SELECT)
		{
			//����â ����
			if(!it->first)
			{
				if(strcmp(it->buffer, "")==0)m_select.MakeSelection(it->text);
					else m_select.MakeSelection(it->buffer);
				m_select.SetSelect(0);	//��ġ �ʱ�ȭ

				it->first = true;
			}

			//������ ���� ������ ����â ǥ��
			int result=m_select.Selecting();
			if(result==-1 || result==-999)done=FALSE;
			else
			{
				select_value=result;
				DELETE_COMMAND;
			}
		}
		//���� �Է�
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
					fputs(" : ���� ���� ������ ���� �ʾҽ��ϴ�.\n", debug_fp);
				}
				else
				{
					m_valuemap[hash(it->buffer)] = atoi(input_buffer);
				}

				DELETE_COMMAND;
			}
		}
		//���� �Է�
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
					fputs(" : ���� ���� ������ ���� �ʾҽ��ϴ�.\n", debug_fp);
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
		//���� ���� ���
		else if(it->id >= COMMAND_SHOWTEXT && it->id <= COMMAND_HIDESHADOW)
		{
			//������ �������� �˻�
			if(!it->first && strcmp(it->buffer, "")==0 && it->id<=COMMAND_MOVETEXT)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : ���ں��� �̸��� ���� �ʾҽ��ϴ�.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}

			int tmp_id = hash(it->buffer);

			//���ڰ� �����ϴ��� �˻�
			if(!it->first && m_textmap.find(tmp_id) == m_textmap.end() && it->id<=COMMAND_MOVETEXT)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : �ش� ���� ������ ã�� �� �����ϴ�.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}

			ScriptText* p_text = &m_textmap[tmp_id];	//������� ���� ������

			//���� ǥ��
			if(it->id==COMMAND_SHOWTEXT)
			{
				//�⺻ �Ӽ�
				p_text->show=TRUE;
				p_text->x=it->value[0];
				p_text->y=it->value[1];
				
				DELETE_COMMAND;
			}
			//���� �����
			else if(it->id==COMMAND_HIDETTEXT)
			{
				m_textmap[tmp_id].show=FALSE;
				DELETE_COMMAND;
			}
			//���� �̵�
			else if(it->id==COMMAND_MOVETEXT)
			{
				//��ȭġ ���
				static double move_x, move_y;	//�ѹ��� ������ �Ÿ�
				if(!it->first)
				{
					it->value[3] = Max(it->value[2], 1);	//�����ð��� �ּ� 1
					move_x = (double)it->value[0] / (double)it->value[2];
					move_y = (double)it->value[1] / (double)it->value[2];
					p_text->rx = (double)p_text->x;
					p_text->ry = (double)p_text->y;
					it->first = true;
				}

				//�̵�
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
			//���ڻ�
			else if(it->id==COMMAND_TEXTCOLOR)
			{
				if(tmp_id == 0)	//��ü ���ڻ� ����
				{
					m_dlg.SetColor(it->value[0], it->value[1], it->value[2]);
					m_select.SetColor(it->value[0], it->value[1], it->value[2]);
					m_input.SetColor(it->value[0], it->value[1], it->value[2]);
					for(int i=0; i<3; ++i)text_rgb[i] = it->value[i];
				}
				else	//Ư�� ���ڻ� ����
				{
					p_text->color.r = it->value[0];
					p_text->color.g = it->value[1];
					p_text->color.b = it->value[2];
				}

				DELETE_COMMAND;
			}
			//�׸��� ���̱�
			else if(it->id==COMMAND_SHOWSHADOW)
			{
				if(tmp_id == 0)	//��ü ���� �׸���
				{
					m_dlg.ShowShadow(true);
					m_select.ShowShadow(true);
					m_input.ShowShadow(true);
					text_shadow=true;
				}
				else	//Ư�� ���� �׸���
				{
					p_text->shadow = true;
				}

				DELETE_COMMAND;
			}
			//�׸��� �����
			else if(it->id==COMMAND_HIDESHADOW)
			{
				if(tmp_id == 0)	//��ü ���� �׸���
				{
					m_dlg.ShowShadow(false);
					m_select.ShowShadow(false);
					m_input.ShowShadow(false);
					text_shadow=false;
				}
				else	//Ư�� ���� �׸���
				{
					p_text->shadow = false;
				}

				DELETE_COMMAND;
			}
		}
		//�۲� ����
		else if(it->id==COMMAND_FONT)
		{
			jdd->DeleteFont(global_font);
			global_font=jdd->CreateFont(it->buffer, 20);

			DELETE_COMMAND;
		}
		//��ȭâ ����
		else if(it->id==COMMAND_WINDOW)
		{
			//�Է�â, ������, ��ȭâ ��� ���� �Ӽ����� ����
			m_dlg.SetDlg(it->value[0], it->value[1], it->value[2], it->value[3]);
			m_select.SetDlg(it->value[0], it->value[1], it->value[2], it->value[3]);

			DELETE_COMMAND;
		}
		//��ȭâ ������
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
				fputs(" : �߸��� ������ �Է��߽��ϴ�.\n", debug_fp);
			}

			DELETE_COMMAND;
		}
		//�׸� �б�
		else if(it->id==COMMAND_PICTURE)
		{
			//�߸��� ID �Է�
			if(it->value[0] < 0)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : �߸��� �׸���ȣ�� �Է��߽��ϴ�.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}

			char* pic_name=StrAdd("ScriptPicture%d", it->value[0]);		//Scirpt+�׸�ID�� �׸� ����
			bool load_first = false;	//�� ó������ �׸��� �о������ ����
			if(m_pic.find(it->value[0]) == m_pic.end())load_first=true;
			ScriptPicture* p_pic = &m_pic[it->value[0]];	//������� �׸� ������

			jdd->DeleteSurface(pic_name);
			if(!jdd->DuplicateSurface(pic_name, it->buffer, &p_pic->id))
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : �׸��� ã�� �� �����ϴ�.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}
			strcpy(p_pic->src, it->buffer);	//�׸� �̸� ����

			//�׸� ���� ������ �α�
			JPictureInfo pi;
			jdd->GetPictureInfo(it->buffer, &pi);
			p_pic->key = pi.GetColorKey();
			p_pic->opacity = pi.GetOpacity();
			p_pic->filp = 0;	//������ ���� ���°� �⺻

			//�ִϿ����� ���
			if(it->value[1] > 0)
			{
				int ani_x = pi.GetWidth() / (it->value[1]);	//������ ���� �°� ���� ���� ������
				int ani_y = pi.GetHeight();					//���δ� ������ �׸� ũ�⿡ ����

				//����Ʈ �����ð�
				it->value[2] = Max(it->value[2], 1);

				//�ִϸ��̼� ���� Ȯ��
				if(it->value[3] == 0)animation.CreateAnimation(p_pic->id, ani_x, ani_y, ani_loop, it->value[1], it->value[2]);
				else if(it->value[3] == 2)animation.CreateAnimation(p_pic->id, ani_x, ani_y, ani_once, it->value[1], it->value[2]);
				else animation.CreateAnimation(p_pic->id, ani_x, ani_y, ani_exchange, it->value[1], it->value[2]);

				p_pic->frame = it->value[1];
				p_pic->delay = it->value[2];
				p_pic->anitype = it->value[3];
				p_pic->ani=TRUE;
			}
			else p_pic->ani=FALSE;

			if(load_first)p_pic->show=FALSE;	//ó�� �о� �� ��쿡�� �׸��� ������ �ʴ� ������ �ʱ�ȭ

			DELETE_COMMAND;
		}
		//�׸� ���� ���
		else if(it->id >= COMMAND_SHOWPICTURE && it->id <= COMMAND_VFLIP)
		{
			//�׸� ��� �����(Ư�� ���)
			if(it->id==COMMAND_HIDEPICTURE && it->value[0] == -1)
			{
				std::map<int, ScriptPicture>::iterator git;
				for(git = m_pic.begin(); git != m_pic.end(); ++git)git->second.show = FALSE;
				
				DELETE_COMMAND;
				continue;
			}


			//�׸��� �����ϴ��� �˻�
			if(!it->first && m_pic.find(it->value[0]) == m_pic.end())
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : �׸��� ã�� �� �����ϴ�.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}

			ScriptPicture* p_pic = &m_pic[it->value[0]];	//������� �׸� ������

			//�׸� ǥ��
			if(it->id==COMMAND_SHOWPICTURE)
			{
				//�⺻ �Ӽ�
				p_pic->show=TRUE;
				p_pic->x=it->value[1];
				p_pic->y=it->value[2];
				
				//����
				JPictureInfo pi;
				p_pic->opacity=((float)it->value[3])/100;
				pi.SetColorKey(p_pic->key);
				pi.SetOpacity(p_pic->opacity);
				jdd->SetPictureInfo(p_pic->id, &pi);

				//�ݺ����
				p_pic->loop = it->value[4];
				p_pic->gap_x = it->value[5];
				p_pic->gap_y = it->value[6];

				//�ִϸ��̼��� ��� �ǰ���
				if(p_pic->ani)animation.GetAni(p_pic->id)->Reset();

				DELETE_COMMAND;
			}
			//�׸� �̵�
			else if(it->id==COMMAND_MOVEPICTURE)
			{
				//��ȭġ ���
				static double move_x, move_y;	//�ѹ��� ������ �Ÿ�
				if(!it->first)
				{
					it->value[3] = Max(it->value[3], 1);	//�����ð��� �ּ� 1
					move_x = (double)it->value[1] / (double)it->value[3];
					move_y = (double)it->value[2] / (double)it->value[3];
					p_pic->rx = (double)p_pic->x;
					p_pic->ry = (double)p_pic->y;
					it->first = true;
				}

				//�̵�
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
			//�׸� ����
			else if(it->id==COMMAND_PICTUREOPACITY)
			{
				//��ȭġ ���
				static float change;
				if(!it->first)
				{
					it->value[2] = Max(it->value[2], 1);	//�����ð��� �ּ� 1
					change = (((float)it->value[1]/100) - p_pic->opacity)/(float)it->value[2];
					it->first = true;
				}

				if(it->value[2]>0)
				{
					//����
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
			//�׸� �����
			else if(it->id==COMMAND_HIDEPICTURE)
			{
				p_pic->show=FALSE;
				DELETE_COMMAND;
			}
			//���� ������
			else if(it->id==COMMAND_HFLIP)
			{
				if(p_pic->filp & DPX_HFLIP)p_pic->filp -= DPX_HFLIP;
					else p_pic->filp |= DPX_HFLIP;

				DELETE_COMMAND;
			}
			//�¿� ������
			else if(it->id==COMMAND_VFLIP)
			{
				if(p_pic->filp & DPX_VFLIP)p_pic->filp -= DPX_VFLIP;
					else p_pic->filp |= DPX_VFLIP;
				
				DELETE_COMMAND;
			}
		}
		//ȭ�� ȿ��
		else if(it->id==COMMAND_SCREENEFFECT)
		{
			//���� �� ����� �۾�
			static double now_screen, screen_change;
			if(!it->first)
			{
				//ȭ�� ȿ�� ����
				screen_effect[0]=it->value[0];
				screen_effect[1]=it->value[1];

				//��ȭġ ���
				it->value[3] = Max(it->value[3], 1);	//�����ð��� �ּ� 1
				screen_change = (double)(it->value[2] - it->value[1])/(double)it->value[3];
				now_screen = (double)it->value[1];
				it->first = true;
			}

			//��ȭ
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
		//ȿ���� �ҷ�����
		else if(it->id==COMMAND_EFFECT)
		{
			//���� ȿ������ ����� ���ο� ȿ���� ����
			SndObjDestroy(m_snd[it->value[0]].sound);
			m_snd[it->value[0]].sound = SndObjCreate(SoundOBJ, it->buffer, 2);

			if(m_snd[it->value[0]].sound == NULL)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : ���� ������ ã�� �� �����ϴ�.\n", debug_fp);
			}
			else strcpy(m_snd[it->value[0]].src, it->buffer);	//�ҽ� ���� ���

			DELETE_COMMAND;
		}
		//ȿ���� ���
		else if(it->id==COMMAND_EFFECTPLAY)
		{
			if(m_snd.find(it->value[0]) == m_snd.end())
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" :  �Ҹ��� ã�� �� �����ϴ�.\n", debug_fp);

				DELETE_COMMAND;
				continue;
			}

			_Play(m_snd[it->value[0]].sound);
			DELETE_COMMAND;
		}
		//����� �б�
		else if(it->id==COMMAND_MUSICPLAY)
		{
			char ext[20];
			strcpy(ext, strrchr(it->buffer, '.'));

			if(ext == NULL)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : �ùٸ� �������� �̸��� �ƴմϴ�.\n", debug_fp);
			}
			else if(strcmp(ext, ".mid") == 0)
			{
				if(!_MidiPlay(it->buffer))
				{
					DebugMode();
					fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
					fputs(" : MIDI ������ ������ �� �����ϴ�.\n", debug_fp);
					music_type = MUSIC_MID;
				}
			}
			else if(strcmp(ext, ".wav") == 0)
			{
				if(m_music.LoadSegmentFile(it->buffer) == S_FALSE)
				{
					DebugMode();
					fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
					fputs(" : WAV ������ ã�� �� �����ϴ�.\n", debug_fp);
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
					fputs(" : OGG ������ ���� �� �����ϴ�.\n", debug_fp);
					music_type = MUSIC_OGG;
				}

				m_ogg->PlayOgg(true);
			}
			
			strcpy(bgm_name, it->buffer);
			DELETE_COMMAND;
		}
		//BGM ����
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
		//������ ���
		else if(it->id==COMMAND_AVIPLAY)
		{
			_PlayAVI(it->buffer);

			DELETE_COMMAND;
		}
		//AŰ
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
		//SŰ
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
		//DŰ
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
		//FŰ
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
		//�� ȭ��ǥ
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
		//�Ʒ� ȭ��ǥ
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
		//���� ȭ��ǥ
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
		//������ ȭ��ǥ
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
		//����Ű
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
		//����
		else if(it->id==COMMAND_RANDOM)
		{
			m_valuemap[hash(it->buffer)] = (rand()%(abs(it->value[1] - it->value[0]))+1) + it->value[0];

			DELETE_COMMAND;
		}
		//��� ���
		else if(it->id==COMMAND_WAIT)
		{
			if(it->value[0]>0)
			{
				--(it->value[0]);
				done=FALSE;
			}
			else DELETE_COMMAND;
		}
		//�̵�
		else if(it->id==COMMAND_GOTO)
		{
			if(it->value[0] >= 0)	//���̳ʽ� ���� ����
			{
				return_no.push_back(script_no);	//���ƿ� �ּ� ����
				script_no = m_BookmarkHash[it->value[0]];	//�̵�
			}

			DELETE_COMMAND;
		}
		//�б�
		else if(it->id==COMMAND_CASE)
		{
			if(select_value < SCRIPT_VALUE_MAX && it->value[select_value] >= 0)
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[select_value]];
				select_value = 0;	//������ �ʱ�ȭ
			}

			DELETE_COMMAND;
		}
		//���ư���
		else if(it->id==COMMAND_RETURN)
		{
			//���Ϳ��� ���ư� �ּҸ� ���������� ������
			if(!return_no.empty())
			{
				std::vector<int>::iterator rit = return_no.end();
				rit--;
				script_no = (*rit);

				//�������� ����
				return_no.pop_back();
			}
			DELETE_COMMAND;
		}
		//�ٸ� ��ũ��Ʈ �ε�
		else if(it->id==COMMAND_LOAD)
		{
			int start_no = it->value[0]; //���� �ּ� �ܿ��α�
			UnLoad();
			Load(it->buffer);
			script_no = m_BookmarkHash[start_no];
			done=TRUE;
			break;
		}
		//�ε� �޴� �θ���
		else if(it->id==COMMAND_LOADMENU)
		{
			on_loadmenu=true;
			DELETE_COMMAND;
		}
		//���̺� �޴� �θ���
		else if(it->id==COMMAND_SAVEMENU)
		{
			on_savemenu=true;
			DELETE_COMMAND;
		}
		//é�͸� ����
		else if(it->id==COMMAND_CHAPTER)
		{
			if(strcmp(it->buffer, "")==0)
			{
				DebugMode();
				fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
				fputs(" : é�͸��� ���� �ʾҽ��ϴ�.\n", debug_fp);
			}

			strcpy(chapter_name, it->buffer);
			DELETE_COMMAND;
		}
		//���̺� ����, �Ұ���
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
		//��������
		else if(it->id==COMMAND_SELECTAREA)
		{
			//�ʱ� ����
			if(!it->first)
			{
				area_select = true;
				strcpy(area_cursor, it->buffer);

				it->first = true;				
			}

			done=false;

			//�¿��̵�
			if(GetKey(vkey_left, 1))
			{
				area_x = Max(area_x-5, 0);
			}
			else if(GetKey(vkey_right, 1))
			{
				area_x = Min(area_x+5, SCREEN_X);
			}
			//�����̵�
			if(GetKey(vkey_up, 1))
			{
				area_y = Max(area_y-5, 0);
			}
			else if(GetKey(vkey_down, 1))
			{
				area_y = Min(area_y+5, SCREEN_Y);
			}
			//����
			if(GetKey(vkey_enter))
			{
				area_select = false;
				done=true;
				DELETE_COMMAND;
			}
		}
		//�������ÿ� ���� �б�
		else if(it->id==COMMAND_GOTOAREA)
		{
			if(area_x >= it->value[1] && area_x <= it->value[3] && area_y >= it->value[2] && area_y <= it->value[4])
			{
				return_no.push_back(script_no);
				script_no = m_BookmarkHash[it->value[select_value]];
			}

			DELETE_COMMAND;
		}
		//������
		else if(it->id==COMMAND_END)
		{
			script_end = true;
			DELETE_COMMAND;
		}
	}

	//��� ����� �Ϸ�Ǿ����� ť ����
	if(done)m_script.clear();
}