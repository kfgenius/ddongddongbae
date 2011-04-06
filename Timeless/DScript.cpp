//2007. 12. 30
#include <windows.h>

#include <string>

#include "DScript.h"
#include "calc.h"
#include "DlgEncoder.h"

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
#define MUSIC_OGG				3

//�ý��� ����
#define CURSOR_SOUND			0
#define SELECT_SOUND			1

//����ȭ�� ���� ��ũ��
#define DELETE_COMMAND			it->id=-1	//��ɹ� ���

//Ŀ�ǵ� �Ϸ� ��Ȳ
#define RUN_NOTEND				-1
#define RUN_WELL				0
#define RUN_END					1
#define RUN_CLEAR				99

//�ؽ� �Լ�(�����̺� �� ������)
#define HASH_NUMBER 37
#define HASH_SIZE 262139

#define KEY1 8			//��ũ��Ʈ ���� �ص��� ���� Ű1
#define KEY2 4			//��ũ��Ʈ ���� �ص��� ���� Ű2

BOOL script_mouse;

//////////////////////////////////////////
//���� �Լ�

static int hash(char* sz)	//�ؽ��� ���ϱ�
{
	if(sz==NULL) return 0;
	unsigned int hashvalue=0;
	for(int lp=0;sz[lp];lp++)
		hashvalue=(hashvalue*HASH_NUMBER+sz[lp])%HASH_SIZE;
	return hashvalue;
}

//������ �������� �˻�
BOOL IncludeValue(char* str)
{
	if(strchr(str, '[')!=NULL || strchr(str, '{')!=NULL)return TRUE;
		else return FALSE;
}

//�빮�ڸ� �ҹ��ڷ�
void ToSmallCase(char* str)
{
	size_t strmax = strlen(str);
	for(size_t i=0; i<strmax; ++i)
		if(str[i] >= 'A' && str[i] <= 'Z')str[i]+=0x20;
}

//////////////////////////////////////////////////////
// ���� ����(�̸� ���صδ� �ý��ۺ��� �ؽð�)
int area_x_hash = hash("#Ŀ��X");
int area_y_hash = hash("#Ŀ��Y");
int area_cursor_hash = hash("#Ŀ��");
int select_hash = hash("#����");
int chapter_hash = hash("#é��");
int bgm_hash = hash("#�����");
int system_load_hash = hash("#�ε�ȭ��");
int system_save_hash = hash("#���̺�ȭ��");
int system_menu_hash = hash("#�ý��۸޴�");
int system_cursor_hash = hash("#�ý���Ŀ��");

//�ý��� ����
HSNDOBJ sound[2];

/////////////////////////////////////////////////////
//��ũ��Ʈ ��� �޼ҵ�
CScriptCommand::CScriptCommand()
{
	//�� �ʱ�ȭ
	ZeroMemory(value, sizeof(int)*SCRIPT_VALUE_MAX);
	text=NULL;
	strcpy(buffer,"");
	first=false;
}

/////////////////////////////////////////////////////
//��ũ��Ʈ �޼ҵ�
void CScript::ClearValueMap()	//���� �� Ŭ����
{
	m_valuemap.clear();
	m_textmap.clear();
}

//��ȭ ���� �ҷ�����
CScript::CScript(char* script_file)
{	
	Load(script_file);
	if(!enable)return;	//�ʱ�ȭ ���н� ���

	CreateCommandMap();

	//����
	if(SoundOBJ)m_ogg = new COgg(SoundOBJ);
		else m_ogg = NULL;

	//��Ÿ
	debug_mode = false;	
	text_rgb[0]=text_rgb[1]=text_rgb[2]=0;
	text_shadow=false;
	strcpy(font_name, "�ü�");
	message_speed = 4;
	text_skip = false;
	text_auto = false;
	ready = FALSE;
	end = FALSE;
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
	RunCommand.clear();

	//������ ��ü
	ClearValueMap();

	//���� �÷��̾� ����
	if(m_ogg)
	{
		delete m_ogg;
		m_ogg = NULL;
	}
}

void CScript::Load(char* script_file)
{
	char tmp_file[256];
	//dlg�� �б�
	sprintf(tmp_file, "%s.dlg", script_file);
	if(fp=fopen(tmp_file, "rb"))
	{
		strcpy(this->script_file, script_file);
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
		if(fp=fopen(tmp_file, "rb"))
		{
			strcpy(this->script_file, script_file);
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

	//é�͸� ���
	char* temp_chapter = strrchr(script_file, '\\');
	if(temp_chapter == NULL)strcpy(m_textmap[chapter_hash].buffer, script_file);
		else strcpy(m_textmap[chapter_hash].buffer, temp_chapter+1);

	//ȭ��ȿ�� �ʱ�ȭ
	screen_effect[0] = 0;
	screen_effect[1] = 0;

	//���� ���� �ʱ�ȭ
	strcpy(m_textmap[bgm_hash].buffer, "");

	//��Ÿ
	area_select = false;
	m_valuemap[area_x_hash] = SCREEN_WIDTH/2;
	m_valuemap[area_y_hash] = SCREEN_HEIGHT/2;
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

	//å���� ��ü
	m_BookmarkHash.clear();

	//���� ��ü
	m_pic.clear();
	m_move_x.clear();
	m_move_y.clear();

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

	//ȭ��ȿ�� �ʱ�ȭ
	screen_effect[0] = 0;
	screen_effect[1] = 0;

	//���� �Ҹ� ����
	music_type = -1;
	_MidiStop();
	if(m_ogg)m_ogg->StopOgg();

	enable = FALSE;
}

//��ũ��Ʈ ������ ����
BOOL CScript::SetPage(int no)
{
	if(!enable || m_BookmarkHash.find(no) == m_BookmarkHash.end())	//�ش� �������� ã�� �� ����
	{
		return FALSE;
	}
	else
	{
		script_no=m_BookmarkHash[no];	//��ũ��Ʈ ������
		m_script.clear();
		ready = TRUE;
		return TRUE;
	}
}
//��ũ��Ʈ ó��
void CScript::Scripting()
{
	if(!enable)	//�������� ������ �� �� �� ����
	{
		gameover = TRUE;
		return;
	}
	//���� ���� �����̰ų� �غ� ���� �ʾ����� ��ũ��Ʈ�� ó������ ����
	else if(gameover || !ready)
	{
		return;
	}

	//��ɹ��� ������� ������
	if(m_script.empty())
	{
		BringScript();
		if(script_no < 0)return;
	}

	//���콺 ó��
	static BOOL left_click;
	script_mouse = FALSE;
	if(LButton)
	{
		if(!left_click)
		{
			left_click = TRUE;
			script_mouse = TRUE;
		}
	}
	else left_click = FALSE;

	RunScript();	//��ũ��Ʈ ó��

	//ȭ�� ȿ��
	if(screen_effect[1] > 0)
	{
		if(screen_effect[0] == SE_FADE)
		{
			jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_fadeout(screen_effect[1]));
		}
		else if(screen_effect[0] == SE_WHITE)
		{
				jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_whiteout(screen_effect[1]));
		}
		else if(screen_effect[0] == SE_GRAY)
		{
			jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_partial_gray(screen_effect[1]));
		}
		else if(screen_effect[0] == SE_HUE)
		{
			jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_hue_change(screen_effect[1]*3));
		}
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

	//���� �˻�
	if(script_no < 0 || (script_no >= snrs && m_script.empty()) || !enable)
	{
		end = TRUE;
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
		read_next = ReadScript(&m_script, script_no);
		++script_no;
	}while(read_next);
}

//��ɹ� �� ����
void CScript::CreateCommandMap()
{	
	//�ؽ�Ʈ ���� ��ɹ�
	RunCommand[hash("���")]=&CScript::ComPrint;
	RunCommand[hash("����")]=&CScript::ComSelect;
	RunCommand[hash("�����Է�")]=&CScript::ComInputNum;
	RunCommand[hash("�����Է�")]=&CScript::ComInputText;
	RunCommand[hash("����ǥ��")]=&CScript::ComShowText;
	RunCommand[hash("���ڼ���")]=&CScript::ComHideText;
	RunCommand[hash("�����̵�")]=&CScript::ComMoveText;

	//�ؽ�Ʈ ���� ��ɹ�
	RunCommand[hash("��ȭâ")]=&CScript::ComDlgBox;
	RunCommand[hash("�Է�â")]=&CScript::ComInputBox;
	RunCommand[hash("â������")]=&CScript::ComWindowOpacity;
	RunCommand[hash("���ڻ�")]=&CScript::ComTextColor;
	RunCommand[hash("�׸���ǥ��")]=&CScript::ComShowShadow;
	RunCommand[hash("�׸��ڼ���")]=&CScript::ComHideShadow;
	RunCommand[hash("�۲�")]=&CScript::ComSetFont;

	//�׸� ���� ��ɹ�
	RunCommand[hash("�׸�")]=&CScript::ComPicture;
	RunCommand[hash("�׸�ǥ��")]=&CScript::ComShowPicture;
	RunCommand[hash("�׸��̵�")]=&CScript::ComMovePicture;
	RunCommand[hash("�׸�����")]=&CScript::ComHidePicture;
	RunCommand[hash("�׸�������")]=&CScript::ComPictureOpacity;
	RunCommand[hash("�¿����")]=&CScript::ComHFlip;
	RunCommand[hash("���Ϲ���")]=&CScript::ComVFlip;
	RunCommand[hash("ȭ��ȿ��")]=&CScript::ComScreenEffect;

	//���� ���� ��ɹ�
	RunCommand[hash("�������")]=&CScript::ComPlayBGM;
	RunCommand[hash("��������")]=&CScript::ComStopBGM;
	RunCommand[hash("�Ҹ�")]=&CScript::ComSound;
	RunCommand[hash("�Ҹ����")]=&CScript::ComPlaySound;
	RunCommand[hash("�ý��ۼҸ�")]=&CScript::ComSystemSound;
	RunCommand[hash("AVI")]=&CScript::ComAVI;

	//Ű�Է� ��ɹ�
	RunCommand[hash("AŰ")]=&CScript::ComKey_A;
	RunCommand[hash("SŰ")]=&CScript::ComKey_S;
	RunCommand[hash("DŰ")]=&CScript::ComKey_D;
	RunCommand[hash("FŰ")]=&CScript::ComKey_F;
	RunCommand[hash("��Ű")]=&CScript::ComKey_UP;
	RunCommand[hash("��Ű")]=&CScript::ComKey_DOWN;
	RunCommand[hash("��Ű")]=&CScript::ComKey_LEFT;
	RunCommand[hash("��Ű")]=&CScript::ComKey_RIGHT;
	RunCommand[hash("����Ű")]=&CScript::ComKey_ENTER;

	//���콺 ��ɹ�
	RunCommand[hash("���콺����")]=&CScript::ComEnableMouse;
	RunCommand[hash("Ŭ��")]=&CScript::ComLeftClick;

	//����
	RunCommand[hash("����")]=&CScript::ComRandom;

	//��ũ��Ʈ �帧 ����
	RunCommand[hash("���")]=&CScript::ComWait;
	RunCommand[hash("�б�")]=&CScript::ComCase;
	RunCommand[hash("�̵�")]=&CScript::ComGoto;
	RunCommand[hash("���ư�")]=&CScript::ComReturn;
	RunCommand[hash("��������")]=&CScript::ComSelectArea;
	RunCommand[hash("�����б�")]=&CScript::ComGotoByArea;
	RunCommand[hash("���")]=&CScript::ComInfinity;

	//����
	RunCommand[hash("��")]=&CScript::ComEnd;
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
	int depth = 0;
	if(snr[no][0]=='[')
	{
		//���� �̸� �о���̱�
		int i;
		for(i=1; snr[no][i] != NULL; ++i)
		{
			if(snr[no][i]==']')
			{
				if(depth<=0)break;
					else --depth;
			}
			else if(snr[no][i]=='[')++depth;

			buffer[i-1] = snr[no][i];
		}

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
				
				fprintf(debug_fp, "1: %s\n", calc);
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
		for(i=1; snr[no][i] != NULL; ++i)
		{
			if(snr[no][i]=='}')
			{
				if(depth<=0)break;
					else --depth;
			}
			else if(snr[no][i]=='{')++depth;

			buffer[i-1] = snr[no][i];
		}
		buffer[i-1] = NULL;

		//���� �̸� �ȿ� ������ ���� ���
		if(IncludeValue(buffer))
		{
			char tmp_buffer[80];
			strcpy(tmp_buffer, buffer);
			ConvertString(buffer, tmp_buffer);
		}

		//������
		char* start_point = strchr(snr[no], '"');
		
		if(start_point == NULL)	//�߸��� ���� ����
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, no, snr[no]);
			fputs(" : \"\"�ȿ� ������ �����ʽÿ�.\n", debug_fp);
			return TRUE;	
		}

		//��
		char* end_point = strrchr(snr[no], '"');

		if(end_point==start_point)	// "�� �ϳ��� ����
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, no, snr[no]);
			fputs(" : ������ \"�� �����ϴ�.\n", debug_fp);
			return TRUE;
		}

		//���� ����
		int value_length=(int)(end_point - start_point) - 1;

		if(value_length >= 0)
		{
			char tmp_buffer[80];
			if(value_length==0)	//�� ����
			{
				strcpy(tmp_buffer, "");
			}
			else	//���� ����
			{
				strncpy(tmp_buffer, start_point+1, value_length);
				tmp_buffer[value_length] = NULL;
			}

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

		return TRUE;
	}
	//�Ϲ� ��ȭ
	else if(snr[no][0]!='@' && snr[no][0]!='~')
	{
		temp_command.id=hash("���");
		
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
	temp_command.id=hash(buffer);

	//��¹�, ���ù� ��� ������ ���
	if(temp_command.id == hash("���") || temp_command.id == hash("����"))
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
			if(temp_command.id!=hash("���") && IncludeValue(&snr[no][i]))	//�ٸ� ������ ���� ���� ��
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
	if(temp_command.id==hash("�׸�ǥ��"))
	{
		if(value_id<4)temp_command.value[3] = 100;		//�������� ���� ������ �⺻ 100
		if(value_id<5)temp_command.value[4] = 1;		//�ݺ� ���� ���� ������ �⺻ 1
	}
	else if(temp_command.id==hash("�׸�����"))
	{
		if(value_id<1)temp_command.value[0] = -1;	//�׸� ����� ��ȣ�� ���� ������ ��� ����
	}
	else if(temp_command.id==hash("Ŭ��"))
	{
		if(value_id<2)		//������ ���� ���ϸ� ��ü ����
		{
			temp_command.value[1] = 0;
			temp_command.value[2] = 0;
			temp_command.value[3] = 640;
			temp_command.value[4] = 480;
		}
	}
	/*else if(temp_command.id==hash("����"))
	{
		if(value_id<1)temp_command.value[0] = -1;	//��ǥ�� �� ������ �ڵ� ����
	}*/

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
			JPictureInfo pi;
			pi.SetColorKey((pit->second).key);
			pi.SetOpacity((pit->second).opacity);
			jdd->SetPictureInfo((pit->second).id, &pi);

			for(int i=0; i<pit->second.loop; ++i)
			{
				if((pit->second).ani)	//�ִϸ��̼��� ���
				{
					//������
					if((pit->second).flip)
					{
						animation.GetAni((pit->second).id)->DrawEx(jdd->GetBackBufferID(), (pit->second).id, px, py, (pit->second).flip);
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
					if((pit->second).flip)
					{
						jdd->DrawPictureEx(jdd->GetBackBufferID(), (pit->second).id, px, py, NULL, (pit->second).flip);
					}
					//�Ϲ� ���
					else
					{
						jdd->DrawPicture(jdd->GetBackBufferID(), (pit->second).id, px, py, NULL);
					}
				}

				//������ ����� ���� ��ǥ�� �̵��ϸ� ���
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

	//ť ���� ��� �۾��� �������� �˻��ϴ� �÷���, �ϴ� �ѹ��� ������ �۾��� ��κ��̹Ƿ� ����Ʈ�� TRUE
	BOOL done=TRUE;

	//��� ����
	std::vector<CScriptCommand>::iterator it;
	for(it=m_script.begin(); it!=m_script.end(); ++it)
	{
		if(RunCommand.find(it->id) != RunCommand.end())
		{
			int result = (this->*RunCommand[it->id])(it);
			
			if(result == RUN_NOTEND)done=FALSE;
				else if(result == RUN_END)DELETE_COMMAND;
				else if(result == RUN_CLEAR)
				{
					done=TRUE;
					break;
				}
		}
	}

	//��� ����� �Ϸ�Ǿ����� ť ����
	if(done)m_script.clear();

	//Ŀ�� ǥ��
	if(area_select)jdd->DrawPicture(backbuffer,m_textmap[area_cursor_hash].buffer, m_valuemap[area_x_hash], m_valuemap[area_y_hash], NULL);
	else if(mouse_control)jdd->DrawPicture(backbuffer,m_textmap[area_cursor_hash].buffer, MouseX, MouseY, NULL);
}

/////////////////////////////////////////////////////////////////
//���� ��ɹ�

//������ �̵�
void CScript::Goto(int goto_page, int return_page)
{
	return_no.push_back(return_page);	//���ƿ� �ּ� ����
	script_no = m_BookmarkHash[goto_page];	//�̵�
}

//�׸� ID �޾ƿ���
ScriptPicture* CScript::CheckPicture(COMMAND_PTR it)
{
	//�׸��� �����ϴ��� �˻�
	if(!it->first && m_pic.find(it->value[0]) == m_pic.end())
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : �׸��� ã�� �� �����ϴ�.\n", debug_fp);

		return NULL;
	}

	return &m_pic[it->value[0]];	//������� �׸� ������
}

//���ۿ� ���ڸ� �������� �˻�
BOOL CScript::CheckBuffer(COMMAND_PTR it)
{
	//������ �������� �˻�
	if(!it->first && strcmp(it->buffer, "")==0)
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : ���ں��� �̸��� ���� �ʾҽ��ϴ�.\n", debug_fp);

		return FALSE;
	}
	else return TRUE;
}

//���ڰ� ���ڸʿ� �����ϴ� �˻�
ScriptText* CScript::GetTextPoint(COMMAND_PTR it, BOOL error_msg)
{
	int tmp_id = hash(it->buffer);

	//���ڰ� �����ϴ��� �˻�
	if(!it->first && m_textmap.find(tmp_id) == m_textmap.end())
	{
		if(error_msg)
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
			fputs(" : �ش� ���� ������ ã�� �� �����ϴ�.\n", debug_fp);
		}

		return NULL;
	}

	return &m_textmap[tmp_id];	//������� ���� ������
}

/////////////////////////////////////////////////////////////////
//��ɹ� ó��

//���
int CScript::ComPrint(COMMAND_PTR it)
{
	if(!it->first)	//�� ó���� ��ȭ�� ����
	{
		//���� ������ ���� ���� ���۷� ����, �ƴϸ� ���ڿ� �����ͷ� ����
		if(strcmp(it->buffer, "")==0)m_dlg.MakeText(it->text);
			else m_dlg.MakeText(it->buffer);

		it->first = true;
	}

	//��ȭ ���
	int result = 0;
	static int text_delay;
	if(text_delay == 0 || text_skip)
	{
		result = m_dlg.Printing();
		text_delay = message_speed;
	}
	else
	{
		m_dlg.ShowText();
		--text_delay;
	}

	//��ȭ ����� ������ ����
	if(result == 999 || result == -1)
	{
		return RUN_END;
	}
	else
	{
		return RUN_NOTEND;
	}
}

//����
int CScript::ComSelect(COMMAND_PTR it)
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
	if(result==-1 || result==-999)return RUN_NOTEND;
	else
	{
		m_valuemap[select_hash]=result;
		return RUN_END;
	}
}

//���� �Է�
int CScript::ComInputNum(COMMAND_PTR it)
{
	if(!CheckBuffer(it))return RUN_END;	//���ۿ� ���ڸ� ���� �ʾ���

	//�Է�â ȯ�� ����
	if(!it->first)
	{
		m_input.SetFlag(FLAG_NUMBER);
		m_input.Clear();
		it->first=true;
	}

	//�Է�
	int result=m_input.Inputing();
	if(result==0)return RUN_NOTEND;
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

		return RUN_END;
	}
}

//���� �Է�
int CScript::ComInputText(COMMAND_PTR it)
{
	if(!CheckBuffer(it))return RUN_END;	//���ۿ� ���ڸ� ���� �ʾ���

	//�Է�â ȯ�� ����
	if(!it->first)
	{
		m_input.SetFlag(FLAG_TEXT | FLAG_NUMBER | FLAG_MARK);
		m_input.Clear();
		it->first=true;
	}

	//�Է�
	int result=m_input.Inputing();
	if(result==0)return RUN_NOTEND;
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

		return RUN_END;
	}
}

//���� ���̱�
int CScript::ComShowText(COMMAND_PTR it)
{
	if(!CheckBuffer(it))return RUN_END;	//���ۿ� ���ڸ� ���� �ʾ���

	//���ڸʿ� �����ϴ��� �˻�
	ScriptText* p_text = GetTextPoint(it);
	if(!p_text)return RUN_END;

	//�⺻ �Ӽ�
	p_text->show=TRUE;
	p_text->x=it->value[0];
	p_text->y=it->value[1];
	
	return RUN_END;
}

//���� �����
int CScript::ComHideText(COMMAND_PTR it)
{
	if(!CheckBuffer(it))return RUN_END;	//���ۿ� ���ڸ� ���� �ʾ���

	//���ڸʿ� �����ϴ��� �˻�
	ScriptText* p_text = GetTextPoint(it);
	if(p_text)p_text->show=FALSE;

	return RUN_END;
}

//���� �̵�
int CScript::ComMoveText(COMMAND_PTR it)
{
	if(!CheckBuffer(it))return RUN_END;	//���ۿ� ���ڸ� ���� �ʾ���

	//���ڸʿ� �����ϴ��� �˻�
	ScriptText* p_text = GetTextPoint(it);
	if(!p_text)return RUN_END;

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

		return RUN_NOTEND;
	}
	else return RUN_END;
}

//���ڻ�
int CScript::ComTextColor(COMMAND_PTR it)
{
	//���ڸʿ� �����ϴ��� �˻�
	ScriptText* p_text = GetTextPoint(it, false);

	if(hash(it->buffer) == 0)	//��ü ���ڻ� ����
	{
		m_dlg.SetTextColor(it->value[0], it->value[1], it->value[2]);
		m_select.SetTextColor(it->value[0], it->value[1], it->value[2]);
		m_input.SetTextColor(it->value[0], it->value[1], it->value[2]);
		for(int i=0; i<3; ++i)text_rgb[i] = it->value[i];
	}
	else if(p_text)	//Ư�� ���ڻ� ����
	{
		p_text->color.r = it->value[0];
		p_text->color.g = it->value[1];
		p_text->color.b = it->value[2];
	}

	return RUN_END;
}

//�׸��� ���̱�
int CScript::ComShowShadow(COMMAND_PTR it)
{
	//���ڸʿ� �����ϴ��� �˻�
	ScriptText* p_text = GetTextPoint(it, false);

	if(hash(it->buffer) == 0)	//��ü ���� �׸���
	{
		m_dlg.ShowShadow();
		m_select.ShowShadow();
		m_input.ShowShadow();
		text_shadow = true;
	}
	else if(p_text)
	{
		p_text->shadow = true;	//Ư�� ���� �׸���
	}

	return RUN_END;
}

//�׸��� �����
int CScript::ComHideShadow(COMMAND_PTR it)
{
	//���ڸʿ� �����ϴ��� �˻�
	ScriptText* p_text = GetTextPoint(it, false);

	if(hash(it->buffer) == 0)	//��ü ���� �׸���
	{
		m_dlg.HideShadow();
		m_select.HideShadow();
		m_input.HideShadow();
		text_shadow = false;
	}
	else if(!p_text)p_text->shadow = false;	//Ư�� ���� �׸���

	return RUN_END;
}

//��Ʈ ����
int CScript::ComSetFont(COMMAND_PTR it)
{
	jdd->DeleteFont(global_font);
	strcpy(font_name, it->buffer);
	global_font=jdd->CreateFont(font_name, 20);

	return RUN_END;
}

//��ȭâ ����
int CScript::ComDlgBox(COMMAND_PTR it)
{
	//������, ��ȭâ�� ���� �Ӽ����� ����
	m_dlg.SetDlg(it->value[0], it->value[1], it->value[2], it->value[3]);
	m_select.SetDlg(it->value[0], it->value[1], it->value[2], it->value[3]);

	//��ȭâ �׸� ����
	m_dlg.MakeDlgBox("_textbox");
	m_select.SetDlgBox("_textbox");
	
	return RUN_END;
}

//�Է�â ����
int CScript::ComInputBox(COMMAND_PTR it)
{
	m_input.SetDlg(it->value[0],it->value[1],it->value[2]);
	
	//�Է�â �׸� ����
	m_input.MakeDlgBox("_inputbox");

	return RUN_END;
}

//â ������
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
		fputs(" : �߸��� ������ �Է��߽��ϴ�.\n", debug_fp);
	}

	return RUN_END;
}

//�׸� �б�
int CScript::ComPicture(COMMAND_PTR it)
{
	//�߸��� ID �Է�
	if(it->value[0] < 0)
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : �߸��� �׸���ȣ�� �Է��߽��ϴ�.\n", debug_fp);

		return RUN_END;
	}

	bool load_first = false;	//�� ó������ �׸��� �о������ ����
	if(m_pic.find(it->value[0]) == m_pic.end())load_first=true;
	ScriptPicture* p_pic = &m_pic[it->value[0]];	//������� �׸� ������

	JPictureInfo pi;
	if(!jdd->GetPictureInfo(it->buffer, &pi))
	{
		//�ε�� ���� ������ ���Ϸ� ã��
		pi.SetColorKey(JColor(0, 0, 255));
		if(!jdd->LoadPicture(it->buffer, it->buffer, &pi, true))
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
			fputs(" : �׸��� ã�� �� �����ϴ�.\n", debug_fp);

			return RUN_END;
		}
	}
	p_pic->id = jdd->GetID(it->buffer);
	strcpy(p_pic->src, it->buffer);	//�׸� �̸� ����

	//�׸� ���� ������ �α�
	p_pic->key = pi.GetColorKey();
	p_pic->opacity = pi.GetOpacity();
	p_pic->flip = 0;	//������ ���� ���°� �⺻

	//�ִϸ޼��� ���
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

	return RUN_END;
}

//�׸� ���̱�
int CScript::ComShowPicture(COMMAND_PTR it)
{
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//�׸��� �������� ����

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

	return RUN_END;
}

//�׸� �̵�
int CScript::ComMovePicture(COMMAND_PTR it)
{
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//�׸��� �������� ����

	//��ȭġ ���
	if(!it->first)
	{
		it->value[3] = Max(it->value[3], 1);	//�����ð��� �ּ� 1
		m_move_x[it->value[0]] = (double)it->value[1] / (double)it->value[3];
		m_move_y[it->value[0]] = (double)it->value[2] / (double)it->value[3];
		p_pic->rx = (double)p_pic->x;
		p_pic->ry = (double)p_pic->y;
		it->first = true;
	}

	//�̵�
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

//�׸� �����
int CScript::ComHidePicture(COMMAND_PTR it)
{
	//�׸� ��� �����(Ư�� ���)
	if(it->value[0] == -1)
	{
		std::map<int, ScriptPicture>::iterator git;
		for(git = m_pic.begin(); git != m_pic.end(); ++git)git->second.show = FALSE;
		
		return RUN_END;
	}

	//�ϳ��� �����
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//�׸��� �������� ����

	p_pic->show=FALSE;

	return RUN_END;
}

//�׸� ����
int CScript::ComPictureOpacity(COMMAND_PTR it)
{
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//�׸��� �������� ����

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

		return RUN_NOTEND;
	}
	else return RUN_END;
}

//���� ����
int CScript::ComVFlip(COMMAND_PTR it)
{
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//�׸��� �������� ����

	if(p_pic->flip & DPX_VFLIP)p_pic->flip &= (0xffff - DPX_VFLIP);
		else p_pic->flip |= DPX_VFLIP;
	
	return RUN_END;
}

//�¿� ����
int CScript::ComHFlip(COMMAND_PTR it)
{
	ScriptPicture* p_pic = CheckPicture(it);
	if(!p_pic)return RUN_END;	//�׸��� �������� ����

	if(p_pic->flip & DPX_HFLIP)p_pic->flip &= (0xffff - DPX_HFLIP);
		else p_pic->flip |= DPX_HFLIP;

	return RUN_END;
}

//ȭ�� ȿ��
int CScript::ComScreenEffect(COMMAND_PTR it)
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

		return RUN_NOTEND;
	}
	else return RUN_END;
}

//����� ���
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
	else if(strcmp(ext, ".ogg") == 0)
	{
		if(!m_ogg || !m_ogg->OpenOgg(it->buffer))
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
			fputs(" : OGG ������ ���� �� �����ϴ�.\n", debug_fp);
			music_type = MUSIC_OGG;
		}
		else m_ogg->PlayOgg(true);
	}
	else
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : �������� �ʴ� ���� �����Դϴ�.\n", debug_fp);
		music_type = MUSIC_OGG;
	}
	
	strcpy(m_textmap[bgm_hash].buffer, it->buffer);

	return RUN_END;
}

//����� ����
int CScript::ComStopBGM(COMMAND_PTR it)
{
	music_type = -1;
	_MidiStop();
	if(m_ogg)m_ogg->StopOgg();

	strcpy(m_textmap[bgm_hash].buffer, "");

	return RUN_END;
}

//���� �б�
int CScript::ComSound(COMMAND_PTR it)
{
	//���� ȿ������ ����� ���ο� ȿ���� ����
	if(SoundOBJ)
	{
		SndObjDestroy(m_snd[it->value[0]].sound);
		m_snd[it->value[0]].sound = SndObjCreate(SoundOBJ, it->buffer, 2);
	}

	if(SoundOBJ && m_snd[it->value[0]].sound == NULL)
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : ���� ������ ã�� �� �����ϴ�.\n", debug_fp);
	}
	else strcpy(m_snd[it->value[0]].src, it->buffer);	//�ҽ� ���� ���

	return RUN_END;
}

//���� ���
int CScript::ComPlaySound(COMMAND_PTR it)
{
	if(m_snd.find(it->value[0]) == m_snd.end())
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" :  �Ҹ��� ã�� �� �����ϴ�.\n", debug_fp);

		return RUN_END;
	}

	_Play(m_snd[it->value[0]].sound);

	return RUN_END;
}

//�ý��� ����
int CScript::ComSystemSound(COMMAND_PTR it)
{
	if(strcmp(script_file, "SYSTEM") != 0)return RUN_END;	//�ý��ۿ����� ��밡��

	if(it->value[0] < 2)
	{
		//���� ȿ������ ����� ���ο� ȿ���� ����
		if(SoundOBJ)
		{
			SndObjDestroy(sound[it->value[0]]);
			sound[it->value[0]] = SndObjCreate(SoundOBJ, it->buffer, 2);
		}

		if(SoundOBJ && sound[it->value[0]] == NULL)
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
			fputs(" : ���� ������ ã�� �� �����ϴ�.\n", debug_fp);
		}
	}

	return RUN_END;
}

//AVI ���
int CScript::ComAVI(COMMAND_PTR it)
{
	_PlayAVI(it->buffer);

	return RUN_END;
}

//AŰ
int CScript::ComKey_A(COMMAND_PTR it)
{
	if(GetKey(vkey_a))
	{
		if(it->value[0] >= 0)Goto(it->value[0], script_no);
		return RUN_CLEAR;
	}

	return RUN_WELL;
}

//SŰ
int CScript::ComKey_S(COMMAND_PTR it)
{
	if(GetKey(vkey_s))
	{
		if(it->value[0] >= 0)Goto(it->value[0], script_no);
		return RUN_CLEAR;
	}

	return RUN_WELL;
}

//DŰ
int CScript::ComKey_D(COMMAND_PTR it)
{
	if(GetKey(vkey_d))
	{
		if(it->value[0] >= 0)Goto(it->value[0], script_no);
		return RUN_CLEAR;
	}

	return RUN_WELL;
}

//FŰ
int CScript::ComKey_F(COMMAND_PTR it)
{
	if(GetKey(vkey_f))
	{
		if(it->value[0] >= 0)Goto(it->value[0], script_no);
		return RUN_CLEAR;
	}

	return RUN_WELL;
}

//�� Ű
int CScript::ComKey_UP(COMMAND_PTR it)
{
	if(GetKey(vkey_up))
	{
		if(it->value[0] >= 0)Goto(it->value[0], script_no);
		return RUN_CLEAR;
	}

	return RUN_WELL;
}

//�Ʒ� Ű
int CScript::ComKey_DOWN(COMMAND_PTR it)
{
	if(GetKey(vkey_down))
	{
		if(it->value[0] >= 0)Goto(it->value[0], script_no);
		return RUN_CLEAR;
	}

	return RUN_WELL;
}

//���� Ű
int CScript::ComKey_LEFT(COMMAND_PTR it)
{
	if(GetKey(vkey_left))
	{
		if(it->value[0] >= 0)Goto(it->value[0], script_no);
		return RUN_CLEAR;
	}

	return RUN_WELL;
}

//������ Ű
int CScript::ComKey_RIGHT(COMMAND_PTR it)
{
	if(GetKey(vkey_right))
	{
		if(it->value[0] >= 0)Goto(it->value[0], script_no);
		return RUN_CLEAR;
	}

	return RUN_WELL;
}

//����Ű �Է�
int CScript::ComKey_ENTER(COMMAND_PTR it)
{
	if(GetKey(vkey_enter))
	{
		if(it->value[0] >= 0)Goto(it->value[0], script_no);
		return RUN_CLEAR;
	}

	return RUN_WELL;
}

//���콺 ����
int CScript::ComEnableMouse(COMMAND_PTR it)
{
	if(strcmp(script_file, "SYSTEM") != 0)return RUN_END;	//�ý��ۿ����� ��밡��

	mouse_control = TRUE;

	return RUN_END;
}

//���콺 ���� Ŭ��
int CScript::ComLeftClick(COMMAND_PTR it)
{
	if(mouse_control && script_mouse && it->value[0] >= 0)
	{
		//���� �˻�
		if(MouseX >= it->value[1] && MouseX < it->value[3] && MouseY >= it->value[2] && MouseY < it->value[4])
		{
			Goto(it->value[0], script_no);
			LButton = FALSE;
			return RUN_CLEAR;
		}
	}

	return RUN_WELL;
}

//���� ���� �߻�
int CScript::ComRandom(COMMAND_PTR it)
{
	m_valuemap[hash(it->buffer)] = (rand()%(abs(it->value[1] - it->value[0]))+1) + it->value[0];

	return RUN_END;
}

//���
int CScript::ComWait(COMMAND_PTR it)
{
	if(it->value[0]>0)
	{
		--(it->value[0]);
		return RUN_NOTEND;
	}
	else return RUN_END;
}

//�������� ���� �̵�
int CScript::ComCase(COMMAND_PTR it)
{
	if(m_valuemap[select_hash] < SCRIPT_VALUE_MAX && it->value[m_valuemap[select_hash]] >= 0)
	{
		Goto(it->value[m_valuemap[select_hash]], script_no);
	}

	return RUN_END;
}

//�̵�
int CScript::ComGoto(COMMAND_PTR it)
{
	//���̳ʽ� ���� ����
	if(it->value[0] >= 0)Goto(it->value[0], script_no);

	return RUN_END;
}

//���ư���
int CScript::ComReturn(COMMAND_PTR it)
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

	return RUN_END;
}

//���� ����
int CScript::ComSelectArea(COMMAND_PTR it)
{
	//�ʱ� ����
	if(!it->first)
	{
		area_select = true;
		if(it->buffer)strcpy(m_textmap[area_cursor_hash].buffer, it->buffer);

		it->first = true;
	}

	//�¿��̵�
	if(GetKey(vkey_left, 1))
	{
		m_valuemap[area_x_hash] = Max(m_valuemap[area_x_hash]-5, 0);
	}
	else if(GetKey(vkey_right, 1))
	{
		m_valuemap[area_x_hash] = Min(m_valuemap[area_x_hash]+5, SCREEN_WIDTH);
	}
	//�����̵�
	if(GetKey(vkey_up, 1))
	{
		m_valuemap[area_y_hash] = Max(m_valuemap[area_y_hash]-5, 0);
	}
	else if(GetKey(vkey_down, 1))
	{
		m_valuemap[area_y_hash] = Min(m_valuemap[area_y_hash]+5, SCREEN_HEIGHT);
	}

	//���콺�� Ŀ���̵�
	if(mouse_control && mouse_move)
	{
		m_valuemap[area_x_hash] = MouseX;
		m_valuemap[area_y_hash] = MouseY;
		mouse_move = FALSE;
	}

	//����
	if(GetKey(vkey_enter) || mouse_control && LeftDown())
	{
		area_select = false;
		return RUN_CLEAR;
	}

	return RUN_NOTEND;
}

//�������ÿ� ���� �̵�
int CScript::ComGotoByArea(COMMAND_PTR it)
{
	if(m_valuemap[area_x_hash] >= it->value[1] && m_valuemap[area_x_hash] < it->value[3] && m_valuemap[area_y_hash] >= it->value[2] && m_valuemap[area_y_hash] < it->value[4])
	{
		Goto(it->value[0], script_no);
	}

	return RUN_END;
}

//��� ���
int CScript::ComInfinity(COMMAND_PTR it)
{
	ready = FALSE;

	return RUN_END;
}

//��ũ��Ʈ ������
int CScript::ComEnd(COMMAND_PTR it)
{
	end = TRUE;

	return RUN_CLEAR;
}

//��ŵ
void CScript::SetTextSkip(BOOL on_off)
{
	text_skip = on_off;
}

//����
void CScript::SetTextAuto(BOOL on_off)
{
	m_dlg.SetTextAuto(on_off);
}

//��ŵ ���� �˾ƿ���
BOOL CScript::GetTextSkip()
{
	return text_skip;
}

//���� ���� �˾ƿ���
BOOL CScript::GetTextAuto()
{
	return m_dlg.GetTextAuto();
}

//�غ� ���� Ȯ��
BOOL CScript::IsReady()
{
	return ready;
}

//���� ���� Ȯ��
BOOL CScript::IsEnd()
{
	return end;
}