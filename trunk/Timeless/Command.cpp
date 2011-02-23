#include <windows.h>

#include "Command.h"
#include "Global.h"

//명령 처리 클래스 메소드
//(문자 배열로 char com={"이동","대기"...}등을 만들어 놓고, 가능한 명령을 넣어서 선택할 수 있게 한다.

CCommand::CCommand(char* source[], int x, int y, int width, int line, BOOL row, int max)
				//명령어 문자 배열, X,Y좌표, 사이간격, 몇개 이상 표시되면 줄을 바꿀건지, 세로 or 가로, 최대치 (버퍼사이즈 추가할 것!)
{
	comdlg.SetDlg(x, y, width, line, row);
	Init(source,max);
}

void CCommand::Init(char* source[], int max)
{
	count=0;
	this->source=source;
	command_max=max;
	strcpy(commands,"");
	comdlg.MakeSelection(commands);
}

void CCommand::AddComs(int n, ...)	//명령 여러개 추가
{
	va_list ap;
	va_start(ap,n);
	for(int i=0; i<n; ++i)AddCom(va_arg(ap,int));
	va_end(ap);
}

void CCommand::AddCom(int id)	//사용할 수 있는 명령 추가
{
	if(count>=command_max)return;	//명령어가 꽉 참
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,source[id]);
	comdlg.MakeSelection(commands);
	com_id[count]=id;
	++count;
}

//문자배열을 이용하지 않고 직접 집어 넣기
void CCommand::AddComStr(int id, char* str)
{
	if(count>=command_max)return;	//명령어가 꽉 참
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,str);
	comdlg.MakeSelection(commands);
	com_id[count]=id;
	++count;
}

int CCommand::GetComID(int no)
{
	if(no>=0 && no<command_max)return com_id[no];
		else return -1;
}

//가득찼는지 확인
BOOL CCommand::IsFull()
{
	if(count>=command_max)return TRUE;
		else return FALSE;
}

//실시간으로 선택(다른 작업과 병렬로 처리)
int CCommand::CommandSelecting()
{
	if(count<1)return -999;		//사용 가능 명령이 없으면 실행하지 않음

	int selected;
	selected=comdlg.Selecting();

	if(selected>=0)return GetComID(selected);
		else return -1;
}

//선택만 함(프로세스 독점)
int CCommand::CommandSelect()
{
	int selected;
	while(1)
	{
		if(!ProcessMessage())break;		//윈도우 메세지 처리

		selected=comdlg.Selecting();	//명령 선택
		if(selected>=0)break;
		else if(selected==-999)return -1;

		jdd->Render();
	}

	return GetComID(selected);
}

//비었으면 TRUE, 차 있으면 FALSE를 리턴
BOOL CCommand::Empty()
{
	if(count==0)return TRUE;
		else return FALSE;
}

int CCommand::GetCount()
{
	return count;
}