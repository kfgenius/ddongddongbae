//��ũ��Ʈ 2008. 05. 22

#include <stdio.h>

#include <map>
#include <vector>
#include <deque>

using namespace std;

#include "donglib.h"

#define SCRIPT_VALUE_MAX	8	//��ũ��Ʈ���� ���� ������ �� �Ѱ�
#define COMMAND_PTR			vector<CScriptCommand>::iterator

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
	DWORD flip;		//��������
	int loop;		//�ݺ� ǥ�� ����
	int gap_x;		//��°���X
	int gap_y;		//��°���Y
		
	//������ �ʿ��� ����
	char src[80];	//�׷��� �ҽ� �̸�
	int frame;		//�ִϸ��̼� �����Ӽ�
	int delay;		//�ִϸ��̼� ������
	int anitype;	//�ִϸ��̼� Ÿ��
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

//��� �Լ� ������ ����
class CScript;
typedef int (CScript::*FUNC)(vector<CScriptCommand>::iterator);

//��ũ��Ʈ �б� ���
class CScript
{
private:
	FILE *fp;
	FILE *debug_fp;

	int snrs;			//�ó����� �Ѱ�
	char** snr;			//�ó����� ������

	int script_no;			//ó�� ���� �����
	vector<int> return_no;	//���ư����� ��

	void CreateCommandMap();

	//��ũ��Ʈ ó��
	BOOL ReadScript();		//��ũ��Ʈ�� �о ť�� �ִ� �Լ�
	void BringScript();		//������ ��ũ��Ʈ�� ������
	void RunScript();		//������ ��ũ��Ʈ�� ����
	void ConvertString(char* result, char* source);	//���ڿ� �����ִ� ������ ���ڷ� ��ȯ
	void LoadFromTxt();
	void DebugMode();		//����� ������� �˻��� �� ����� ��� ����


	BOOL enable;					//����� �ε�Ǿ� ��밡������ Ȯ��
	BOOL script_end;				//��ũ��Ʈ�� �������� �˾ƺ�
	char script_file[80];			//���� �ε�� �ʿ�
	int screen_effect[2];			//ȭ��ȿ��
	int select_no;					//������ ��ȣ
	char input_buffer[80];			//�Էµ� ���ڸ� �ӽ÷� �����ϴ� ��
	bool debug_mode;				//����� ���

	map<int, int> m_BookmarkHash;		//�ϸ�ũ �ؽ�
	map<int, int> m_CommandMap;			//��ɹ� �ؽ�
	vector<CScriptCommand> m_script;	//��ũ��Ʈ ���� ����

	//����
	map<int, int> *m_valuemap;			//���� ����
	map<int, char*> *m_textmap;	//���� ����

	//��ȭâ, ����â, �Է�â
	CTextDlg m_dlg;
	CSelectDlg m_select;
	CInputDlg m_input;

	//�׸�
	CAnimation animation;
	map<int, ScriptPicture> m_pic;
	map<int, double> m_move_x;
	map<int, double> m_move_y;

	//����
	COgg* m_ogg;					//OGG �÷��̾�
	HSNDOBJ* m_snd;

	//���� ���
	void Goto(int goto_page, int return_page);
	ScriptPicture* CheckPicture(COMMAND_PTR it);
	BOOL CheckBuffer(COMMAND_PTR it);

	///////////////////////////////////////////////
	//��ɵ�
	map<int, FUNC> RunCommand;

	//���� ����
	int ComPrint(COMMAND_PTR);
	int ComSelect(COMMAND_PTR);
	int ComInputNum(COMMAND_PTR);
	int ComInputText(COMMAND_PTR);

	//���� �Ӽ� ����
	int ComTextColor(COMMAND_PTR);
	int ComShowShadow(COMMAND_PTR);
	int ComHideShadow(COMMAND_PTR);
	int ComDlgBox(COMMAND_PTR);
	int ComInputBox(COMMAND_PTR);
	int ComWindowOpacity(COMMAND_PTR);

	//�׸� ����
	int ComPicture(COMMAND_PTR);
	int ComShowPicture(COMMAND_PTR);
	int ComMovePicture(COMMAND_PTR);
	int ComHidePicture(COMMAND_PTR);
	int ComPictureOpacity(COMMAND_PTR);
	int ComVFlip(COMMAND_PTR);
	int ComHFlip(COMMAND_PTR);
	int ComScreenEffect(COMMAND_PTR);

	//���� ����
	int ComPlayBGM(COMMAND_PTR);
	int ComStopBGM(COMMAND_PTR);
	int ComPlaySound(COMMAND_PTR);
	int ComAVI(COMMAND_PTR);

	//���� ����
	int ComRandom(COMMAND_PTR);

	//�帧���� ����
	int ComWait(COMMAND_PTR);
	int ComCase(COMMAND_PTR);
	int ComGoto(COMMAND_PTR);
	int ComReturn(COMMAND_PTR);
	int ComEnd(COMMAND_PTR);

public:
	//�ε�&��ε�
	void Load(char* script_file);
	void UnLoad();

	//��ũ��Ʈ ó�� ���
	BOOL SetPage(int script_no);
	BOOL Script();

	//������&�Ҹ���
	CScript(HSNDOBJ* m_snd, map<int, int> *m_valuemap, map<int, char*> *m_textmap);
	~CScript();
};