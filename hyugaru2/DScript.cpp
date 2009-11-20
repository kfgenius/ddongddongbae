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
#define SE_QUAKE				7

//����ȭ�� ���� ��ũ��
#define DELETE_COMMAND			it->id=-1	//��ɹ� ���

//Ŀ�ǵ� �Ϸ� ��Ȳ
#define RUN_NOTEND				-1
#define RUN_WELL				0
#define RUN_END					1
#define RUN_CLEAR				99
#define SCRIPT_END				-99

//�ؽ�
#define HASH(x)					jdd->GetHash(x)

//////////////////////////////////////////
//���� �Լ�

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

//��ȭ ���� �ҷ�����
CScript::CScript(HSNDOBJ* m_snd, map<int, int> *m_valuemap, map<int, char*> *m_textmap)
{	
	CreateCommandMap();

	//����
	if(SoundOBJ)m_ogg = new COgg(SoundOBJ);
		else m_ogg = NULL;
	
	this->m_snd = m_snd;

	//������
	this->m_valuemap = m_valuemap;
	this->m_textmap = m_textmap;

	//��Ÿ
	debug_mode = false;
	enable = FALSE;
}

//��ȭ���� ����
CScript::~CScript()
{
	//����� ���� �ݱ�
	if(debug_mode)fclose(debug_fp);

	//��ũ��Ʈ ���� ��ε�
	if(enable)UnLoad();

	//��ɹ� ����
	m_script.clear();

	//��ɹ� �� ��ü
	RunCommand.clear();

	//���� �÷��̾� ����
	if(m_ogg)
	{
		delete m_ogg;
		m_ogg = NULL;
	}
}

void CScript::Load(char* script_file)
{
	if(enable)UnLoad();	//�̹� �ε�Ǿ� ������ ���� ��ũ��Ʈ ��ε�

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

	//��ũ��Ʈ �д� ��ġ �ʱ�ȭ
	script_no = 0;
	script_end = TRUE;

	//ȭ��ȿ�� �ʱ�ȭ
	screen_effect[0] = 0;
	screen_effect[1] = 0;
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

	//���� ���� ����
	return_no.clear();

	//ȭ��ȿ�� �ʱ�ȭ
	screen_effect[0] = 0;
	screen_effect[1] = 0;

	//���� �Ұ��� ���·� ����
	enable=FALSE;
}

//������ ����
BOOL CScript::SetPage(int script_no)
{
	//�������� �������� �ʰų� ��ũ��Ʈ�� ������ ���� ��
	if(!enable || m_BookmarkHash.find(script_no) == m_BookmarkHash.end())
	{
		script_end = TRUE;
		return FALSE;
	}
	//�̻��� ���� ��
	else
	{
		this->script_no=m_BookmarkHash[script_no];
		script_end = FALSE;
		return TRUE;
	}
}

//��ũ��Ʈ ó��
BOOL CScript::Script()
{
	if(!enable || script_end)return FALSE;

	//��ũ��Ʈ ����
	if(m_script.empty())
	{
		BringScript();
	}
	//��ũ��Ʈ ó��
	else
	{
		RunScript();
	}

	return !script_end;
}

void CScript::BringScript()
{
	BOOL read_next;	//����ؼ� ���� ����� �д��� ����

	do{
		//��ũ��Ʈ ����
		if(script_no >= snrs)
		{
			script_end = TRUE;
			break;
		}

		read_next=ReadScript();
		++script_no;
	}while(read_next);
}

//��ɹ� �� ����
void CScript::CreateCommandMap()
{	
	//�ؽ�Ʈ ���� ��ɹ�
	RunCommand[HASH("���")]=&CScript::ComPrint;
	RunCommand[HASH("����")]=&CScript::ComSelect;
	RunCommand[HASH("�����Է�")]=&CScript::ComInputNum;
	RunCommand[HASH("�����Է�")]=&CScript::ComInputText;

	//�ؽ�Ʈ ���� ��ɹ�
	RunCommand[HASH("��ȭâ")]=&CScript::ComDlgBox;
	RunCommand[HASH("�Է�â")]=&CScript::ComInputBox;
	RunCommand[HASH("â������")]=&CScript::ComWindowOpacity;
	RunCommand[HASH("���ڻ�")]=&CScript::ComTextColor;
	RunCommand[HASH("�׸���ǥ��")]=&CScript::ComShowShadow;
	RunCommand[HASH("�׸��ڼ���")]=&CScript::ComHideShadow;

	//�׸� ���� ��ɹ�
	RunCommand[HASH("�׸�")]=&CScript::ComPicture;
	RunCommand[HASH("�׸�ǥ��")]=&CScript::ComShowPicture;
	RunCommand[HASH("�׸��̵�")]=&CScript::ComMovePicture;
	RunCommand[HASH("�׸�����")]=&CScript::ComHidePicture;
	RunCommand[HASH("�׸�������")]=&CScript::ComPictureOpacity;
	RunCommand[HASH("�¿����")]=&CScript::ComHFlip;
	RunCommand[HASH("���Ϲ���")]=&CScript::ComVFlip;
	RunCommand[HASH("ȭ��ȿ��")]=&CScript::ComScreenEffect;

	//���� ���� ��ɹ�
	RunCommand[HASH("�������")]=&CScript::ComPlayBGM;
	RunCommand[HASH("��������")]=&CScript::ComStopBGM;
	RunCommand[HASH("�Ҹ����")]=&CScript::ComPlaySound;
	RunCommand[HASH("AVI")]=&CScript::ComAVI;

	//����
	RunCommand[HASH("����")]=&CScript::ComRandom;

	//��ũ��Ʈ �帧 ����
	RunCommand[HASH("���")]=&CScript::ComWait;
	RunCommand[HASH("�б�")]=&CScript::ComCase;
	RunCommand[HASH("�̵�")]=&CScript::ComGoto;
	RunCommand[HASH("���ư�")]=&CScript::ComReturn;
	RunCommand[HASH("��")]=&CScript::ComEnd;
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
			_itoa_s((*m_valuemap)[HASH(buffer)], num_buffer, 40, 10);
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
			strcat(result, (*m_textmap)[HASH(buffer)]);
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
BOOL CScript::ReadScript()	//�����ϴ� ���� ����ؼ� ��ũ��Ʈ�� �д��� �ϴ� ��
{
	CScriptCommand temp_command;
	BOOL read_next=FALSE;
	char buffer[80];	//�ٿ뵵 ����

	temp_command.snr_no = script_no;

	//���� ����
	int depth = 0;
	if(snr[script_no][0]=='[')
	{
		//���� �̸� �о���̱�
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

		if(IncludeValue(&snr[script_no][i+1]))	//�ٸ� ������ ���� ���� ��
		{
			ConvertString(text_buffer, &snr[script_no][i+1]);
			calc = text_buffer;
		}
		else	//�ܼ� ������ ��
		{
			calc = strchr(snr[script_no], ' ');
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
				fprintf(debug_fp, "%s(%d) - %s", script_file, script_no, snr[script_no]);
				fputs("�߸��� �����Դϴ�.\n", debug_fp);
				
				fprintf(debug_fp, "1: %s\n", calc);
			}
			else
			{
				px=ev.Evaluate(vr, vals);
				if(px)
				{
					DebugMode();
					fprintf(debug_fp, "%s(%d) - %s", script_file, script_no, snr[script_no]);
					fputs("�߸��� �����Դϴ�.\n", debug_fp);
				}
				else (*m_valuemap)[HASH(buffer)] = (int)vr;
			}
		}
		else if(debug_mode)
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, script_no, snr[script_no]);
			fputs("������ �������� �ʽ��ϴ�.\n", debug_fp);
		}

		return TRUE;
	}
	//���� ���� ����
	else if(snr[script_no][0]=='{')
	{
		//���� ���� �̸� �о���̱�
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

		//���� �̸� �ȿ� ������ ���� ���
		if(IncludeValue(buffer))
		{
			char tmp_buffer[80];
			strcpy(tmp_buffer, buffer);
			ConvertString(buffer, tmp_buffer);
		}

		//������
		char* start_point = strchr(snr[script_no], '"');
		
		if(start_point == NULL)	//�߸��� ���� ����
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, script_no, snr[script_no]);
			fputs(" : \"\"�ȿ� ������ �����ʽÿ�.\n", debug_fp);
			return TRUE;	
		}

		//��
		char* end_point = strrchr(snr[script_no], '"');

		if(end_point==start_point)	// "�� �ϳ��� ����
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, script_no, snr[script_no]);
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

			//���ں��� �ֱ�
			int text_id = HASH(buffer);
			if((*m_textmap)[text_id] != NULL)delete[] (*m_textmap)[text_id];
			(*m_textmap)[text_id] = new char[strlen(tmp_buffer) + 1];
			strcpy((*m_textmap)[text_id], tmp_buffer);
		}

		return TRUE;
	}
	//�Ϲ� ��ȭ
	else if(snr[script_no][0]!='@' && snr[script_no][0]!='~')
	{
		temp_command.id=HASH("���");
		
		//������ �־� ��ȯ�� �ʿ��� ���� ���ۻ��, �ƴϸ� ������ ���
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

	//���� ����� ��� �д��� ����
	if(snr[script_no][0]=='~')read_next=TRUE;

	//����� �о� ���̱�
	int value_id=0;		//����� ��� ���°���� Ȯ��
	size_t i;			//���� �а� �ִ� ��ġ
	size_t strmax = strlen(snr[script_no]);	//���ڿ� ũ��
	for(i=1; i < strmax; ++i)
	{
		if(snr[script_no][i]==' ' || snr[script_no][i]==0x09)break;

		buffer[i-1]=snr[script_no][i];	//���� ����
	}

	//��ɾ��� ID �˾Ƴ���
	buffer[i-1]=NULL;
	temp_command.id=HASH(buffer);

	//��¹�, ���ù� ��� ������ ���
	if(temp_command.id == HASH("���") || temp_command.id == HASH("����"))
	{
		temp_command.text=&snr[script_no][i+1];
		m_script.push_back(temp_command);
		return FALSE;
	}

	//�� �о� ���̱�
	for(; i < strmax; ++i)
	{
		//��� �б�
		if(snr[script_no][i]>='0' && snr[script_no][i]<='9' || snr[script_no][i]=='-')
		{
			int value_length=1;
			char* start_point = &snr[script_no][i++];

			for(; i < strmax && snr[script_no][i]>='0' && snr[script_no][i]<='9'; ++i)++value_length;	//���� ���

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
		else if(snr[script_no][i]=='"')
		{
			int value_length=0;
			char* start_point = &snr[script_no][++i];

			for(;  i < strmax && snr[script_no][i]!='"'; ++i)++value_length;	//���� ���

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
		else if(snr[script_no][i]=='[')
		{
			int value_length=0;
			//i++;
			char* start_point = &snr[script_no][++i];

			for(; i < strmax && snr[script_no][i]!=']'; ++i)++value_length;	//���� ���

			//���� �̸� ����
			strncpy(buffer, start_point, value_length);
			buffer[value_length] = NULL;

			//���� ���� ã�Ƽ� �ѱ�
			if(value_id < SCRIPT_VALUE_MAX)
			{
				temp_command.value[value_id]=(*m_valuemap)[HASH(buffer)];
				++value_id;
			}
		}
		//���ڿ� �б�
		else if(snr[script_no][i]!=' ' && snr[script_no][i]!=0x09 && snr[script_no][i]!=',')
		{
			//�ܼ� ���ڿ����� ������ ���� ���ڿ����� Ȯ��
			if(temp_command.id!=HASH("���") && IncludeValue(&snr[script_no][i]))	//�ٸ� ������ ���� ���� ��
			{
				ConvertString(temp_command.buffer, &snr[script_no][i]);
			}
			else	//�ܼ� ������ ��
			{
				temp_command.text=&snr[script_no][i];	//���� ���� ��ġ
			}

			break;
		}
	}

	//����Ʈ ��
	if(temp_command.id==HASH("�׸�ǥ��"))
	{
		if(value_id<4)temp_command.value[3] = 100;		//�������� ���� ������ �⺻ 100
		if(value_id<5)temp_command.value[4] = 1;		//�ݺ� ���� ���� ������ �⺻ 1
	}
	else if(temp_command.id==HASH("�׸�����"))
	{
		if(value_id<1)temp_command.value[0] = -1;	//�׸� ����� ��ȣ�� ���� ������ ��� ����
	}
	else if(temp_command.id==HASH("Ŭ��"))
	{
		if(value_id<2)		//������ ���� ���ϸ� ��ü ����
		{
			temp_command.value[1] = 0;
			temp_command.value[2] = 0;
			temp_command.value[3] = 640;
			temp_command.value[4] = 480;
		}
	}
	else if(temp_command.id==HASH("����"))
	{
		if(value_id<1)temp_command.value[0] = -1;	//��ǥ�� �� ������ �ڵ� ����
	}

	//��ɹ��� ť�� ����
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

	//ȭ�� ȿ��
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

	//ť ���� ��� �۾��� �������� �˻��ϴ� �÷���, �ϴ� �ѹ��� ������ �۾��� ��κ��̹Ƿ� ����Ʈ�� TRUE
	BOOL done=TRUE;

	//��� ����
	std::vector<CScriptCommand>::iterator it;
	for(it=m_script.begin(); it!=m_script.end(); ++it)
	{
		if(RunCommand.find(it->id) != RunCommand.end())
		{
			int result = (this->*RunCommand[it->id])(it);
			
			//����� ������� ����
			if(result == RUN_NOTEND)
			{
				done=FALSE;
			}
			//����� ����Ǿ���
			else if(result == RUN_END)
			{
				DELETE_COMMAND;
			}
			//������ ��ɱ��� ����
			else if(result == RUN_CLEAR)
			{
				done=TRUE;
				break;
			}
			//��ũ��Ʈ ����
			else if(result == SCRIPT_END)
			{
				done=TRUE;
				script_end = TRUE;
				break;
			}
		}
	}

	//��� ����� �Ϸ�Ǿ����� ť ����
	if(done)m_script.clear();
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

	//��ȭ ����� ������ ����
	int result=m_dlg.Printing();
	if(result==999 || result==-1)return RUN_END;
		else return RUN_NOTEND;
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
		select_no=result;
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
			(*m_valuemap)[HASH(it->buffer)] = atoi(input_buffer);
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
			strcpy((*m_textmap)[HASH(it->buffer)], input_buffer);
		}

		return RUN_END;
	}
}

//���ڻ�
int CScript::ComTextColor(COMMAND_PTR it)
{
	m_dlg.SetColor(it->value[0], it->value[1], it->value[2]);
	m_select.SetColor(it->value[0], it->value[1], it->value[2]);
	m_input.SetColor(it->value[0], it->value[1], it->value[2]);

	return RUN_END;
}

//�׸��� ���̱�
int CScript::ComShowShadow(COMMAND_PTR it)
{
	m_dlg.ShowShadow(true);
	m_select.ShowShadow(true);
	m_input.ShowShadow(true);

	return RUN_END;
}

//�׸��� �����
int CScript::ComHideShadow(COMMAND_PTR it)
{
	m_dlg.ShowShadow(false);
	m_select.ShowShadow(false);
	m_input.ShowShadow(false);

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
	JPictureInfo pi;
	if(jdd->GetPictureInfo("_inputbox", &pi))jdd->DeleteSurface("_inputbox");
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

	JPictureInfo pi;
	if(!jdd->GetPictureInfo(it->buffer, &pi))
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : �׸��� ã�� �� �����ϴ�.\n", debug_fp);

		return RUN_END;
	}
	ScriptPicture* p_pic = &m_pic[it->value[0]];	//������� �׸� ������
	p_pic->id = jdd->GetID(it->buffer);
	strcpy(p_pic->src, it->buffer);	//�׸� �̸� ����

	//�׸� ���� ������ �α�
	p_pic->key = pi.GetColorKey();
	p_pic->opacity = pi.GetOpacity();
	p_pic->flip = 0;	//������ ���� ���°� �⺻

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
		}
	}
	else if(strcmp(ext, ".ogg") == 0)
	{
		if(!m_ogg || !m_ogg->OpenOgg(it->buffer))
		{
			DebugMode();
			fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
			fputs(" : OGG ������ ���� �� �����ϴ�.\n", debug_fp);
		}
		else m_ogg->PlayOgg(true);
	}
	else
	{
		DebugMode();
		fprintf(debug_fp, "%s(%d) - %s", script_file, it->snr_no, snr[it->snr_no]);
		fputs(" : �������� �ʴ� ���� �����Դϴ�.\n", debug_fp);
	}
	
	return RUN_END;
}

//����� ����
int CScript::ComStopBGM(COMMAND_PTR it)
{
	_MidiStop();
	if(m_ogg)m_ogg->StopOgg();

	return RUN_END;
}

//���� ���
int CScript::ComPlaySound(COMMAND_PTR it)
{
	_Play(m_snd[it->value[0]]);

	return RUN_END;
}

//AVI ���
int CScript::ComAVI(COMMAND_PTR it)
{
	_PlayAVI(it->buffer);

	return RUN_END;
}

//���� ���� �߻�
int CScript::ComRandom(COMMAND_PTR it)
{
	(*m_valuemap)[HASH(it->buffer)] = (rand()%(abs(it->value[1] - it->value[0]))+1) + it->value[0];

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
	if(select_no < SCRIPT_VALUE_MAX && it->value[select_no] >= 0)
	{
		Goto(it->value[select_no], script_no);
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

//��� ���
int CScript::ComEnd(COMMAND_PTR it)
{
	return SCRIPT_END;
}