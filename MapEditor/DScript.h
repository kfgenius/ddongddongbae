//��ũ��Ʈ Ver0.5

#include <stdio.h>

#include <map>
#include <vector>
#include <deque>

using namespace std;

#include "donglib.h"

#define SCRIPT_VALUE_MAX		8	//��ũ��Ʈ���� ���� ������ �� �Ѱ�

/////////////////////////////////////////////////////
//�׸� ����ü
struct ScriptPicture
{
	int id;			//�׸� ID
	int x, y;		//�׸� ��ǥ
	double rx, ry;	//����ǥ
	BOOL show;		//���̴��� ������� ����
	BOOL ani;		//�ִϸ��̼����� ����
	float opacity;	//����
	JColor key;		//Į��Ű
	DWORD filp;		//��������
	int loop;		//�ݺ� ǥ�� ����
	int gap_x;		//��°���X
	int gap_y;		//��°���Y
		
	//������ �ʿ��� ����
	char src[80];	//�׷��� �ҽ� �̸�
	int frame;		//�ִϸ��̼� �����Ӽ�
	int delay;		//�ִϸ��̼� ������
	int anitype;	//�ִϸ��̼� Ÿ��
};

/////////////////////////////////////////////////////
//���� ����ü
struct ScriptText
{
	int x, y;		//�׸� ��ǥ
	double rx, ry;	//����ǥ
	BOOL show;		//���̴��� ������� ����
	JColor color;	//���ڻ�
	bool shadow;	//�׸���
	
	//������ �ʿ��� ����
	char buffer[80];	//�� ����
};

/////////////////////////////////////////////////////
//���� ����ü
struct ScriptSound
{
	HSNDOBJ sound;	//���� ������Ʈ
	char src[80];	//���� �ҽ� �̸�
};

///////////////////////////////////////////
//��� Ŭ����
class CScriptCommand 
{
public:
	int	id;							//��ɹ� ��ȣ
	int value[SCRIPT_VALUE_MAX];	//����
	char* text;						//���ڿ� ������
	char buffer[1024];				//���ڿ� ����
	bool first;						//ó���� �ص־� �� ���� üũ�ϴ� �÷���

	int snr_no;						//�ڽ��� �ó����� ��ȣ(����� ��)

	CScriptCommand();
};

//�����Լ� : ������ Ŭ����
void ClearValueMap();

//��ũ��Ʈ �б� ���
class CScript
{
private:
	FILE *fp;
	FILE *debug_fp;

	int snrs;			//�ó����� �Ѱ�
	char** snr;			//�ó����� ������

	bool script_end;		//��ũ��Ʈ ������
	int script_no;			//ó�� ���� �����
	vector<int> return_no;	//���ư����� ��

	void CreateCommandMap();

	BOOL IsBookMark(int no);
	int GetCommandID(int idx);

	int get_char_num(char chr);
	char DlgDecode(char b1, char b2);

	BOOL ReadScript(vector<CScriptCommand> *m_script, int no);		//��ũ��Ʈ�� �о ť�� �ִ� �Լ�
	void BringScript();		//������ ��ũ��Ʈ�� ������
	void RunScript();		//������ ��ũ��Ʈ�� ����
	void ConvertString(char* result, char* source);	//���ڿ� �����ִ� ������ ���ڷ� ��ȯ
	void DebugMode();		//����� ������� �˻��� �� ����� ��� ����
	void SystemMenu(bool save=true);	//�ý��� �޴�
	void GetSystemValue(char* dest, char* src);		//�ý��� ���� ���

	char mlc_file[80];				//mlc ���� �̸�(Unload�� �ʿ�)
	char script_file[80];			//���� �ε�� �ʿ�
	int screen_effect[2];			//ȭ��ȿ��
	BOOL enable;					//����� �ε�Ǿ� ��밡������ Ȯ��
	int select_value;				//������ ������ �ӽ÷� �����ϴ� ��
	char input_buffer[80];			//�Էµ� ���ڸ� �ӽ÷� �����ϴ� ��
	bool debug_mode;				//����� ���
	char chapter_name[80];			//���̺�� ǥ�õ� é�� �̸�

	map<int, int> m_BookmarkHash;		//�ϸ�ũ �ؽ�
	map<int, int> m_CommandMap;			//��ɹ� �ؽ�
	vector<CScriptCommand> m_script;	//��ũ��Ʈ ���� ����

	//�����
	char bgm_name[80];
	int music_type;

	bool on_savemenu, on_loadmenu;	//�޴� ȣ�� ����

	//���ڻ�, �׸���
	int text_rgb[3];
	bool text_shadow;

	bool enable_save;	//���̺� ���� ����

	//���� ���� ����
	bool area_select;
	char area_cursor[80];
	int area_x, area_y;

	//���
	CTextDlg m_dlg;
	CSelectDlg m_select;
	CInputDlg m_input;

	//�׸�
	CAnimation animation;
	map<int, ScriptPicture> m_pic;

	//����
	map<int, ScriptSound> m_snd;
	COgg* m_ogg;					//OGG �÷��̾�
	CDirectMusic m_music;			//DirectMusic

public:
	void Script(int script_no);			//��ũ��Ʈ�� ��� ����

	//�ε�&��ε�
	void Load(char* script_file);
	void LoadFromTxt();
	void UnLoad();

	//������ �Ҹ���
	CScript(char* script_file);
	~CScript();
};