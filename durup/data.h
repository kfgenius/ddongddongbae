/*  DATA.H�� ���Ͽ�
-------------------------
DATA.H�� ���� ���� �� ���������� ���̴� �Լ��� �����Ͱ� ����ִ�.
�׷��ٰ� �ϳ��� ���̺귯���� ���� ��ƴ�.
��� ���ΰ� �и��Ǿ� �� ���� ����, �ٸ� ���� ���յ� �����.
�׸��� �״�� ���̴� �� �ƴ϶� ���ӿ� ���� �˸°� �ణ �����ؾ� �ȴ�.
�׷��ϱ� �׳� ���鶧���� �ݺ��Ǵ� ������ �κ��� ���̴� ��Ȱ�� �ϴ� �ҽ���.
*/


//���� ������ �ʿ��� �� ���� ����
#define N_ZERO		0
#define N_ONE		1
#define N_TWO		2
#define N_THREE		3
#define N_FOUR		4
#define N_FIVE		5

//�⺻ �׸� ���ϵ�
#define PIC_DLG			"Dlg"
#define PIC_SELECTBAR	"Select"
#define PIC_BLACK		"Black"
#define PIC_WHITE		"White"

#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))

FILE* fp;

char global_buffer[1024];

HWND hwnd;
//����
HSNDOBJ Sound[100];

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

BOOL SoundCard;
BOOL ReplayFlag;

//�̵� ����
BOOL _MidiPlay(char* pszMidiFN, BOOL bReplayFlag = TRUE)
{
    char szMCISendString[256];

    wsprintf(szMCISendString,"open %s type sequencer alias MUSIC", pszMidiFN);

    if ( mciSendString ( "close all", NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( szMCISendString, NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, hwnd ) != 0) return(FALSE);

    ReplayFlag = bReplayFlag; 
    return TRUE;
}

BOOL _MidiStop()
{
    if ( mciSendString ( "close all", NULL, 0, NULL) != 0 ) return ( FALSE );
    return TRUE;
}

BOOL _MidiReplay()
{
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, hwnd) != 0 ) return ( FALSE );
    return TRUE;
}

void _Play( int num )
{
    if ( SoundCard ) SndObjPlay( Sound[num], NULL );
}

//�ִ� �ּҰ�
int Max(int x, int y)
{
	if(x>y)return x;
		else return y;
}

int Min(int x, int y)
{
	if(x<y)return x;
		else return y;
}

//�ִ� �ּҰ�
int MaxMin(int x, int min, int max)
{
	if(x<min)return min;
		else if(x>max) return max;
		else return x;
}

bool Between(int value, int min, int max)
{
	if(value>=min && value<=max)return true;
		else return false;
}

//Ű
#define ALL_KEYS 71
int keys[ALL_KEYS]={
	VK_UP, VK_LEFT, VK_RIGHT, VK_DOWN, VK_RETURN, VK_ESCAPE, VK_SPACE, VK_CONTROL, VK_MENU,
	VK_TAB, VK_SHIFT, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6,
	VK_NUMPAD7, VK_NUMPAD8,	VK_NUMPAD9, VK_NUMPAD0,
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', VK_ADD, VK_BACK, VK_DECIMAL, VK_DELETE, VK_DIVIDE, VK_END, VK_HELP, VK_HOME,
	VK_INSERT, VK_MULTIPLY, VK_NEXT, VK_PRIOR, VK_PAUSE, VK_SUBTRACT
};

int keypush[ALL_KEYS];

//Ű ó��
bool GetKey(int key, int delay=-1)
{
	bool push;
	if(delay==-1)push=false;
		else push=true;

	if(_GetKeyState(keys[key]))
	{
		if(!keypush[key])
		{
			keypush[key]=delay;
			return true;
		}
		else if(push)
		{
			keypush[key]=Max(0,keypush[key]-1);
		}
	}
	else keypush[key]=0;
	
	return false;
}

int FindKey(int key)
{
	for(int i=0; i<ALL_KEYS; i++)
		if(keys[i]==key)return i;
	return -1;
}

//����Ʈ Ŭ����
class CList
{
protected:
	int max;

	void New(int vid);
	void Copy(int to, int from);

public:
	int count;
	int *id;

	void Init();
	void Add(int vid);
	void Delete(int vid);
	int Find(int vid);
	bool IsFull();
};

void CList::New(int vid)
{
	if(IsFull())return;

	id[count]=vid;
	count++;
}

void CList::Copy(int to, int from)
{
	id[to]=id[from];
}

void CList::Init()
{
	count=0;
}

void CList::Add(int vid)
{
	if(Find(vid)==-1)New(vid);
}

void CList::Delete(int vid)
{
	if(vid<0 || vid>=max)return;

	for(int i=vid; i<count-1; i++)Copy(i,i+1);

	count--;
}

int CList::Find(int vid)
{
	for(int i=0; i<count; i++)
		if(id[i]==vid)return i;

	return -1;
}

bool CList::IsFull()
{
	if(count==max)return true;
		else return false;
}

/*ȭ�� �׸���� ���μ���, ��Ʈ��
 ��ȭ ��� Ŭ���� ���� ������ ���μ����� �����ؾ� ó���� ���� �κ��� ���Ƽ�,
 ���� ����̳� ���� �κ��� ��忡 ���� �޶����� �ϴ� �Լ��� �����,
 Ŭ������ �ý����� �����ϸ鼭 �̸� ȣ���ϴ� ������� ����
 ��, ȭ�� ����� ��� ���� Screen �ϳ��� ����ϰ�, ó���� Process �ϳ��� �����.
 �ؽ�Ʈ ��� ���� ���, �ؽ�Ʈ ����� ������ ���� �� ���� �ȿ��� �� �Լ��� ���������,
 �� �������� ������ ��ü���� ��°� ��� �ǰ� ��.
*/
int game_mode;
static void Screen();
static void Process();
static void Control();

//���ڿ� ó�� �Լ�
char* StrAdd(char* msg, ...)
{
	strcpy(global_buffer,"");
	va_list ap;
	va_start(ap,msg);
	int max=strlen(msg);
	for(int i=0; i<max; i++)
	{
		if(msg[i]=='c')
		{
			strcat(global_buffer,va_arg(ap,char*));
		}
		else if(msg[i]=='d')
		{
			char int_buffer[10];
			itoa(va_arg(ap,int),int_buffer,10);
			strcat(global_buffer,int_buffer);
		}
	}

	return global_buffer;
}

//���� ������ �����
void MakeNumSelect(int from, int to, int step=1)
{
	char num_buffer[12];

	strcpy(global_buffer,"");
	for(int i=from; i<=to; i+=step)
	{
		if(i>from)strcat(global_buffer,"\\");
		strcat(global_buffer,itoa(i,num_buffer,10));
	}

}

//��ȣȭ�� ���� ���ڿ�
char CodeTo[76]="dyKp:8jP;[R^FIqN0WsTX4U`a52Z]1HnbuVvlh76\\ti=SArJ@>Q_wgkoB?LCcYD3<M9EmGxeOfz";

int get_char_num(char chr)
{
	int i;
	for(i=0; i<76; i++)
		if(CodeTo[i]==chr)break;

	return i;
}

//��� ����
void Pause()
{
	while(!GetKey(5) && !GetKey(4))
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//Process();
		Screen();

		jdd->Render();
	}
}

//ȭ�� �����
void Clear()
{
	for(int j=0; j<24; j++)
		jdd->DrawPicture(backbuffer,PIC_BLACK,0,j*20,NULL);
}

//���̵� �ƿ�
void FadeOut()
{
	JPictureInfo pi;
	pi.SetOpacity(0.2f);
	jdd->SetPictureInfo(PIC_BLACK,&pi);
	for(int i=0; i<10; i++)
	{
		for(int j=0; j<24; j++)
			jdd->DrawPicture(backbuffer,PIC_BLACK,0,j*20,NULL);
		jdd->Render();
		Sleep(30);
	}
	pi.SetOpacity(1.0);
	jdd->SetPictureInfo(PIC_BLACK,&pi);

	//�����ϰ� ����
	for(int j=0; j<24; j++)
		jdd->DrawPicture(backbuffer,PIC_BLACK,0,j*20,NULL);
	jdd->Render();
}

//ȭ��Ʈ �ƿ�
void WhiteOut()
{
	JPictureInfo pi;
	pi.SetOpacity(0.2f);
	jdd->SetPictureInfo(PIC_WHITE,&pi);
	for(int i=0; i<10; i++)
	{
		for(int j=0; j<24; j++)
			jdd->DrawPicture(backbuffer,PIC_WHITE,0,j*20,NULL);
		jdd->Render();
		Sleep(30);
	}
	pi.SetOpacity(1.0);
	jdd->SetPictureInfo(PIC_WHITE,&pi);

	//�����ϰ� ����
	for(int j=0; j<24; j++)
		jdd->DrawPicture(backbuffer,PIC_WHITE,0,j*20,NULL);
	jdd->Render();
}

/////////////////////////////////////////////////////
//�ѱ� �Է� �ý���
char* han1[]={"��","rk","��","rkr","�A","rkR","��","rks","��","rke","��","rkf","��","rkfr","��","rkfa","�E","rkfq","�F","rkft",
			"�G","rkfx","�H","rkfv","�I","rkfg","��","rka","��","rkq","��","rkqt","��","rkt","��","rkT","��","rkd","��","rkw",
			"��","rkc","��","rkx","��","rkv","��","rkg","��","ro","��","ror","��","ros","��","roe","��","roa","��","roq",
			"��","rot","��","roT","��","rod","��","ri","��","rir","��","ris","��","rif","��","rit","��","rid","��","rO",
			"��","rOs","��","rOf","��","rj","��","rjr","��","rjs","��","rje","��","rjf","��","rjfa","��","rja","��","rjq",
			"��","rjt","��","rjT","��","rjd","��","rjw","��","rjx","��","rjv","��","rjg","��","rp","��","rps","��","rpf",
			"��","rpa","��","rpq","��","rpt","��","rpT","��","rpd","��","ru","��","rur","��","rur","��","rus","��","rue",
			"��","ruf","��","rua","��","ruq","��","rut","��","ruT","��","rud","��","rux","��","rP","��","rPs","��","rPf",
			"��","rPq","��","rPt","��","rh","��","rhr","��","rhs","��","rhe","��","rhf","��","rhfa","��","rhft","��","rhfg",
			"��","rha","��","rhq","��","rht","��","rhd","��","rhw","��","rhk","��","rhkr","��","rhks","��","rhkf","��","rhkfa",
			"��","rhka","��","rhkq","��","rhkt","��","rhkd","��","rho","��","rhos","��","rhof","��","rhoq","��","rhoT","��","rhod",
			"��","rhl","��","rhlr","��","rhls","��","rhlf","��","rhla","��","rhlq","��","rhlt","��","rhld","��","ry","��","rys",
			"��","ryf","��","ryq","��","ryt","��","rn","��","rnr","��","rns","��","rne","��","rnf","��","rnfr","��","rnfa",
			"��","rnfg","��","rna","��","rnq","��","rnt","��","rnd","��","rnw","��","rnj","��","rnjr","��","rnjs","��","rnjf",
			"��","rnjT","��","rnjd","��","rnp","��","rnpt","��","rnl","��","rnlr","��","rnls","��","rnlf","��","rnla","��","rnlq",
			"��","rnlt","��","rb","��","rbs","��","rbf","��","rm","��","rmr","��","rms","��","rme","��","rmf","��","rmfr",
			"��","rma","��","rmq","��","rmt","��","rmd","��","rml","��","rl","��","rlr","��","rls","��","rle","��","rlf",
			"��","rlfa","��","rla","��","rlq","��","rlt","��","rld","��","rlw","��","rlv","��","Rk","��","Rkr","��","RkR",
			"��","Rks","��","Rkf","��","Rkfa","��","Rka","��","Rkq","��","Rkt","��","RkT","��","Rkd","��","Rkx","��","Ro",
			"��","Ror","��","Ros","��","Rof","��","Roa","��","Roq","��","Rot","��","RoT","��","Rod","��","Ri","��","Rir",
			"��","Rif","��","Rj","��","Rjr","��","RjR","��","Rjs","��","Rjf","��","Rja","��","Rjq","��","Rjt","��","RjT",
			"��","Rjd","��","Rp","��","Rpr","��","RpR","��","Rps","��","Rpa","��","Rpt","��","Rpd","��","Ru","��","Rus",
			"��","Ruf","��","Rut","��","RuT","��","Rux","��","RP","��","Rh","��","Rhr","��","Rhs","��","Rha","��","Rhsg",
			"��","Rhf","��","Rha","��","Rhq","��","Rht","��","Rhd","��","Rhw","��","Rhc","��","Rhk","��","Rhkr","��","Rhkf",
			"��","RhkT","��","Rhkd","��","Rho","��","Rhor","��","Rhod","��","Rhl","��","Rhls","��","RHlf","��","Rhla","��","Rhlq",
			"��","Rhld","��","Ry","��","Rn","��","Rnr","��","Rns","��","Rnf","��","Rnfg","��","Rna","��","Rnq","��","Rnt",
			"��","Rnd","��","Rnw","��","Rnj","��","Rnjf","��","RnjT","��","Rnjd","��","Rnp","��","Rnpr","��","Rnps","��","Rnpf",
			"��","Rnpa","��","Rnpq","��","RnpT","��","Rnl","��","Rnls","��","Rnlf","��","Rnla","��","Rnlq","��","Rb","��","Rm",
			"��","Rmr","��","Rms","��","Rmsg","��","Rmf","��","Rmfa","��","Rmfg","��","Rma","��","Rmq","��","Rmt","��","Rmd",
			"��","Rmx","��","Rl","��","Rlr","��","Rls","��","Rlf","��","Rla","��","Rlq","��","Rlt","��","Rld","��","sk",
			"��","skr","��","skR","��","sks","��","ske","��","skf","��","skfr","��","skfa","��","ska","��","skq","��","skt",
			"��","skT","��","skd","��","skw","��","skc","��","skx","��","skg","��","so","��","sor","��","sos","��","sof",
			"��","soa","��","soq","��","sot","��","soT","��","sod","��","si","��","sir","��","sis","��","sif","��","sia",
			"��","sid","��","sj","��","sjr","��","sjt","��","sjs","��","sjf","��","sjfa","��","sjfq","��","sja","��","sjq",
			"��","sjt","��","sjT","��","sjd","��","sjg","��","sp","��","spr","��","sps","��","spf","��","spa","��","spq",
			"��","spt","��","spT","��","spd","��","su","��","sur","��","sus","��","suf","��","sua","��","suq","��","suT",
			"��","sud","��","suz","��","sP","��","sPs","��","sh","��","shr","��","shs","��","shf","��","shfa","��","sha",
			"��","shq","��","sht","��","shd","��","shv","��","shg","��","shk","��","shks","��","shkf","��","shkT","��","shl",
			"��","shls","��","shlf","��","shla","��","shlq","��","shlt","��","sy","��","syr","��","sys","��","syf","��","syq",
			"��","syt","��","syd","��","sn","��","snr","��","sns","��","sne","��","snf","��","sna","��","snq","��","snt",
			"��","snd","��","snj","��","snjT","��","snp","��","snl","��","snls","��","snlf","��","snla","��","snlq","��","sb",
			"��","sbr","��","sbf","��","sba","��","sbq","��","sbd","��","sm","��","smr","��","sms","��","smf","��","smfr",
			"��","smfa","��","sma","��","smq","��","smt","��","smd","��","smw","��","smv","��","sml","��","smls","��","smlf",
			"��","sl","��","slr","��","sls","��","slf","��","slfa","��","sla","��","slq","��","slt","��","sld","��","slv",
			"��","ek","��","ekr","��","ekR","��","eks","��","eke","��","ekf","��","ekfr","��","ekfa","��","ekfq","��","ekfg",
			"��","eka","��","ekq","��","ekt","��","ekT","��","ekd","��","ekw","��","ekc","��","ekg","��","eo","��","eor",
			"��","eos","��","eof","��","eoa","��","eoq","��","eot","��","eoT","��","eod","��","ei","��","ej","��","ejr",
			"��","ejR","��","ejs","��","eje","��","ejf","��","ejfa","��","ejfq","��","eja","��","ejq","��","ejt","��","ejd",
			"��","ejc","��","ejv","��","ep","��","epr","��","eps","��","epf","��","epa","��","epq","��","ept","��","epT",
			"��","epd","��","eu","��","eus","��","euf","��","euT","��","eud","��","eP","��","ePs","��","eh","��","ehr",
			"��","ehs","��","ehe","��","ehf","��","ehfa","��","ehft","��","eha","��","ehq","��","eht","��","ehd","��","ehc",
			"��","ehx","��","ehk","��","ehks","��","ehkf","��","eho","��","ehoT","��","ehl","��","ehls","��","ehlf","��","ehla",
			"��","ehlq","��","ehlt","��","ey","��","en","��","enr","��","ens","��","enf","��","ena","��","enq","��","ent",
			"��","end","��","enj","��","enjT","��","enp","��","enpd","��","enl","��","enls","��","enlf","��","enlq","��","enlt",
			"��","enld","��","eb","��","ens","��","enf","��","ena","��","end","��","em","��","emr","��","ems","��","eme",
			"��","emf","��","emfa","��","ema","��","emq","��","emt","��","emd","��","eml","��","el","��","elr","��","els",
			"��","ele","��","elf","��","ela","��","elq","��","elt","��","elT","��","eld","��","elw","��","Ek","��","Ekr",
			"��","Eks","��","Ekf","��","Eka","��","Ekq","��","Ekt","��","EkT","��","Ekd","��","Ekg","��","Eo","��","Eor",
			"��","Eos","��","Eof","��","Eoa","��","Eoq","��","Eot","��","EoT","��","Eod","��","Ej","��","Ejr","��","Ejs",
			"��","Ejf","��","Ejfa","��","Ejfq","��","Eja","��","Ejq","��","Ejt","��","EjT","��","Ejd","��","Ejg","��","Ep",
			"��","Epr","��","Eps","��","Epf","��","Epa","��","Epq","��","Ept","��","EpT","��","Epd","��","Eu","��","EuT",
			"��","Eh","��","Ehr","��","Ehs","��","Ehf","��","Ehd","��","Ehk","��","Ehkf","��","Eho","��","Ehl","��","Ehls",
			"��","En","��","Enr","��","Ens","��","Enf","��","Enfg","��","Ena","��","End","��","Enp","��","Enl","��","Enls",
			"��","Enlf","��","Enla","��","Enlq","��","Enld","��","Em","��","Emr","��","Ems","��","Eme","��","Emf","��","Ema",
			"��","Emq","��","Emt","��","Eml","��","Emls","��","Emlf","��","Emla","��","Emlq","��","El","��","Els","��","Elf",
			"��","Ela","��","Elq","��","Elt","��","Eld","��","fk","��","fkr","��","fks","��","fkf","��","fka","��","fkq",
			"��","fkt","��","fkT","��","fkd","��","fkw","��","fkv","��","fkg","��","fo","��","for","��","fos","��","fof",
			"��","foa","��","foq","��","fot","��","foT","��","fod","��","fi","��","fir","��","fis","��","fit","��","fid",
			"��","fj","��","fjr","��","fjs","��","fjf","��","fja","��","fjq","��","fjt","��","fjT","��","fjd","��","fjg",
			"��","fp","��","fpr","��","fps","��","fpf","��","fpa","��","fpq","��","fpt","��","fpd","��","fu","��","fur",
			"��","fus","��","fuf","��","fua","��","fuq","��","fut","��","fuT","��","fud","��","fP","��","fPs","��","fPq",
			"��","fPt","��","fh","��","fhr","��","fhs","��","fhf","��","fha","��","fhq","��","fht","��","fhd","��","fhk",
			"��","fhks","��","fhkd","��","fhoT","��","fhl","��","fhls","��","fhlf","��","fhla","��","fhlq","��","fhlt","��","fhld",
			"��","fy","��","fys","��","fyf","��","fyq","��","fyt","��","fyd","��","fn","��","fnr","��","fns","��","fnf",
			"��","fna","��","fnq","��","fnt","��","fnd","��","fnj","��","fnjT","��","fnp","��","fnl","��","fnlr","��","fnls",
			"��","fnlf","��","fnla","��","fnlt","��","fnld","��","fb","��","fbr","��","fbs","��","fbf","��","fba","��","fbq",
			"��","fbt","��","fbd","��","fm","��","fmr","��","fms","��","fmf","��","fma","��","fmq","��","fmt","��","fmd",
			"��","fmw","��","fmx","��","fmv","��","fl","��","flr","��","fls","��","flf","��","fla","��","flq","��","flt",
			"��","fld","��","ak","��","akr","��","aks","��","aksg","��","ake","��","akf","��","akfr","��","aka","��","aka",
			"��","akq","��","akt","��","akd","��","akw","��","akx","��","akg","��","ao","��","aor","��","aos","��","aof","��","aoa",
			"��","aoq","��","aot","��","aoT","��","aod","��","aow","��","ai","��","air","��","aif","��","aid","��","aj","��","ajr",
			"��","ajs","��","ajf","��","ajfa","��","aja","��","ajq","��","ajt","��","ajd","��","ajw","��","ajg","��","ap","��","apr",
			"��","aps","��","apf","��","apa","��","apq","��","apt","��","apT","��","apd","��","au","��","aur","��","aus",
			"��","auf","��","aut","��","auT","��","aud","��","auc","��","aP","��","ah","��","ahr","��","ahrt","��","ahs","��","ahf",
			"��","ahfa","��","aha","��","ahq","��","aht","��","ahd","��","ahk","��","ahks","��","ahkT","��","ahkd","��","ahl","��","ahls",
			"��","ahlf","��","ahlq","��","ahlt","��","ahld","��","ay","��","ays","��","ayf","��","ayq","��","ayt","��","an","��","anr",
			"��","anR","��","ans","��","ane","��","anf","��","anfr","��","anfa","��","ana","��","anq","��","ant","��","and",
			"��","anx","��","ang","��","anj","��","anjs","��","anjf","��","anjq","��","anjt","��","anp","��","anl","��","anls","��","anlf",
			"��","ab","��","abs","��","abf","��","aba","��","abt","��","am","��","ams","��","amf","��","ama","��","amt","��","al",
			"��","alr","��","als","��","ale","��","alf","��","alfa","��","ala","��","alq","��","alt","��","alT","��","ald","��","alC",
			"��","alx","��","qk","��","qkr","��","qkR","��","qkrt","��","qks","��","qke","��","qkf","��","qkfr","��","qkfa",
			"��","qkfq","��","qka","��","qkq","��","qkt","��","qkd","��","qkx","��","qo","��","qor","��","qos","��","qof","��","qoa",
			"��","qoq","��","qot","��","qoT","��","qod","��","qox","��","qi","��","qir","��","qis","��","qiq","��","qj","��","qjr",
			"��","qjs","��","qje","��","qjf","��","qjfa","��","qja","��","qjq","��","qjt","��","qjd","��","qjw","��","qp","��","qpr",
			"��","qps","��","qpe","��","qpf","��","qpa","��","qpq","��","qpt","��","qpT","��","qpd","��","qu","��","qur",
			"��","qus","��","quf","��","quq","��","qut","��","quT","��","qud","��","qux","��","qP","��","qPs","��","qh","��","qhr",
			"��","qhR","��","qhs","��","qhf","��","qha","��","qhq","��","qht","��","qhd","��","qhk","��","qhks","��","qhkT","��","qho",
			"��","qhoT","��","qhl","��","qhlr","��","qhls","��","qhlf","��","qhla","��","qhlq","��","qy","��","qys","��","qn","��","qnr",
			"��","qns","��","qne","��","qnf","��","qnfr","��","qnfa","��","qna","��","qnq","��","qnt","��","qnd","��","qnx",
			"��","qnv","��","qnj","��","qnjf","��","qnjT","��","qnp","��","qnpf","��","qnpfr","��","qnl","��","qnlr","��","qnls","��","qnlf","��","qnld","��","qb",
			"��","qbs","��","qbf","��","qba","��","qbt","��","qbd","��","qm","��","qmr","��","qms","��","qmf","��","qma","��","qmq",
			"��","qmt","��","ql","��","qlr","��","qls","��","qlf","��","qlfa","��","qla","��","qlq","��","qlt","��","qld","��","qlw",
			"��","qlc","��","Qk","��","Qkr","��","Qks","��","Qkf","��","Qkfa","��","Qka","��","Qkq","��","Qkt","��","QkT",
			"��","Qkd","��","Qkg","��","Qo","��","Qor","��","Qos","��","Qof","��","Qoa","��","Qoq","��","Qot","��","QoT","��","Qod",
			"��","Qi","��","Qir","��","Qia","��","Qj","��","Qjr","��","Qjs","��","Qje","��","Qjf","��","Qja","��","Qjt","��","QjT",
			"��","Qjd","��","Qp","��","Qpd","��","Qu","��","Qur","��","Qua","��","Quq","��","Qut","��","QuT","��","Qud","��","Qh",
			"��","Qhr","��","Qhs","��","Qhf","��","Qha","��","Qhq","��","Qhd","��","Qhl","��","Qy","��","Qyd","��","Qn",
			"��","Qnr","��","Qns","��","Qnf","��","Qna","��","Qnt","��","Qnd","��","Qb","��","Qbd","��","Qm","��","Qms","��","Qmf",
			"��","Qma","��","Qmq","��","Ql","��","Qlr","��","Qls","��","Qlf","��","Qla","��","Qlq","��","Qlt","��","Qld","��","tk",
			"��","tkr","��","tkrt","��","tks","��","tke","��","tkf","��","tkfr","��","tkfa","��","tka","��","tkq","��","tkt","��","tkT",
			"��","tkd","��","tkx","��","to","��","tor","��","tos","��","tof","��","toa","��","toq","��","tot","��","toT",
			"��","tod","��","ti","��","tir","��","tis","��","tif","��","tia","��","tiq","��","tit","��","tid","��","tO",
			"��","tOs","��","tOf","��","tOa","��","tOd","��","tj","��","tjr","��","tjR","��","tjrt","��","tjs","��","tje",
			"��","tjf","��","tjfa","��","tjfq","��","tja","��","tjq","��","tjt","��","tjT","��","tjd","��","tjv","��","tp",
			"��","tpR","��","tps","��","tpf","��","tpa","��","tpq","��","tpt","��","tpT","��","tpd","��","tu","��","tur",
			"��","tus","��","tuf","��","tua","��","tuq","��","tut","��","tuT","��","tud","��","tP","��","tPs","��","tPf",
			"��","tPd","��","th","��","thr","��","thR","��","ths","��","thf","��","thfa","��","tha","��","thq","��","tht",
			"��","thd","��","thx","��","thk","��","thkr","��","thks","��","thkf","��","thkd","��","tho","��","thos","��","thof",
			"��","thoa","��","thot","��","thoT","��","thl","��","thls","��","thlf","��","thla","��","thlq","��","thlt","��","ty",
			"��","tyr","��","tys","��","tyf","��","tya","��","tyq","��","tyt","��","tyd","��","tn","��","tnr","��","tns",
			"��","tne","��","tnf","��","tna","��","tnq","��","tnt","��","tnd","��","tnc","��","tnx","��","tnv","��","tnj",
			"��","tnjT","��","tnp","��","tnpr","��","tnps","��","tnpf","��","tnpa","��","tnpd","��","tnl","��","tnlr","��","tnls",
			"��","tnlf","��","tnla","��","tnlq","��","tnlt","��","tnld","��","tb","��","tbr","��","tbf","��","tba","��","tbt",
			"��","tbd","��","tm","��","tmr","��","tms","��","tmf","��","tmfr","��","tma","��","tmq","��","tmt","��","tmd",
			"��","tl","��","tlr","��","tls","��","tle","��","tlf","��","tlfg","��","tla","��","tlq","��","tlt","��","tld",
			"��","tlv","��","Tk","��","Tkr","��","Tkrt","��","Tks","��","Tkf","��","Tka","��","Tkq","��","TkT","��","Tkd",
			"��","Tkg","��","To","��","Tor","��","Tos","��","Tof","��","Toa","��","Toq","��","ToT","��","Tod","��","Tid",
			"��","Tj","��","Tjr","��","Tjs","��","Tjf","��","Tjfa","��","Tja","��","Tjq","��","TjT","��","Tjd","��","Tp",
			"��","Tpr","��","Tps","��","Tpf","��","TPs","��","Th","��","Thr","��","Ths","��","The","��","Thf","��","Thfa",
			"��","Tha","��","Thq","��","Thd","��","Thk","��","Thkr","��","Thks","��","ThkT","��","Tho","��","ThoT","��","Thl",
			"��","Thls","��","Thlf","��","Thla","��","Thlq","��","Ty","��","Tn","��","Tnr","��","Tns","��","Tnf","��","Tna",
			"��","Tnq","��","Tnd","��","Tnj","��","TnT","��","Tnp","��","Tnl","��","Tnls","��","Tbd","��","Tm","��","Tmr",
			"��","Tms","��","Tmf","��","Tmfa","��","Tmfg","��","Tma","��","Tmq","��","Tml","��","Tmls","��","Tmlf","��","Tmla",
			"��","Tl","��","Tlr","��","Tls","��","Tlf","��","Tla","��","Tlq","��","Tlt","��","Tld","��","dk","��","dkr",
			"��","dks","��","dksw","��","dksg","��","dkf","��","dkfr","��","dkfa","��","dkfg","��","dka","��","dkq","��","dkt",
			"��","dkT","��","dkd","��","dkx","��","dkv","��","do","��","dor","��","dos","��","dof","��","doa","��","doq",
			"��","dot","��","doT","��","dod","��","di","��","dir","��","dis","��","dif","��","difq","��","dia","��","diq",
			"��","dit","��","did","��","dix","��","dig","��","dO","��","dOs","��","dOf","��","dOq","��","dj","��","djr",
			"��","djs","��","djsw","��","dje","��","djf","��","djfr","��","djfa","��","dja","��","djq","��","djqt","��","djt",
			"��","djT","��","djd","��","djw","��","djz","��","djv","��","dp","��","dpr","��","dps","��","dpf","��","dpa",
			"��","dpq","��","dpt","��","dpd","��","du","��","dur","��","duR","��","dus","��","duf","��","dufa","��","dufq",
			"��","dua","��","duq","��","duqt","��","dut","��","duT","��","dud","��","dux","��","duv","��","dug","��","dP",
			"��","dPs","��","dPf","��","dPa","��","dPq","��","dPt","��","dPT","��","dh","��","dhr","��","dhs","��","dhf",
			"��","dhfr","��","dhfa","��","dhft","��","dhfg","��","dha","��","dhq","��","dht","��","dhd","��","dhc","��","dhk",
			"��","dhkr","��","dhks","��","dhkf","��","dhka","��","dhkq","��","dhkt","��","dhkT","��","dhkd","��","dho","��","dhor",
			"��","dhos","��","dhoa","��","dhot","��","dhod","��","dhl","��","dhlr","��","dhls","��","dhlf","��","dhla","��","dhlq",
			"��","dhlt","��","dhld","��","dy","��","dyr","��","dys","��","dyf","��","dya","��","dyq","��","dyr","��","dyd",
			"��","dn","��","dnr","��","dns","��","dnf","��","dnfr","��","dnfa","��","dna","��","dnq","��","dnt","��","dnd",
			"��","dnj","��","dnjr","��","dnjs","��","dnjf","��","dnja","��","dnjq","��","dnjT","��","dnjd","��","dnp","��","dnpr",
			"��","dnps","��","dnpf","��","dnpa","��","dnpq","��","dnpd","��","dnl","��","dnlr","��","dnls","��","dnlf","��","dnla",
			"��","dnlq","��","dnlt","��","dnld","��","db","��","dbr","��","dbs","��","dbf","��","dba","��","dbq","��","dbt",
			"��","dbd","��","dbc","��","dm","��","dmr","��","dms","��","dmf","��","dmfv","��","dma","��","dnq","��","dmt",
			"��","dmd","��","dmw","��","dmc","��","dmz","��","dmx","��","dmv","��","dmg","��","dml","��","dmls","��","dmlf",
			"��","dmla","��","dmlt","��","dl","��","dlr","��","dls","��","dlf","��","dlfr","��","dlfa","��","dlfg","��","dla",
			"��","dlq","��","dlt","��","dlT","��","dld","��","dlw","��","dlv","��","wk","��","wkr","��","wks","��","wksg",
			"��","wke","��","wkf","��","wkfa","��","wka","��","wkq","��","wkt","��","wkT","��","wkd","��","wkw","��","wo",
			"��","wor","��","wos","��","wof","��","woa","��","woq","��","wot","��","woT","��","wod","��","wi","��","wir",
			"��","wis","��","wisg","��","wif","��","wia","��","wid","��","wO","��","wOs","��","wOf","��","wj","��","wjr",
			"��","wjs","��","wjf","��","wjfa","��","wja","��","wjq","��","wjt","��","wjd","��","wjw","��","wp","��","wpr",
			"��","wps","��","wpf","��","wpa","��","wpq","��","wpt","��","wpd","��","wu","��","wus","��","wuf","��","wua",
			"��","wuq","��","wuT","��","wud","��","wP","��","wh","��","whr","��","whs","��","whf","��","whfa","��","wha",
			"��","whq","��","wht","��","whd","��","whw","��","whc","��","whg","��","whk","��","whkr","��","whkf","��","whkq",
			"��","whkt","��","whkd","��","who","��","whoT","��","whod","��","whl","��","whls","��","whlf","��","whla","��","whlq",
			"��","whlt","��","whld","��","wy","��","wyr","��","wys","��","wyd","��","wn","��","wnr","��","wns","��","wnf",
			"��","wnfr","��","wnfa","��","wna","��","wnq","��","wnt","��","wnd","��","wnj","��","wnjT","��","wnp","��","wnl",
			"��","wnlr","��","wnls","��","wnlf","��","wnla","��","wnlq","��","wnlt","��","wb","��","wbs","��","wbf","��","wba",
			"��","wm","��","wmr","��","wms","��","wmf","��","wma","��","wmq","��","wmt","��","wmd","��","wl","��","wlr",
			"��","wls","��","wle","��","wlf","��","wlfa","��","wla","��","wlq","��","wlt","¡","wld","¢","wlw","£","wlx",
			"¤","wlv","¥","Wk","¦","Wkr","§","Wks","¨","Wksg","©","Wkf","ª","Wkfq","«","Wka","¬","Wkq","­","Wkt",
			"®","WkT","¯","Wkd","°","Wo","±","Wor","²","Wos","³","Wof","´","Woa","µ","Woq","¶","Wot","·","WoT",
			"¸","Wod","¹","Wi","º","Wis","»","Wid","¼","Wj","½","Wjr","¾","Wjs","¿","Wjf","��","Wja","��","Wjq",
			"��","Wjt","��","WjT","��","Wjd","��","Wp","��","Wpd","��","Wu","��","WuT","��","Wh","��","Whr","��","Whs",
			"��","Whf","��","Wha","��","Whq","��","Wht","��","Whd","��","Whc","��","Whk","��","Whkr","��","Whkf","��","WhkT",
			"��","Who","��","WhoT","��","Whl","��","Whls","��","Whlf","��","Whla","��","Whlq","��","Wyd","��","Wn","��","Wnr",
			"��","Wns","��","Wnf","��","Wna","��","Wnq","��","Wnd","��","Wnj","��","WnjT","��","Wnjd","��","Wnl","��","Wb",
			"��","Wm","��","Wma","��","Wmt","��","Wmd","��","Wl","��","Wlr","��","Wls","��","Wlf","��","Wla","��","Wlq",
			"��","Wld","��","Wlw","��","Wlg","��","ck","��","ckr","��","cks","��","cksg","��","ckf","��","cka","��","ckq",
			"��","ckt","á","ckT","â","ckd","ã","ckw","ä","co","å","cor","æ","cos","ç","cof","è","coa","é","coq",
			"ê","cot","ë","coT","ì","cod","í","ci","î","cis","ï","cisg","ð","cif","ñ","cia","ò","cid","ó","cj",
			"ô","cjr","õ","cjs","ö","cjf","÷","cja","ø","cjq","ù","cjt","ú","cjT","û","cjd","ü","cp","ý","cpr",
			"þ","cps","ÿ","cpf","��","cpa","��","cpq","��","cpt","��","cpd","��","cu","��","cus","��","cuT","��","cP",
			"��","cPs","��","cPd","��","ch","��","chr","��","chs","��","chf","��","cha","��","chq","��","cht","��","cha",
			"��","chk","��","chks","��","chkf","��","chkd","��","chl","��","chls","��","chlf","��","chla","��","chlq","��","chlt",
			"��","chld","��","cy","��","cya","�h","cyq","��","cn","��","cnr","��","cns","��","cnf","��","cna","��","cnq",
			"��","cnt","��","cnd","��","cnj","��","cnjT","��","cnp","��","cnps","�X","cnpt","��","cnl","��","cnls","��","cnlf",
			"��","cnla","��","cnlq","��","cnlt","��","cnld","��","cb","��","cbs","��","cbf","��","cba","��","cbd","��","cm",
			"��","cmr","��","cms","��","cmf","��","cma","��","cmq","��","cmt","��","cmd","ġ","cl","Ģ","clr","ģ","cls",
			"Ĥ","cle","ĥ","clf","Ħ","clfr","ħ","cla","Ĩ","clq","ĩ","clt","Ī","cld","ī","zk","Ĭ","zkr","ĭ","zks",
			"Į","zkf","į","zka","İ","zkq","ı","zkt","Ĳ","zkd","ĳ","zo","Ĵ","zor","ĵ","zos","Ķ","zof","ķ","zoa",
			"ĸ","zoq","Ĺ","zot","ĺ","zoT","Ļ","zod","ļ","zi","Ľ","zir","ľ","zid","Ŀ","zj","��","zjr","��","zjs",
			"��","zje","��","zjf","��","zja","��","zjq","��","zjt","��","zjT","��","zjd","��","zp","��","zp","��","zps",
			"��","zpf","��","zpa","��","zpq","��","zpt","��","zpd","��","zu","��","zus","��","zuf","��","zua","��","zuq",
			"��","zut","��","zuT","��","zud","��","zP","��","zh","��","zhr","��","zhs","��","zhf","��","zha","��","zhq",
			"��","zht","��","zhd","��","zhk","��","zhkr","��","zhks","��","zhkf","��","zhka","��","zhkd","��","zho","��","zhod",
			"��","zhl","��","zhlf","��","zy","��","zn","��","znr","��","zns","��","znf","��","zna","��","znq","��","znt",
			"��","znd","��","znj","��","znjs","��","znjf","��","znjd","��","znp","��","znpd","��","znl","��","znlr","��","znls",
			"��","znlf","š","znla","Ţ","znlq","ţ","znlt","Ť","znld","ť","zb","Ŧ","zbs","ŧ","zbf","Ũ","zba","ũ","zm",
			"Ū","zmr","ū","zms","Ŭ","zmf","ŭ","zma","Ů","zmq","ů","zmd","Ű","zl","ű","zlr","Ų","zls","ų","zlf",
			"Ŵ","zla","ŵ","zlq","Ŷ","zlt","ŷ","zld","Ÿ","xk","Ź","xkr","ź","xks","Ż","xkf","ż","xkfr","Ž","xka",
			"ž","xkq","ſ","xkt","��","xkT","��","xkd","��","xo","��","xor","��","xos","��","xof","��","xoa","��","xoq",
			"��","xot","��","xoT","��","xod","��","xi","��","xid","��","xj","��","xjr","��","xjs","��","xjf","��","xjfa",
			"��","xja","��","xjq","��","xjt","��","xjT","��","xjd","��","xp","��","xpr","��","xps","��","xpf","��","xpa",
			"��","xpq","��","xpt","��","xpd","��","xu","��","xus","��","xuT","��","xP","��","xPs","��","xh","��","xhr",
			"��","xhs","��","xhf","��","xha","��","xhq","��","xht","��","xhd","��","xhv","��","xhk","��","xhks","��","xho",
			"��","xhl","��","xhls","��","xhlt","��","xhld","��","xy","��","xn","��","xnr","��","xns","��","xnf","��","xna",
			"��","xnq","��","xnt","��","xnd","��","xnj","��","xnjT","ơ","xnp","Ƣ","xnl","ƣ","xnlr","Ƥ","xnls","ƥ","xnlf",
			"Ʀ","xnla","Ƨ","xnlq","ƨ","xnld","Ʃ","xb","ƪ","xbs","ƫ","xb","Ƭ","xba","ƭ","xbd","Ʈ","xm","Ư","xmr",
			"ư","xms","Ʊ","xme","Ʋ","xmf","Ƴ","xmfa","ƴ","xma","Ƶ","xmq","ƶ","xmt","Ʒ","xml","Ƹ","xls","ƹ","xmlf",
			"ƺ","xmla","ƻ","xmlq","Ƽ","xl","ƽ","xlr","ƾ","xls","ƿ","xlf","��","xla","��","xlq","��","xlt","��","xld",
			"��","vk","��","vkr","��","vkR","��","vks","��","vkf","��","vkfa","��","vka","��","vkq","��","vkt","��","vkT",
			"��","vkd","��","vkx","��","vko","��","vor","��","vos","��","vof","��","voa","��","voq","��","vot","��","voT",
			"��","vod","��","vi","��","vir","��","vj","��","vjr","��","vjs","��","vjf","��","vja","��","vjq","��","vjt",
			"��","vjT","��","vjd","��","vp","��","vpr","��","vps","��","vpf","��","vpa","��","vpq","��","vpt","��","vpd",
			"��","vu","��","vus","��","vuf","��","vua","��","vuq","��","vuT","��","vud","��","vP","��","vPf","��","vPq",
			"��","vPt","��","vh","��","vhr","��","vhs","��","vhf","��","vha","��","vhq","��","vht","��","vhd","ǡ","vhk",
			"Ǣ","vhkd","ǣ","vhl","Ǥ","vhls","ǥ","vy","Ǧ","vys","ǧ","vyf","Ǩ","vyq","ǩ","vyt","Ǫ","vn","ǫ","vnr",
			"Ǭ","vns","ǭ","vne","Ǯ","vnf","ǯ","vnfa","ǰ","vna","Ǳ","vnq","ǲ","vnt","ǳ","vnd","Ǵ","vnj","ǵ","vnjd",
			"Ƕ","vnl","Ƿ","vnls","Ǹ","vnlf","ǹ","vnla","Ǻ","vnlt","ǻ","vb","Ǽ","vbs","ǽ","vbf","Ǿ","vba","ǿ","vbt",
			"��","vbd","��","vm","��","vms","��","vmf","��","vma","��","vmq","��","vmt","��","vl","��","vlr","��","vls",
			"��","vlf","��","vla","��","vlq","��","vlt","��","vld","��","gk","��","gkr","��","gks","��","gkf","��","gkfx",
			"��","gka","��","gkq","��","gkt","��","gkd","��","go","��","gor","��","gos","��","gof","��","goa","��","goq",
			"��","got","��","goT","��","god","�d","gog","��","gi","��","gid","��","gj","��","gjr","��","gjs","��","gjf",
			"��","gjfa","��","gja","��","gjq","��","gjt","��","gjd","��","gp","��","gpr","��","gps","��","gpf","��","gpa",
			"��","gpq","��","gpt","��","gpd","��","gu","��","gur","��","gus","��","guf","��","gua","��","guq","��","gut",
			"��","guT","��","gud","��","gP","��","gPs","ȡ","gPf","Ȣ","gPq","ȣ","gh","Ȥ","ghr","ȥ","ghs","Ȧ","ghf",
			"ȧ","ghfx","Ȩ","gha","ȩ","ghq","Ȫ","ght","ȫ","ghd","Ȭ","ghx","ȭ","ghk","Ȯ","ghkr","ȯ","ghks","Ȱ","ghkf",
			"ȱ","ghkt","Ȳ","ghkd","ȳ","gho","ȴ","ghor","ȵ","ghos","ȶ","ghot","ȷ","ghod","ȸ","ghl","ȹ","ghlr","Ⱥ","ghls",
			"Ȼ","ghlf","ȼ","ghlq","Ƚ","ghlt","Ⱦ","ghld","ȿ","gy","��","gys","��","gyf","��","gyq","��","gyt","��","gn",
			"��","gnr","��","gns","��","gnf","��","gnfx","��","gna","��","gnt","��","gnd","��","gnj","��","gnjs","��","gnjf",
			"��","gnja","��","gnjd","��","gnp","��","gnpr","��","gnps","��","gnpf","��","gnpd","��","gnl","��","gnlr","��","gnls",
			"��","gnlf","��","gnla","��","gnlq","��","gnlt","��","gnld","��","gb","��","gbr","��","gbs","��","gbf","��","gba",
			"��","gbt","��","gbd","��","gm","��","gmr","��","gms","��","gmsg","��","gme","��","gmf","��","gmfr","��","gma",
			"��","gmq","��","gmt","��","gmd","��","gmx","��","gml","��","gmls","��","gmlf","��","gmla","��","gmlq","��","gmld",
			"��","gl","��","glr","��","gls","��","glf","��","gla","��","glq","��","glt","��","gld"};

#define STR_MAX	80
#define HAN_MAX 2364
#define	MOUM	19
char jamo[34]="rRseEfaqQtTdwWczxvgkoiOjpuPhynbml";
char* han_jamo[33]={"��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��",
					"��","��","��","��","��","��","��","��","��","��","��","��","��","��"};

class CHan
{
private:
	int mark[20];
	char han[STR_MAX];

	unsigned int FindMark(char eng);

public:
	char* EngToHan(char* text);

	CHan();
};

CHan::CHan()
{
	//�˻� �ӵ� ����� ���� ��������
	int mp=0;
	char* check_text[19]={"��","��","��","��","��","��","��","��","��","��","��","��","��","¥","��","ī","Ÿ","��","��"};
	for(unsigned int i=0; i<HAN_MAX; i++)
	{
		if(strcmp(han1[i*2], check_text[mp])==0)
		{
			mark[mp]=i;
			mp++;
			if(mp>=19)break;
		}
	}
	mark[19]=HAN_MAX;
}

unsigned int CHan::FindMark(char eng)
{
	for(unsigned int j=0; j<19; j++)
		if(eng==jamo[j])return j;

	return 0;
}

char* CHan::EngToHan(char* text)
{
	unsigned int pp=0;
	unsigned int begin[STR_MAX], end[STR_MAX];

	//�Է� �ޱ�
	strcpy(han, "");	

	//����, ���� �ѱ� �ƴ� ���� Ȯ��(���� �� �κ��� ����Ʈ�� �ξ ��ȯ�� ������ �ǰ� ��)
	for(unsigned int i=1; i<strlen(text); i++)
	{
		//�ѱۿ� ��ġ �� �Ǵ� �빮�ڸ� �ҹ��ڷ� �ٲ�
		if(text[i]>='A' && text[i]<='Z' && text[i]!='Q' && text[i]!='W' && text[i]!='E'	&& text[i]!='R'
			&& text[i]!='T' && text[i]!='O' && text[i]!='P')text[i]+=0x20;

		//�������� �������� ����
		for(int j=0; j<33; j++)
			if(text[i]==jamo[j])
			{
				if(j>=MOUM)
				{
					begin[pp]=i-1;
					if(pp>0)end[pp-1]=i-2;
					pp++;
					i++;	//���� ������ ��츦 �����ؼ� ������ �ѱ�(���� ������ ���� �о� ���ڰ� �Ǵ� ���� �����Ƿ� ��: �޻�(O) �ΤӤ���(X))
				}
				break;
			}
	}
	if(pp>0)end[pp-1]=strlen(text)-1;

	//����Ʈ ����(�ѱ��� �κи� �ؼ�)
	for(int i=0; i<pp; i++)
	{
		char buffer[10];
		int bp=0;
		bool head=true;	//�� �κ����� ���� �޺κ����� �˾Ƴ�(�߶󳻾� �ϴ� �κ� ��: 1@��2$ -> 1@�� head=true�� ����, 2$�� head=false�� ���¿��� ����
		for(unsigned int j=begin[i]; j<=end[i]; j++)
		{
			if(text[j]>='a' && text[j]<='z' || text[j]>='A' && text[j]<='Z')
			{
				buffer[bp]=text[j];
				bp++;
				head=false;
			}
			else
			{
				if(head)begin[i]++;
					else end[i]=j-1;
			}
		}
		buffer[bp]=NULL;

		//������ �ѱ۷� ���� �� ��ġ�ϴ� ���� ã��
		while(strlen(buffer)>1)
		{
			bool find=false;
			unsigned int loop_start=FindMark(buffer[0]);
			unsigned int loop_end=mark[loop_start+1];
			loop_start=mark[loop_start];

			for(int j=loop_start; j<loop_end; j++)
				if(strcmp(han1[j*2+1], buffer)==0)
				{
					find=true;
					break;
				}

			//ã�� �� ���� �� ������ �ٿ��� ã�ƺ�
			if(!find)
			{
				buffer[strlen(buffer)-1]=NULL;
				end[i]--;
			}
			else break;
		}

		//���� ���ϴ� �ѱ����� ǥ��
		if(strlen(buffer)<=1)begin[i]=9999;
	}

	//��ȯ
	unsigned int pp2=0;
	for(int i=0; i<strlen(text); i++)
	{
		while(begin[pp2]==9999)pp2++;

		//�ѱ��� �κ�
		if(pp2<pp && i==begin[pp2])
		{
			char buffer[10];
			int bp=0;
			for(unsigned int j=begin[pp2]; j<=end[pp2]; j++)
			{
				buffer[bp]=text[j];
				bp++;
			}
			buffer[bp]=NULL;

			unsigned int loop_start=FindMark(buffer[0]);
			unsigned int loop_end=mark[loop_start+1];
			loop_start=mark[loop_start];
			for(int j=loop_start; j<loop_end; j++)
				if(strcmp(han1[j*2+1], buffer)==0)
				{
					strcat(han, han1[j*2]);
					break;
				}

			i=end[pp2];
			pp2++;
		}
		//�ƴ� �κ�
		else
		{			
			//�ѱ� �϶��� ������ ���� �������� ǥ��
			if(text[i]>='a' && text[i]<='z' || text[i]>='A' && text[i]<='Z')
			{
				for(int j=0; j<33; j++)
				{
					if(text[i]==jamo[j])
					{
						strcat(han, han_jamo[j]);
						break;
					}
				}
			}
			//�ᱹ ���� �� ���� �״��
			else
			{
				char tmp[2];
				tmp[0]=text[i];
				tmp[1]=NULL;
				strcat(han, tmp);
			}			
		}
	}

	han[strlen(han)]=NULL;
	return han;
}

CHan m_han;

/////////////////////////////////////////////////////
//��ȭ ó�� Ŭ����
class CDlg
{
protected:
	//�������� ���� ����
	char* select_point[300];
	bool row;
	int n_of_e;
	int x, y, gap, line;
	int col, xmax, ymax, width;
	char dlg_buffer[1024];
	int select;

	//��� ����� ���� ����
	int tp, sp;
	bool ani_end;
	int length;
	RECT text, shadow;

	char han_name[161];	

	//��ȭâ ���̱�
	void ShowDlg();

public:
	//��ȭâ ��縸 ���
	void ShowDlg(int vx, int vy, int vwidth, int vline);

	//��ȭ ���� �޼ҵ�
	void MakeSelection(char* input_dlg, int vx, int vy, int vgap, int vline, int start=0);
	int Selecting();
	int Select(char* input_dlg, int vx, int vy, int vgap, int vline, int vstart=0, bool cancel=false);
	void MakeText(char* content, int vx, int vy, int vwidth, int vline);
	void Print(char* content, int vx, int vy, int vwidth, int vline);
	int Printing();
	char* InputHan(int vx, int vy, int str_max);
};

#define SNR_START 4

//��ȭ �����
void CDlg::MakeText(char* content, int vx, int vy, int vwidth, int vline)
{
	x=vx; y=vy; width=Max(3,vwidth); line=vline;

	length=strlen(content);
	strcpy(dlg_buffer,content);
	ani_end=false;
	row=false;

	//��ȭ�� ����ϱ� �˸°� ó��
	sp=SNR_START;	//ó���ǰ� �ִ� ����Ʈ
	int space=-1;	//�������� ó���� ����Ʈ, space�� �ִ� ��
	int senlen=0;	//������ ����, �ѱ� ����Ʈ
	
	while(sp<length)
	{
		if(dlg_buffer[sp]&0x80)//�ѱ�
		{
			senlen+=21;
			sp+=2;
		}
		else
		{
			senlen+=11;

			if(dlg_buffer[sp]==' ')space=sp;
			else if(dlg_buffer[sp]=='\\')
			{
				dlg_buffer[sp]='\n';
				space=-1;
				senlen=0;
			}
			sp++;
		}

		int limit;
		if(dlg_buffer[sp]&0x80)limit=width*10-21;
			else limit=width*10-11;
		if(senlen>limit)
		{
			if(space<0)	//�����̽��� �ϳ��� ������ ���� �� ��ȯ
			{
				//���̿� CR�� �־���
				char* tmp_text;
				tmp_text = new char[length+2];

				for(int i=0; i<sp; i++)tmp_text[i]=dlg_buffer[i];
				tmp_text[sp]='\n';
				for(int i=sp+1; i<length+1; i++)tmp_text[i]=dlg_buffer[i-1];
				tmp_text[length+1]=NULL;
				strcpy(dlg_buffer,tmp_text);
				delete[] tmp_text;

				length++;
				sp++;
			}
			else dlg_buffer[space]='\n';

			space=-1;
			senlen=0;
		}		
	}

	SetRect(&text,x,y,640,480);
	SetRect(&shadow,x+1,y+1,640,480);

	sp=SNR_START;
	n_of_e=0;		//�ٹٲ� Ƚ��
	tp=0;			//����ϴ� �ܾ�
	//��ȭâ�� ���� ���
	xmax=x+width*10;
	ymax=y+line*20;
}

//��ȭ ���(������ ���¿���)
void CDlg::Print(char* content, int vx, int vy, int vwidth, int vline)
{
	MakeText(content, vx, vy, vwidth, vline);
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(Printing()==999)break;
		jdd->Render();
		if(!ani_end && !_GetKeyState(VK_RETURN))Sleep(10);
	}
}

//��ȭ���(�ǽð�����)
int CDlg::Printing()
{
	static char text_buffer[1024];

	if(!ani_end)
	{
		if(sp<length && n_of_e<line)
		{
			if(dlg_buffer[sp]=='\n')n_of_e++;
			text_buffer[tp]=dlg_buffer[sp];
			if(dlg_buffer[sp]&0x80)	//�ѱ��� ��� 2ĭ ����
			{
				sp++; tp++;
				text_buffer[tp]=dlg_buffer[sp];
			}
			text_buffer[tp+1]=NULL;
			//������ ����
			sp++; tp++;
		}
		else ani_end=true;
	}
	else
	{
		//��������
		if(GetKey(4))
		{
			if(sp<length)
			{
				ani_end=false;
				n_of_e=0;
				tp=0;
			}
			else return 999;
		}
	}
	//��ȭ ���
	Process();
	Screen();
	ShowDlg();
	jdd->DrawText(backbuffer,text_buffer,font20,&shadow,JColor(0,0,0));
	jdd->DrawText(backbuffer,text_buffer,font20,&text,JColor(255,255,255));

	return 0;
}

int CDlg::Select(char* input_dlg, int vx, int vy, int vgap, int vline, int vstart, bool cancel)
{
	MakeSelection(input_dlg,vx,vy,vgap,vline,vstart);
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		int selected=Selecting();
		if(selected>=0 || (cancel && selected==-999))break;

		jdd->Render();
	}

	return select;
}

void CDlg::MakeSelection(char* input_dlg, int vx, int vy, int vgap, int vline, int start)
{
	strcpy(dlg_buffer,input_dlg);
	int length=strlen(dlg_buffer);
	x=vx; y=vy; gap=vgap; line=vline;

	//���μ��������� ���� ���������� Ȯ�� (���̳ʽ��� ���� ������)
	if(line<0)
	{
		line*=-1;
		row=true;
	}
	else if(line==0)
	{
		line=1;
		row=false;
	}
	else row=false;
		

	//��ȭ�� ����ϱ� �˸°� ó��
	n_of_e=0;
	select=start;
	int bp=1;
	select_point[0]=dlg_buffer;
	int i;
	for(i=0; i<length; i++)
	{
		if(dlg_buffer[i]=='\\')
		{
			dlg_buffer[i]=NULL;
			select_point[bp]=dlg_buffer+i+1;
			n_of_e++;
			bp++;
			continue;
		}
		if(dlg_buffer[i]==13)
		{
			continue;
		}
	}
	//��ũ��Ʈ���� �о� �� ������ ���� ��ǥ�� ����� �� ����
	if(dlg_buffer[i-1]==13)
	{
		dlg_buffer[i-1]=NULL;
	}

	//��ȭâ ǥ�ÿ� �ʿ��� �͵� ���
	if(row)
	{
		col=(n_of_e+1)/line+((((n_of_e+1)%line)==0)?0:1);
		xmax=x+line*gap;
		ymax=y+col*20;
		width=gap/10*line;
	}
	else
	{
		col=(n_of_e+1)/line+((((n_of_e+1)%line)==0)?0:1);
		xmax=x+col*gap;
		ymax=y+line*20;
		width=gap/10*col;
	}

}

int CDlg::Selecting()
{
	Process();
	Screen();
	//��ȭ����&���ù�
	int sx, sy;
	if(row)
	{
		//�� �Ʒ� �̵�
		if(GetKey(2,10))select=Min(n_of_e,select+1);
			else if(GetKey(1,10))select=Max(0,select-1);
		//�¿� �̵�
		if(GetKey(3,10))
		{
			if(select+line<=n_of_e)select+=line;
		}
		else if(GetKey(0,10))
		{
			if(select-line>=0)select-=line;
		}

		ShowDlg();
		sx=x+select%line*gap;
		sy=y+(select/line)*20;
	}
	else
	{
		//�� �Ʒ� �̵�
		if(GetKey(3,10))select=Min(n_of_e,select+1);
			else if(GetKey(0,10))select=Max(0,select-1);

		//�¿� �̵�
		if(GetKey(2,10))
		{
			if(select+line<=n_of_e)select+=line;
		}
		else if(GetKey(1,10))
		{
			if(select-line>=0)select-=line;
		}

		ShowDlg();
		sx=x+select/line*gap;
		sy=y+(select%line)*20;
	}

	RECT src_rect;
	SetRect(&src_rect,0,0,gap/2,20);
	jdd->DrawPicture(backbuffer,PIC_SELECTBAR,sx,sy,&src_rect);
	SetRect(&src_rect,320-gap/2,0,320,20);
	jdd->DrawPicture(backbuffer,PIC_SELECTBAR,sx+gap/2,sy,&src_rect);

	//��������
	for(int i=0; i<=n_of_e; i++)
	{
		int tx,ty;
		if(row)
		{
			tx=(i%line)*gap+x;
			ty=(i/line)*20+y;
		}
		else
		{
			tx=(i/line)*gap+x;
			ty=(i%line)*20+y;
		}

		jdd->DrawText(backbuffer,select_point[i],font20,tx+1,ty+1,JColor(0,0,0));
		jdd->DrawText(backbuffer,select_point[i],font20,tx,ty,JColor(255,255,255));
	}

	if(GetKey(4))return select;
		else if(GetKey(5))return -999;
		else return -1;
}

void CDlg::ShowDlg(int vx, int vy, int vwidth, int vline)
{
	x=vx; y=vy; width=Max(3,vwidth); line=vline;
	row=false;
	xmax=x+width*10;
	ymax=y+line*20;

	ShowDlg();
}

void CDlg::ShowDlg()
{
	//�߸��� �� ����(���ǻ� ������ �������� �ؼ� ��ġ�� �����ϹǷ� �̷��� ����� ��ġ�� �־��)
	width=MaxMin(width, 3, 100);
	col=MaxMin(col, 1, 30);
	line=MaxMin(line, 1, 30);

	//��
	jdd->DrawPicture(backbuffer,"Dlg1",x-5,y-5,NULL);
	for(int i=0; i<width; i++)jdd->DrawPicture(backbuffer,"Dlg2",x+(i*10),y-5,NULL);
	jdd->DrawPicture(backbuffer,"Dlg3",xmax,y-5,NULL);
	//�߰�
	int ysize=(row)?col:line;

	for(int i=0; i<ysize; i++)
	{
		int tmp_y=y+i*20;
		jdd->DrawPicture(backbuffer,"Dlg4",x-5,tmp_y,NULL);
		for(int j=0; j<width; j++)jdd->DrawPicture(backbuffer,"Dlg5",x+(j*10),tmp_y,NULL);
		jdd->DrawPicture(backbuffer,"Dlg4",xmax,tmp_y,NULL);
	}
	//�Ʒ�
	jdd->DrawPicture(backbuffer,"Dlg7",x-5,ymax,NULL);
	for(int i=0; i<width; i++)jdd->DrawPicture(backbuffer,"Dlg2",x+(i*10),ymax,NULL);
	jdd->DrawPicture(backbuffer,"Dlg9",xmax,ymax,NULL);
}

//�ѱ��Է�
char* CDlg::InputHan(int vx, int vy, int str_max)
{
	int old_len=0;
	char eng_buffer[81];
	strcpy(han_name,"");
	strcpy(eng_buffer,"");
	x=vx;
	y=vy;

	while(1)
	{
		Clear();
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//�Էµ� �� ǥ��
		Process();
		Screen();
		ShowDlg(vx, vy, str_max*11/10, 1);
		jdd->DrawText(backbuffer,han_name,font20,x+1,y+1,JColor(0,0,0));
		jdd->DrawText(backbuffer,han_name,font20,x,y,JColor(255,255,255));

		int eng_len=(int)strlen(eng_buffer);
		//�Է�
		if(eng_len<80)
		{
			//����Ű
			if(GetKey(31))
			{
				strcat(eng_buffer, "a");
			}
			else if(GetKey(32))
			{
				strcat(eng_buffer, "b");
			}
			else if(GetKey(33))
			{
				strcat(eng_buffer, "c");
			}
			else if(GetKey(34))
			{
				strcat(eng_buffer, "d");
			}
			else if(GetKey(35))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "E");
					else strcat(eng_buffer, "e");
			}
			else if(GetKey(36))
			{
				strcat(eng_buffer, "f");
			}
			else if(GetKey(37))
			{
				strcat(eng_buffer, "g");
			}
			else if(GetKey(38))
			{
				strcat(eng_buffer, "h");
			}
			else if(GetKey(39))
			{
				strcat(eng_buffer, "i");
			}
			else if(GetKey(40))
			{
				strcat(eng_buffer, "j");
			}
			else if(GetKey(41))
			{
				strcat(eng_buffer, "k");
			}
			else if(GetKey(42))
			{
				strcat(eng_buffer, "l");
			}
			else if(GetKey(43))
			{
				strcat(eng_buffer, "m");
			}
			else if(GetKey(44))
			{
				strcat(eng_buffer, "n");
			}
			else if(GetKey(45))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "O");
					else strcat(eng_buffer, "o");
			}
			else if(GetKey(46))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "P");
					else strcat(eng_buffer, "p");
			}
			else if(GetKey(47))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "Q");
					else strcat(eng_buffer, "q");
			}
			else if(GetKey(48))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "R");
					else strcat(eng_buffer, "r");
			}
			else if(GetKey(49))
			{
				strcat(eng_buffer, "s");
			}
			else if(GetKey(50))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "T");
					else strcat(eng_buffer, "t");
			}
			else if(GetKey(51))
			{
				strcat(eng_buffer, "u");
			}
			else if(GetKey(52))
			{
				strcat(eng_buffer, "v");
			}
			else if(GetKey(53))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "W");
					else strcat(eng_buffer, "w");
			}
			else if(GetKey(54))
			{
				strcat(eng_buffer, "x");
			}
			else if(GetKey(55))
			{
				strcat(eng_buffer, "y");
			}
			else if(GetKey(56))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "Z");
					else strcat(eng_buffer, "z");
			}
			//�����̽�
			else if(GetKey(6))
			{
				strcat(eng_buffer, " ");
			}
			//����, Ư�� ����
			else if(GetKey(21))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, ")");
					else strcat(eng_buffer, "0");
			}
			else if(GetKey(22))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "!");
					else strcat(eng_buffer, "1");
			}
			else if(GetKey(23))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "@");
					else strcat(eng_buffer, "2");
			}
			else if(GetKey(24))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "#");
					else strcat(eng_buffer, "3");
			}
			else if(GetKey(25))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "$");
					else strcat(eng_buffer, "4");
			}
			else if(GetKey(26))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "%");
					else strcat(eng_buffer, "5");
			}
			else if(GetKey(27))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "^");
					else strcat(eng_buffer, "6");
			}
			else if(GetKey(28))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "&");
					else strcat(eng_buffer, "7");
			}
			else if(GetKey(29))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "*");
					else strcat(eng_buffer, "8");
			}
			else if(GetKey(30))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "(");
					else strcat(eng_buffer, "9");
			}
		}
		//Ư�� Ű
		if(GetKey(4))break;
		else if(GetKey(58, 10))
		{
			if(eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
		}

		//�Է� ���� ����
		if(old_len!=eng_len)
		{
			strcpy(han_name, m_han.EngToHan(eng_buffer));
			//�Է� �Ѱ�
			while(str_max<(int)strlen(han_name) && eng_len>0)
			{
				eng_buffer[strlen(eng_buffer)-1]=NULL;
				strcpy(han_name, m_han.EngToHan(eng_buffer));
			}
			old_len=eng_len;
		}

		jdd->Render();
	}

	return han_name;
}

/////////////////////////////////////////////////////
//��ȭ ó�� Ŭ����+��ũ��Ʈ �б� ���
class CHugeDlg:public CDlg
{
	int snrs;			//�ó����� �Ѱ�
	char** snr;			//�ó����� ������
	int mark_no;		//å���� ����
	int bookmark[1000];	//å����

	int process_snr;	//ó�� ���� �ó�����
	int answer;			//ó�� �� ��� �����

	bool IsBookMark(int no);
	void Function(int command);

public:
	void Script(int script_no);

	CHugeDlg(char* dialog_file);
	~CHugeDlg();
};

#define KEY1 8
#define KEY2 4

//��ȭ ���� �ҷ�����
CHugeDlg::CHugeDlg(char* dialog_file)
{	
	if(fp=fopen(dialog_file,"rb"))
	{
		//��� �� �б�
		fread(&snrs,sizeof(int),1,fp);
		//å���� �б�
		fread(&mark_no,sizeof(int),1,fp);
		fread(&bookmark,sizeof(int),mark_no,fp);
		//��系�� �б�
		snr=new char*[snrs];
		for(int snr_no=0; snr_no<snrs; snr_no++)
		{			
			//���Ͽ��� �б�
			int text_size;
			fread(&text_size,sizeof(int),1,fp);
			snr[snr_no]=new char[text_size/2+1];

			char* buffer=new char[text_size+1];
			fread(buffer,text_size,1,fp);
			buffer[text_size]=NULL;

			//�ص�
			for(int i=0; i<text_size/2; i++)
			{
				char b1, b2;
				b1=buffer[i*2];
				b2=buffer[i*2+1];
				b1=get_char_num(b1)-KEY1;
				b2=get_char_num(b2)-30-KEY2;

				snr[snr_no][i]=b1*16+b2;
			}
			snr[snr_no][text_size/2]=NULL;
			//printf("%d:%s\n",snr_no,snr[snr_no]);
			delete[] buffer;
		}
		
		fclose(fp);
	}
}

//��ȭ���� ����
CHugeDlg::~CHugeDlg()
{
	for(int i=0; i<snrs; i++)
		if(snr[i]!=NULL)
		{
			delete[] snr[i];
			snr[i]=NULL;
		}
}

//��ũ��Ʈ ó��
void CHugeDlg::Script(int script_no)
{
	int command;
	char buffer[5];
	script_no=bookmark[script_no];

	do{
		//����� �о� ���̱�
		strncpy(buffer, snr[script_no], 3);
		buffer[3]=NULL;
		command=atoi(buffer);
		process_snr=script_no;

		if(command==999)break;
			else Function(command);
		script_no++;
	}while(script_no<snrs && !IsBookMark(script_no));
}

bool CHugeDlg::IsBookMark(int no)
{
	for(int i=0; i<mark_no; i++)
		if(no==bookmark[i])return true;

	return false;
}

/////////////////////////////////
//��� ó�� Ŭ����
class CCommand
{
	int com_id[64];
	int command_max;
	char** source;
	char commands[1024];

	int GetComID(int count);
	CDlg comdlg;

public:
	int count;

	void Init(char* vsource[], int max=64);
	void AddComs(int n, ...);
	void AddCom(int id);
	void AddComStr(int id, char* str);
	void MakeCommand(int x, int y, int gap, int vline=0);
	int CommandSelect(int x, int y, int gap, int vline=0);
	int CommandSelecting();
	bool IsFull();
	bool Empty();

	CCommand(char* vsource[], int max=64);
};

CCommand::CCommand(char* vsource[], int max)	//�ʱ�ȭ
{
	Init(vsource,max);
}

void CCommand::Init(char* vsource[], int max)
{
	count=0;
	source=vsource;
	command_max=max;
	strcpy(commands,"");
}

void CCommand::AddComs(int n, ...)
{
	va_list ap;
	va_start(ap,n);
	for(int i=0; i<n; i++)AddCom(va_arg(ap,int));
	va_end(ap);
}

void CCommand::AddCom(int id)	//����� �� �ִ� ��� �߰�
{
	if(count>=command_max)return;	//��ɾ �� ��
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,source[id]);
	com_id[count]=id;
	count++;
}

void CCommand::AddComStr(int id, char* str)
{
	if(count>=command_max)return;	//��ɾ �� ��
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,str);
	com_id[count]=id;
	count++;
}

int CCommand::GetComID(int count)
{
	if(count>=0 && count<command_max)return com_id[count];
		else return -1;
}

bool CCommand::IsFull()
{
	if(count>=command_max)return true;
		else return false;
}

void CCommand::MakeCommand(int x, int y, int gap, int vline)
{
	if(vline==0)vline=Min(10,count);
	comdlg.MakeSelection(commands,x,y,gap,vline);
}

int CCommand::CommandSelecting()
{
	if(count<1)return -1;		//��� ���� ����� ������ �������� ����

	int selected;
	selected=comdlg.Selecting();

	if(selected>=0)return GetComID(selected);
		else return -1;
}

int CCommand::CommandSelect(int x, int y, int gap, int vline)
{
	if(vline==0)vline=Min(10,count);

	comdlg.MakeSelection(commands,x,y,gap,vline);
	int selected;
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		selected=comdlg.Selecting();
		if(selected>=0)break;

		jdd->Render();
	}

	return GetComID(selected);
}

bool CCommand::Empty()
{
	if(count==0)return true;
		else return false;
}

//////////////////////////////////////
//���� ó�� Ŭ����
#define FILEMAX 35

class CFiles
{
	void AddFile(char* name);
public:
	char filename[FILEMAX][40];
	int count;
	void SearchFile(char* dir, char* filename);

	CFiles();
};

CFiles::CFiles()
{
	count=0;
}

void CFiles::AddFile(char* name)
{
	if(count>=FILEMAX)return;
	strcpy(filename[count],name);
	count++;
}

void CFiles::SearchFile(char* dir, char* filetype)
{
	HANDLE hFind;
    WIN32_FIND_DATA FindData;
    int ErrorCode;
    BOOL Continue = TRUE;

	char search_for[30];
	strcpy(search_for,dir);
	strcat(search_for,"\\*.");
	strcat(search_for,filetype);

    hFind = FindFirstFile(search_for, &FindData);

    if(hFind == INVALID_HANDLE_VALUE)
    {
        ErrorCode = GetLastError();
        if (ErrorCode == ERROR_FILE_NOT_FOUND)
        {
            //printf("There are no files matching that path/mask\n");
        }
        else
        {
            //printf("FindFirstFile() returned error code %d", ErrorCode);
        }
        Continue = FALSE;
    }
    else
    {
        AddFile(FindData.cFileName);
    }

    if (Continue)
    {
        while (FindNextFile(hFind, &FindData))
        {
           AddFile(FindData.cFileName);
        }

        ErrorCode = GetLastError();

        if (ErrorCode != ERROR_NO_MORE_FILES)
        {
            //printf("FindNextFile() returned error code %d", ErrorCode);
        }

        if (!FindClose(hFind))
        {
            ErrorCode = GetLastError();
            //printf("FindClose() returned error code %d", ErrorCode);
        }
    }
}

//////////////////////////////////////////////////////////////////
//�ִϸ��̼� Ŭ����
#define LOOP_TYPE_ANI		1
#define EXCHANGE_TYPE_ANI	2

class CAnimation
{
private:
	int x_size, y_size;
	bool walk;							//���� ����(�⺻��: false)
	int top, bottom, left, right;		//���� �׷����� ���� �����¿� �ִϸ��̼� �׸��� ��ġ(Y��ǥ�� ����)
	int frame, frame_max;
	int delay, delay_max;

public:
	char type;
	RECT ani_rect;

	CAnimation();
	CAnimation(int x, int y, char vtype=0, int vframe_max=0, int vdelay_max=0);
	void Set(int x, int y, char vtype=0, int vframe_max=0, int vdelay_max=0);
	void Process();
	void SetWalk(int t, int b, int l, int r);
	void SetFrame(int vframe);
	int GetXSize();
	int GetYSize();
};

//CAnimation �޼ҵ�
CAnimation::CAnimation()
{
	frame=delay=0;
	walk=false;
}

CAnimation::CAnimation(int x, int y, char vtype, int vframe_max, int vdelay_max)
{
	Set(x, y, vtype, vframe_max, vdelay_max);
	CAnimation();
}

void CAnimation::Set(int x, int y, char vtype, int vframe_max, int vdelay_max)
{
	x_size=x;
	y_size=y;
	SetRect(&ani_rect, 0, 0, x, y);
	type=vtype;
	if(type==2)frame_max=vframe_max*2-1;
		else frame_max=vframe_max;
	delay_max=vdelay_max;
}

void CAnimation::Process()
{
	if(type==1)	//������ ���� �ִϸ��̼�
	{
		delay++;
		if(delay>=delay_max)
		{
			delay=0;
			frame++;
			if(frame>=frame_max)frame=0;
		}
		
		SetRect(&ani_rect, x_size*frame, 0, x_size*(frame+1), y_size);
	}
	else if(type==2)	//�ݺ� �ִϸ��̼�
	{
		delay++;
		if(delay>=delay_max)
		{
			delay=0;
			frame++;
			if(frame>=frame_max)frame=0;
		}
		int frame2=abs((frame_max/2)-frame);
		SetRect(&ani_rect, x_size*frame2, 0, x_size*(frame2+1), y_size);
	}
}

void CAnimation::SetWalk(int t, int b, int l, int r)
{
	top=t;
	bottom=b;
	left=l;
	right=r;
}

void CAnimation::SetFrame(int vframe)
{
	frame=vframe;
	SetRect(&ani_rect, x_size*vframe, 0, x_size*(vframe+1), y_size);
}

int CAnimation::GetXSize()
{
	return x_size;
}

int CAnimation::GetYSize()
{
	return y_size;
}

void DrawPicture(char* src, int x, int y, int frame=0, CAnimation* ani=NULL)
{
	if(ani!=NULL)
	{
		if(frame>=0)ani->SetFrame(frame);
		jdd->DrawPicture(backbuffer, src, x, y, &ani->ani_rect);
	}
	else jdd->DrawPicture(backbuffer, src, x, y, NULL);
}

//////////////////////////////////////////////////////
//ť Ŭ����
#define QUE_MAX	100

class QueData
{
public:
	unsigned char command;
	int* value;
	char* content;

	void Delete();
};

void QueData::Delete()
{
	command=-1;

	if(content!=NULL)
	{
		delete[] content;
		content=NULL;
	}

	if(value!=NULL)
	{
		delete[] value;
		value=NULL;
	}
}

class Que
{
	QueData *que;

	int que_max;
	bool empty;

public:
	//����, �Ҹ�, ����, ����
	void Init();
	void Destroy();
	void Insert(unsigned char command, char* content, unsigned char values, ...);
	bool Delete(int id);

	//�� ���
	int GetMax();
	QueData* GetQue(int id);
};

void Que::Init()
{
	//ť ���� Ȯ��
	que=new QueData[QUE_MAX];

	//������ �ʱ�ȭ
	for(int i=0; i<QUE_MAX; i++)
	{
		que[i].command=-1;
		que[i].value=NULL;
		que[i].content=NULL;
	}

	//�Ѱ� �ʱ�ȭ
	que_max=0;
}

void Que::Destroy()
{
	for(int i=0; i<que_max; i++)
		if(que[i].command>=0)que[i].Delete();

	if(que!=NULL)
	{
		delete [] que;
		que=NULL;
	}
}

void Que::Insert(unsigned char command, char* content, unsigned char values, ...)
{
	int id;
	bool empty=false;

	//�� ť�� ����
	for(int i=0; i<que_max; i++)
	{
		if(que[i].command<0)
		{
			id=i;
			empty=true;			
			break;
		}
	}

	//���ο� �ڸ� �����
	if(!empty && que_max<QUE_MAX-1)
	{
		id=que_max;
		que_max++;
	}

	//���
	que[id].command=command;

	//���ڿ�
	if(content!=NULL)
	{
		que[id].content=new char[strlen(content)+1];
		strcpy(que[id].content, content);
	}

	//���ڿ�
	if(values>0)
	{
		que[id].value=new int[values];
		va_list ap;
		va_start(ap,values);
		for(int i=0; i<values; i++)que[id].value[i]=va_arg(ap,int);
		va_end(ap);
	}

}

bool Que::Delete(int id)
{
	que[id].Delete();
	while(id==que_max-1)
	{
		que_max--;
		if(id>0)
		{
			if(que[id-1].command<0)id--;
		}
		else return true;
	}
	return false;
}

int Que::GetMax()
{
	return que_max;
}

QueData* Que::GetQue(int id)
{
	return &que[id];
}