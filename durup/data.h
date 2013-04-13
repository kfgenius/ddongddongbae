/*  DATA.H¿¡ ´ëÇÏ¿©
-------------------------
DATA.H´Â °ÔÀÓ ¸¸µé ¶§ °øÅëÀûÀ¸·Î ¾²ÀÌ´Â ÇÔ¼ö³ª µ¥ÀÌÅÍ°¡ µé¾îÀÖ´Ù.
±×·¸´Ù°í ÇÏ³ªÀÇ ¶óÀÌºê·¯¸®·Î º¸±ä ¾î·Æ´Ù.
»ç½Ç ¸ŞÀÎ°ú ºĞ¸®µÇ¾î ¾µ ¼öµµ ¾ø°í, ´Ù¸¥ °÷¿¡ °áÇÕµµ Èûµé´Ù.
±×¸®°í ±×´ë·Î ¾²ÀÌ´Â °Ô ¾Æ´Ï¶ó °ÔÀÓ¿¡ µû¶ó ¾Ë¸Â°Ô ¾à°£ ¼öÁ¤ÇØ¾ß µÈ´Ù.
±×·¯´Ï±î ±×³É ¸¸µé¶§¸¶´Ù ¹İº¹µÇ´Â ±ÍÂúÀº ºÎºĞÀ» ÁÙÀÌ´Â ¿ªÈ°À» ÇÏ´Â ¼Ò½º´Ù.
*/


//¹º°¡ ±¸ºĞÀÌ ÇÊ¿äÇÒ ¶§ ¾²´Â ¼ıÀÚ
#define N_ZERO		0
#define N_ONE		1
#define N_TWO		2
#define N_THREE		3
#define N_FOUR		4
#define N_FIVE		5

//±âº» ±×¸² ÆÄÀÏµé
#define PIC_DLG			"Dlg"
#define PIC_SELECTBAR	"Select"
#define PIC_BLACK		"Black"
#define PIC_WHITE		"White"

#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))

FILE* fp;

char global_buffer[1024];

HWND hwnd;
//»ç¿îµå
HSNDOBJ Sound[100];

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

BOOL SoundCard;
BOOL ReplayFlag;

//¹Ìµğ ¿¬ÁÖ
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

//ÃÖ´ë ÃÖ¼Ò°ª
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

//ÃÖ´ë ÃÖ¼Ò°ª
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

//Å°
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

//Å° Ã³¸®
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

//¸®½ºÆ® Å¬·¡½º
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

/*È­¸é ±×¸®±â¿Í ÇÁ·Î¼¼½º, ÄÁÆ®·Ñ
 ´ëÈ­ Ãâ·Â Å¬·¡½º °°Àº °÷¿¡¼­ ÇÁ·Î¼¼½º¸¦ µ¶Á¡ÇØ¾ß Ã³¸®°¡ ÆíÇÑ ºÎºĞÀÌ ¸¹¾Æ¼­,
 Â÷¶ó¸® Ãâ·ÂÀÌ³ª Á¦¾î ºÎºĞÀ» ¸ğµå¿¡ µû¶ó ´Ş¶óÁö°Ô ÇÏ´Â ÇÔ¼ö·Î ¸¸µé°í,
 Å¬·¡½º°¡ ½Ã½ºÅÛÀ» µ¶Á¡ÇÏ¸é¼­ ÀÌ¸¦ È£ÃâÇÏ´Â ¹æ½ÄÀ¸·Î Á¦ÀÛ
 Áï, È­¸é Ãâ·ÂÀÇ ¸ğµç °ÍÀº Screen ÇÏ³ª°¡ ´ã´çÇÏ°í, Ã³¸®´Â Process ÇÏ³ª°¡ ´ã´çÇÔ.
 ÅØ½ºÆ® Ãâ·Â °°Àº °æ¿ì, ÅØ½ºÆ® Ãâ·ÂÀÇ ·çÇÁ·Î µé¾î°¡¼­ ±× ·çÇÁ ¾È¿¡¼­ ÀÌ ÇÔ¼ö¸¦ Ãâ·ÂÇÔÀ¸·Î,
 ±× ·çÇÁ¿¡¼­ µ¹Áö¸¸ ÀüÃ¼ÀûÀÎ Ãâ·Â°ú Á¦¾î°¡ µÇ°Ô ÇÔ.
*/
int game_mode;
static void Screen();
static void Process();
static void Control();

//¹®ÀÚ¿­ Ã³¸® ÇÔ¼ö
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

//¼ıÀÚ ¼±ÅÃÁö ¸¸µé±â
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

//¾ÏÈ£È­¸¦ À§ÇÑ ¹®ÀÚ¿­
char CodeTo[76]="dyKp:8jP;[R^FIqN0WsTX4U`a52Z]1HnbuVvlh76\\ti=SArJ@>Q_wgkoB?LCcYD3<M9EmGxeOfz";

int get_char_num(char chr)
{
	int i;
	for(i=0; i<76; i++)
		if(CodeTo[i]==chr)break;

	return i;
}

//Àá½Ã Á¤Áö
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

//È­¸é Áö¿ì±â
void Clear()
{
	for(int j=0; j<24; j++)
		jdd->DrawPicture(backbuffer,PIC_BLACK,0,j*20,NULL);
}

//ÆäÀÌµå ¾Æ¿ô
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

	//¿ÏÀüÇÏ°Ô Áö¿ò
	for(int j=0; j<24; j++)
		jdd->DrawPicture(backbuffer,PIC_BLACK,0,j*20,NULL);
	jdd->Render();
}

//È­ÀÌÆ® ¾Æ¿ô
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

	//¿ÏÀüÇÏ°Ô Áö¿ò
	for(int j=0; j<24; j++)
		jdd->DrawPicture(backbuffer,PIC_WHITE,0,j*20,NULL);
	jdd->Render();
}

/////////////////////////////////////////////////////
//ÇÑ±Û ÀÔ·Â ½Ã½ºÅÛ
char* han1[]={"°¡","rk","°¢","rkr","A","rkR","°£","rks","°¤","rke","°¥","rkf","°¦","rkfr","°§","rkfa","E","rkfq","F","rkft",
			"G","rkfx","H","rkfv","I","rkfg","°¨","rka","°©","rkq","°ª","rkqt","°«","rkt","°¬","rkT","°­","rkd","°®","rkw",
			"°¯","rkc","°°","rkx","°±","rkv","°²","rkg","°³","ro","°´","ror","°µ","ros","°¶","roe","°·","roa","°¸","roq",
			"°¹","rot","°º","roT","°»","rod","°¼","ri","°½","rir","°¾","ris","°¿","rif","°À","rit","°Á","rid","°Â","rO",
			"°Ã","rOs","°Ä","rOf","°Å","rj","°Æ","rjr","°Ç","rjs","°È","rje","°É","rjf","°Ê","rjfa","°Ë","rja","°Ì","rjq",
			"°Í","rjt","°Î","rjT","°Ï","rjd","°Ğ","rjw","°Ñ","rjx","°Ò","rjv","°Ó","rjg","°Ô","rp","°Õ","rps","°Ö","rpf",
			"°×","rpa","°Ø","rpq","°Ù","rpt","°Ú","rpT","°Û","rpd","°Ü","ru","°İ","rur","°Ş","rur","°ß","rus","°à","rue",
			"°á","ruf","°â","rua","°ã","ruq","°ä","rut","°å","ruT","°æ","rud","°ç","rux","°è","rP","°é","rPs","°ê","rPf",
			"°ë","rPq","°ì","rPt","°í","rh","°î","rhr","°ï","rhs","°ğ","rhe","°ñ","rhf","°ò","rhfa","°ó","rhft","°ô","rhfg",
			"°õ","rha","°ö","rhq","°÷","rht","°ø","rhd","°ù","rhw","°ú","rhk","°û","rhkr","°ü","rhks","°ı","rhkf","°ş","rhkfa",
			"±¡","rhka","±¢","rhkq","±£","rhkt","±¤","rhkd","±¥","rho","±¦","rhos","±§","rhof","±¨","rhoq","±©","rhoT","±ª","rhod",
			"±«","rhl","±¬","rhlr","±­","rhls","±®","rhlf","±¯","rhla","±°","rhlq","±±","rhlt","±²","rhld","±³","ry","±´","rys",
			"±µ","ryf","±¶","ryq","±·","ryt","±¸","rn","±¹","rnr","±º","rns","±»","rne","±¼","rnf","±½","rnfr","±¾","rnfa",
			"±¿","rnfg","±À","rna","±Á","rnq","±Â","rnt","±Ã","rnd","±Ä","rnw","±Å","rnj","±Æ","rnjr","±Ç","rnjs","±È","rnjf",
			"±É","rnjT","±Ê","rnjd","±Ë","rnp","±Ì","rnpt","±Í","rnl","±Î","rnlr","±Ï","rnls","±Ğ","rnlf","±Ñ","rnla","±Ò","rnlq",
			"±Ó","rnlt","±Ô","rb","±Õ","rbs","±Ö","rbf","±×","rm","±Ø","rmr","±Ù","rms","±Ú","rme","±Û","rmf","±Ü","rmfr",
			"±İ","rma","±Ş","rmq","±ß","rmt","±à","rmd","±á","rml","±â","rl","±ã","rlr","±ä","rls","±å","rle","±æ","rlf",
			"±ç","rlfa","±è","rla","±é","rlq","±ê","rlt","±ë","rld","±ì","rlw","±í","rlv","±î","Rk","±ï","Rkr","±ğ","RkR",
			"±ñ","Rks","±ò","Rkf","±ó","Rkfa","±ô","Rka","±õ","Rkq","±ö","Rkt","±÷","RkT","±ø","Rkd","±ù","Rkx","±ú","Ro",
			"±û","Ror","±ü","Ros","±ı","Rof","±ş","Roa","²¡","Roq","²¢","Rot","²£","RoT","²¤","Rod","²¥","Ri","²¦","Rir",
			"²§","Rif","²¨","Rj","²©","Rjr","²ª","RjR","²«","Rjs","²¬","Rjf","²­","Rja","²®","Rjq","²¯","Rjt","²°","RjT",
			"²±","Rjd","²²","Rp","²³","Rpr","ƒô","RpR","²´","Rps","²µ","Rpa","²¶","Rpt","²·","Rpd","²¸","Ru","²¹","Rus",
			"²º","Ruf","²»","Rut","²¼","RuT","²½","Rux","²¾","RP","²¿","Rh","²À","Rhr","²Á","Rhs","²Ä","Rha","²Â","Rhsg",
			"²Ã","Rhf","²Ä","Rha","²Å","Rhq","²Æ","Rht","²Ç","Rhd","²È","Rhw","²É","Rhc","²Ê","Rhk","²Ë","Rhkr","²Ì","Rhkf",
			"²Í","RhkT","²Î","Rhkd","²Ï","Rho","²Ğ","Rhor","²Ñ","Rhod","²Ò","Rhl","²Ó","Rhls","²Ô","RHlf","²Õ","Rhla","²Ö","Rhlq",
			"²×","Rhld","²Ø","Ry","²Ù","Rn","²Ú","Rnr","²Û","Rns","²Ü","Rnf","²İ","Rnfg","²Ş","Rna","²ß","Rnq","²à","Rnt",
			"²á","Rnd","²â","Rnw","²ã","Rnj","²ä","Rnjf","²å","RnjT","²æ","Rnjd","²ç","Rnp","²è","Rnpr","²é","Rnps","²ê","Rnpf",
			"²ë","Rnpa","²ì","Rnpq","²í","RnpT","²î","Rnl","²ï","Rnls","²ğ","Rnlf","²ñ","Rnla","²ò","Rnlq","²ó","Rb","²ô","Rm",
			"²õ","Rmr","²ö","Rms","²÷","Rmsg","²ø","Rmf","²ù","Rmfa","²ú","Rmfg","²û","Rma","²ü","Rmq","²ı","Rmt","²ş","Rmd",
			"³¡","Rmx","³¢","Rl","³£","Rlr","³¤","Rls","³¥","Rlf","³¦","Rla","³§","Rlq","³¨","Rlt","³©","Rld","³ª","sk",
			"³«","skr","³¬","skR","³­","sks","³®","ske","³¯","skf","³°","skfr","³±","skfa","³²","ska","³³","skq","³´","skt",
			"³µ","skT","³¶","skd","³·","skw","³¸","skc","³¹","skx","³º","skg","³»","so","³¼","sor","³½","sos","³¾","sof",
			"³¿","soa","³À","soq","³Á","sot","³Â","soT","³Ã","sod","³Ä","si","³Å","sir","³Æ","sis","³Ç","sif","³È","sia",
			"³É","sid","³Ê","sj","³Ë","sjr","³Ì","sjt","³Í","sjs","³Î","sjf","³Ï","sjfa","³Ğ","sjfq","³Ñ","sja","³Ò","sjq",
			"³Ó","sjt","³Ô","sjT","³Õ","sjd","³Ö","sjg","³×","sp","³Ø","spr","³Ù","sps","³Ú","spf","³Û","spa","³Ü","spq",
			"³İ","spt","³Ş","spT","³ß","spd","³à","su","³á","sur","³â","sus","³ã","suf","³ä","sua","³å","suq","³æ","suT",
			"³ç","sud","³è","suz","³é","sP","³ê","sPs","³ë","sh","³ì","shr","³í","shs","³î","shf","³ï","shfa","³ğ","sha",
			"³ñ","shq","³ò","sht","³ó","shd","³ô","shv","³õ","shg","³ö","shk","³÷","shks","³ø","shkf","³ù","shkT","³ú","shl",
			"³û","shls","³ü","shlf","³ı","shla","³ş","shlq","´¡","shlt","´¢","sy","´£","syr","´¤","sys","´¥","syf","´¦","syq",
			"´§","syt","´¨","syd","´©","sn","´ª","snr","´«","sns","´¬","sne","´­","snf","´®","sna","´¯","snq","´°","snt",
			"´±","snd","´²","snj","´³","snjT","´´","snp","´µ","snl","´¶","snls","´·","snlf","´¸","snla","´¹","snlq","´º","sb",
			"´»","sbr","´¼","sbf","´½","sba","´¾","sbq","´¿","sbd","´À","sm","´Á","smr","´Â","sms","´Ã","smf","´Ä","smfr",
			"´Å","smfa","´Æ","sma","´Ç","smq","´È","smt","´É","smd","´Ê","smw","´Ë","smv","´Ì","sml","´Í","smls","´Î","smlf",
			"´Ï","sl","´Ğ","slr","´Ñ","sls","´Ò","slf","´Ó","slfa","´Ô","sla","´Õ","slq","´Ö","slt","´×","sld","´Ø","slv",
			"´Ù","ek","´Ú","ekr","´Û","ekR","´Ü","eks","´İ","eke","´Ş","ekf","´ß","ekfr","´à","ekfa","´á","ekfq","´â","ekfg",
			"´ã","eka","´ä","ekq","´å","ekt","´æ","ekT","´ç","ekd","´è","ekw","´é","ekc","´ê","ekg","´ë","eo","´ì","eor",
			"´í","eos","´î","eof","´ï","eoa","´ğ","eoq","´ñ","eot","´ò","eoT","´ó","eod","´ô","ei","´õ","ej","´ö","ejr",
			"´÷","ejR","´ø","ejs","´ù","eje","´ú","ejf","´û","ejfa","´ü","ejfq","´ı","eja","´ş","ejq","µ¡","ejt","µ¢","ejd",
			"µ£","ejc","µ¤","ejv","µ¥","ep","µ¦","epr","µ§","eps","µ¨","epf","µ©","epa","µª","epq","µ«","ept","µ¬","epT",
			"µ­","epd","µ®","eu","µ¯","eus","µ°","euf","µ±","euT","µ²","eud","µ³","eP","µ´","ePs","µµ","eh","µ¶","ehr",
			"µ·","ehs","µ¸","ehe","µ¹","ehf","µº","ehfa","µ»","ehft","µ¼","eha","µ½","ehq","µ¾","eht","µ¿","ehd","µÀ","ehc",
			"µÁ","ehx","µÂ","ehk","µÃ","ehks","µÄ","ehkf","µÅ","eho","µÆ","ehoT","µÇ","ehl","µÈ","ehls","µÉ","ehlf","µÊ","ehla",
			"µË","ehlq","µÌ","ehlt","µÍ","ey","µÎ","en","µÏ","enr","µĞ","ens","µÑ","enf","µÒ","ena","µÓ","enq","µÔ","ent",
			"µÕ","end","µÖ","enj","µ×","enjT","µØ","enp","µÙ","enpd","µÚ","enl","µÛ","enls","µÜ","enlf","µİ","enlq","µŞ","enlt",
			"µß","enld","µà","eb","µá","ens","µâ","enf","µã","ena","µä","end","µå","em","µæ","emr","µç","ems","µè","eme",
			"µé","emf","µê","emfa","µë","ema","µì","emq","µí","emt","µî","emd","µï","eml","µğ","el","µñ","elr","µò","els",
			"µó","ele","µô","elf","µõ","ela","µö","elq","µ÷","elt","µø","elT","µù","eld","µú","elw","µû","Ek","µü","Ekr",
			"µı","Eks","µş","Ekf","¶¡","Eka","¶¢","Ekq","¶£","Ekt","¶¤","EkT","¶¥","Ekd","¶¦","Ekg","¶§","Eo","¶¨","Eor",
			"¶©","Eos","¶ª","Eof","¶«","Eoa","¶¬","Eoq","¶­","Eot","¶®","EoT","¶¯","Eod","¶°","Ej","¶±","Ejr","¶²","Ejs",
			"¶³","Ejf","¶´","Ejfa","¶µ","Ejfq","¶¶","Eja","¶·","Ejq","¶¸","Ejt","¶¹","EjT","¶º","Ejd","¶»","Ejg","¶¼","Ep",
			"¶½","Epr","¶¾","Eps","¶¿","Epf","¶À","Epa","¶Á","Epq","¶Â","Ept","¶Ã","EpT","¶Ä","Epd","¶Å","Eu","¶Æ","EuT",
			"¶Ç","Eh","¶È","Ehr","¶É","Ehs","¶Ê","Ehf","¶Ë","Ehd","¶Ì","Ehk","¶Í","Ehkf","¶Î","Eho","¶Ï","Ehl","¶Ğ","Ehls",
			"¶Ñ","En","¶Ò","Enr","¶Ó","Ens","¶Ô","Enf","¶Õ","Enfg","¶Ö","Ena","¶×","End","¶Ø","Enp","¶Ù","Enl","¶Ú","Enls",
			"¶Û","Enlf","¶Ü","Enla","¶İ","Enlq","¶Ş","Enld","¶ß","Em","¶à","Emr","¶á","Ems","¶â","Eme","¶ã","Emf","¶ä","Ema",
			"¶å","Emq","¶æ","Emt","¶ç","Eml","¶è","Emls","¶é","Emlf","¶ê","Emla","¶ë","Emlq","¶ì","El","¶í","Els","¶î","Elf",
			"¶ï","Ela","¶ğ","Elq","¶ñ","Elt","¶ò","Eld","¶ó","fk","¶ô","fkr","¶õ","fks","¶ö","fkf","¶÷","fka","¶ø","fkq",
			"¶ù","fkt","¶ú","fkT","¶û","fkd","¶ü","fkw","¶ı","fkv","¶ş","fkg","·¡","fo","·¢","for","·£","fos","·¤","fof",
			"·¥","foa","·¦","foq","·§","fot","·¨","foT","·©","fod","·ª","fi","·«","fir","·¬","fis","·­","fit","·®","fid",
			"·¯","fj","·°","fjr","·±","fjs","·²","fjf","·³","fja","·´","fjq","·µ","fjt","·¶","fjT","··","fjd","·¸","fjg",
			"·¹","fp","·º","fpr","·»","fps","·¼","fpf","·½","fpa","·¾","fpq","·¿","fpt","·À","fpd","·Á","fu","·Â","fur",
			"·Ã","fus","·Ä","fuf","·Å","fua","·Æ","fuq","·Ç","fut","·È","fuT","·É","fud","·Ê","fP","·Ë","fPs","·Ì","fPq",
			"·Í","fPt","·Î","fh","·Ï","fhr","·Ğ","fhs","·Ñ","fhf","·Ò","fha","·Ó","fhq","·Ô","fht","·Õ","fhd","·Ö","fhk",
			"·×","fhks","·Ø","fhkd","·Ù","fhoT","·Ú","fhl","·Û","fhls","·Ü","fhlf","·İ","fhla","·Ş","fhlq","·ß","fhlt","·à","fhld",
			"·á","fy","·â","fys","·ã","fyf","·ä","fyq","·å","fyt","·æ","fyd","·ç","fn","·è","fnr","·é","fns","·ê","fnf",
			"·ë","fna","·ì","fnq","·í","fnt","·î","fnd","·ï","fnj","·ğ","fnjT","·ñ","fnp","·ò","fnl","·ó","fnlr","·ô","fnls",
			"·õ","fnlf","·ö","fnla","·÷","fnlt","·ø","fnld","·ù","fb","·ú","fbr","·û","fbs","·ü","fbf","·ı","fba","·ş","fbq",
			"¸¡","fbt","¸¢","fbd","¸£","fm","¸¤","fmr","¸¥","fms","¸¦","fmf","¸§","fma","¸¨","fmq","¸©","fmt","¸ª","fmd",
			"¸«","fmw","¸¬","fmx","¸­","fmv","¸®","fl","¸¯","flr","¸°","fls","¸±","flf","¸²","fla","¸³","flq","¸´","flt",
			"¸µ","fld","¸¶","ak","¸·","akr","¸¸","aks","¸¹","aksg","¸º","ake","¸»","akf","¸¼","akfr","¸½","aka","¸¾","aka",
			"¸¿","akq","¸À","akt","¸Á","akd","¸Â","akw","¸Ã","akx","¸Ä","akg","¸Å","ao","¸Æ","aor","¸Ç","aos","¸È","aof","¸É","aoa",
			"¸Ê","aoq","¸Ë","aot","¸Ì","aoT","¸Í","aod","¸Î","aow","¸Ï","ai","¸Ğ","air","¸Ñ","aif","¸Ò","aid","¸Ó","aj","¸Ô","ajr",
			"¸Õ","ajs","¸Ö","ajf","¸×","ajfa","¸Ø","aja","¸Ù","ajq","¸Ú","ajt","¸Û","ajd","¸Ü","ajw","¸İ","ajg","¸Ş","ap","¸ß","apr",
			"¸à","aps","¸á","apf","¸â","apa","¸ã","apq","¸ä","apt","¸å","apT","¸æ","apd","¸ç","au","¸è","aur","¸é","aus",
			"¸ê","auf","¸ë","aut","¸ì","auT","¸í","aud","¸î","auc","¸ï","aP","¸ğ","ah","¸ñ","ahr","¸ò","ahrt","¸ó","ahs","¸ô","ahf",
			"¸õ","ahfa","¸ö","aha","¸÷","ahq","¸ø","aht","¸ù","ahd","¸ú","ahk","¸û","ahks","¸ü","ahkT","¸ı","ahkd","¸ş","ahl","¹¡","ahls",
			"¹¢","ahlf","¹£","ahlq","¹¤","ahlt","¹¥","ahld","¹¦","ay","¹§","ays","¹¨","ayf","¹©","ayq","¹ª","ayt","¹«","an","¹¬","anr",
			"¹­","anR","¹®","ans","¹¯","ane","¹°","anf","¹±","anfr","¹²","anfa","¹³","ana","¹´","anq","¹µ","ant","¹¶","and",
			"¹·","anx","¹¸","ang","¹¹","anj","¹º","anjs","¹»","anjf","¹¼","anjq","¹½","anjt","¹¾","anp","¹¿","anl","¹À","anls","¹Á","anlf",
			"¹Â","ab","¹Ã","abs","¹Ä","abf","¹Å","aba","¹Æ","abt","¹Ç","am","¹È","ams","¹É","amf","¹Ê","ama","¹Ë","amt","¹Ì","al",
			"¹Í","alr","¹Î","als","¹Ï","ale","¹Ğ","alf","¹Ñ","alfa","¹Ò","ala","¹Ó","alq","¹Ô","alt","¹Õ","alT","¹Ö","ald","¹×","alC",
			"¹Ø","alx","¹Ù","qk","¹Ú","qkr","¹Û","qkR","¹Ü","qkrt","¹İ","qks","¹Ş","qke","¹ß","qkf","¹à","qkfr","¹á","qkfa",
			"¹â","qkfq","¹ã","qka","¹ä","qkq","¹å","qkt","¹æ","qkd","¹ç","qkx","¹è","qo","¹é","qor","¹ê","qos","¹ë","qof","¹ì","qoa",
			"¹í","qoq","¹î","qot","¹ï","qoT","¹ğ","qod","¹ñ","qox","¹ò","qi","¹ó","qir","¹ô","qis","¹õ","qiq","¹ö","qj","¹÷","qjr",
			"¹ø","qjs","¹ù","qje","¹ú","qjf","¹û","qjfa","¹ü","qja","¹ı","qjq","¹ş","qjt","º¡","qjd","º¢","qjw","º£","qp","º¤","qpr",
			"º¥","qps","º¦","qpe","º§","qpf","º¨","qpa","º©","qpq","ºª","qpt","º«","qpT","º¬","qpd","º­","qu","º®","qur",
			"º¯","qus","º°","quf","º±","quq","º²","qut","º³","quT","º´","qud","ºµ","qux","º¶","qP","º·","qPs","º¸","qh","º¹","qhr",
			"ºº","qhR","º»","qhs","º¼","qhf","º½","qha","º¾","qhq","º¿","qht","ºÀ","qhd","ºÁ","qhk","ºÂ","qhks","ºÃ","qhkT","ºÄ","qho",
			"ºÅ","qhoT","ºÆ","qhl","ºÇ","qhlr","ºÈ","qhls","ºÉ","qhlf","ºÊ","qhla","ºË","qhlq","ºÌ","qy","ºÍ","qys","ºÎ","qn","ºÏ","qnr",
			"ºĞ","qns","ºÑ","qne","ºÒ","qnf","ºÓ","qnfr","ºÔ","qnfa","ºÕ","qna","ºÖ","qnq","º×","qnt","ºØ","qnd","ºÙ","qnx",
			"ºÚ","qnv","ºÛ","qnj","ºÜ","qnjf","ºİ","qnjT","ºŞ","qnp","”í","qnpf","”î","qnpfr","ºß","qnl","ºà","qnlr","ºá","qnls","ºâ","qnlf","ºã","qnld","ºä","qb",
			"ºå","qbs","ºæ","qbf","ºç","qba","ºè","qbt","ºé","qbd","ºê","qm","ºë","qmr","ºì","qms","ºí","qmf","ºî","qma","ºï","qmq",
			"ºğ","qmt","ºñ","ql","ºò","qlr","ºó","qls","ºô","qlf","ºõ","qlfa","ºö","qla","º÷","qlq","ºø","qlt","ºù","qld","ºú","qlw",
			"ºû","qlc","ºü","Qk","ºı","Qkr","ºş","Qks","»¡","Qkf","»¢","Qkfa","»£","Qka","»¤","Qkq","»¥","Qkt","»¦","QkT",
			"»§","Qkd","»¨","Qkg","»©","Qo","»ª","Qor","»«","Qos","»¬","Qof","»­","Qoa","»®","Qoq","»¯","Qot","»°","QoT","»±","Qod",
			"»²","Qi","»³","Qir","»´","Qia","»µ","Qj","»¶","Qjr","»·","Qjs","»¸","Qje","»¹","Qjf","»º","Qja","»»","Qjt","»¼","QjT",
			"»½","Qjd","»¾","Qp","»¿","Qpd","»À","Qu","»Á","Qur","»Â","Qua","»Ã","Quq","»Ä","Qut","»Å","QuT","»Æ","Qud","»Ç","Qh",
			"»È","Qhr","»É","Qhs","»Ê","Qhf","»Ë","Qha","»Ì","Qhq","»Í","Qhd","»Î","Qhl","»Ï","Qy","»Ğ","Qyd","»Ñ","Qn",
			"»Ò","Qnr","»Ó","Qns","»Ô","Qnf","»Õ","Qna","»Ö","Qnt","»×","Qnd","»Ø","Qb","»Ù","Qbd","»Ú","Qm","»Û","Qms","»Ü","Qmf",
			"»İ","Qma","»Ş","Qmq","»ß","Ql","»à","Qlr","»á","Qls","»â","Qlf","»ã","Qla","»ä","Qlq","»å","Qlt","»æ","Qld","»ç","tk",
			"»è","tkr","»é","tkrt","»ê","tks","»ë","tke","»ì","tkf","»í","tkfr","»î","tkfa","»ï","tka","»ğ","tkq","»ñ","tkt","»ò","tkT",
			"»ó","tkd","»ô","tkx","»õ","to","»ö","tor","»÷","tos","»ø","tof","»ù","toa","»ú","toq","»û","tot","»ü","toT",
			"»ı","tod","»ş","ti","¼¡","tir","¼¢","tis","¼£","tif","¼¤","tia","¼¥","tiq","¼¦","tit","¼§","tid","¼¨","tO",
			"¼©","tOs","¼ª","tOf","¼«","tOa","¼¬","tOd","¼­","tj","¼®","tjr","¼¯","tjR","¼°","tjrt","¼±","tjs","¼²","tje",
			"¼³","tjf","¼´","tjfa","¼µ","tjfq","¼¶","tja","¼·","tjq","¼¸","tjt","¼¹","tjT","¼º","tjd","¼»","tjv","¼¼","tp",
			"¼½","tpR","¼¾","tps","¼¿","tpf","¼À","tpa","¼Á","tpq","¼Â","tpt","¼Ã","tpT","¼Ä","tpd","¼Å","tu","¼Æ","tur",
			"¼Ç","tus","¼È","tuf","¼É","tua","¼Ê","tuq","¼Ë","tut","¼Ì","tuT","¼Í","tud","¼Î","tP","¼Ï","tPs","¼Ğ","tPf",
			"¼Ñ","tPd","¼Ò","th","¼Ó","thr","¼Ô","thR","¼Õ","ths","¼Ö","thf","¼×","thfa","¼Ø","tha","¼Ù","thq","¼Ú","tht",
			"¼Û","thd","¼Ü","thx","¼İ","thk","¼Ş","thkr","¼ß","thks","¼à","thkf","¼á","thkd","¼â","tho","¼ã","thos","¼ä","thof",
			"¼å","thoa","¼æ","thot","¼ç","thoT","¼è","thl","¼é","thls","¼ê","thlf","¼ë","thla","¼ì","thlq","¼í","thlt","¼î","ty",
			"¼ï","tyr","¼ğ","tys","¼ñ","tyf","¼ò","tya","¼ó","tyq","¼ô","tyt","¼õ","tyd","¼ö","tn","¼÷","tnr","¼ø","tns",
			"¼ù","tne","¼ú","tnf","¼û","tna","¼ü","tnq","¼ı","tnt","¼ş","tnd","½¡","tnc","½¢","tnx","½£","tnv","½¤","tnj",
			"½¥","tnjT","½¦","tnp","½§","tnpr","½¨","tnps","½©","tnpf","½ª","tnpa","½«","tnpd","½¬","tnl","½­","tnlr","½®","tnls",
			"½¯","tnlf","½°","tnla","½±","tnlq","½²","tnlt","½³","tnld","½´","tb","½µ","tbr","½¶","tbf","½·","tba","½¸","tbt",
			"½¹","tbd","½º","tm","½»","tmr","½¼","tms","½½","tmf","½¾","tmfr","½¿","tma","½À","tmq","½Á","tmt","½Â","tmd",
			"½Ã","tl","½Ä","tlr","½Å","tls","½Æ","tle","½Ç","tlf","½È","tlfg","½É","tla","½Ê","tlq","½Ë","tlt","½Ì","tld",
			"½Í","tlv","½Î","Tk","½Ï","Tkr","½Ğ","Tkrt","½Ñ","Tks","½Ò","Tkf","½Ó","Tka","½Ô","Tkq","½Õ","TkT","½Ö","Tkd",
			"½×","Tkg","½Ø","To","½Ù","Tor","½Ú","Tos","½Û","Tof","½Ü","Toa","½İ","Toq","½Ş","ToT","½ß","Tod","½à","Tid",
			"½á","Tj","½â","Tjr","½ã","Tjs","½ä","Tjf","½å","Tjfa","½æ","Tja","½ç","Tjq","½è","TjT","½é","Tjd","½ê","Tp",
			"›®","Tpr","½ë","Tps","½ì","Tpf","½í","TPs","½î","Th","½ï","Thr","½ğ","Ths","½ñ","The","½ò","Thf","½ó","Thfa",
			"½ô","Tha","½õ","Thq","½ö","Thd","½÷","Thk","½ø","Thkr","½ù","Thks","½ú","ThkT","½û","Tho","½ü","ThoT","½ı","Thl",
			"½ş","Thls","¾¡","Thlf","¾¢","Thla","¾£","Thlq","¾¤","Ty","¾¥","Tn","¾¦","Tnr","¾§","Tns","¾¨","Tnf","¾©","Tna",
			"¾ª","Tnq","¾«","Tnd","¾¬","Tnj","¾­","TnT","¾®","Tnp","¾¯","Tnl","¾°","Tnls","¾±","Tbd","¾²","Tm","¾³","Tmr",
			"¾´","Tms","¾µ","Tmf","¾¶","Tmfa","¾·","Tmfg","¾¸","Tma","¾¹","Tmq","¾º","Tml","¾»","Tmls","¾¼","Tmlf","¾½","Tmla",
			"¾¾","Tl","¾¿","Tlr","¾À","Tls","¾Á","Tlf","¾Â","Tla","¾Ã","Tlq","¾Ä","Tlt","¾Å","Tld","¾Æ","dk","¾Ç","dkr",
			"¾È","dks","¾É","dksw","¾Ê","dksg","¾Ë","dkf","¾Ì","dkfr","¾Í","dkfa","¾Î","dkfg","¾Ï","dka","¾Ğ","dkq","¾Ñ","dkt",
			"¾Ò","dkT","¾Ó","dkd","¾Ô","dkx","¾Õ","dkv","¾Ö","do","¾×","dor","¾Ø","dos","¾Ù","dof","¾Ú","doa","¾Û","doq",
			"¾Ü","dot","¾İ","doT","¾Ş","dod","¾ß","di","¾à","dir","¾á","dis","¾â","dif","¾ã","difq","¾ä","dia","¾å","diq",
			"¾æ","dit","¾ç","did","¾è","dix","¾é","dig","¾ê","dO","¾ë","dOs","¾ì","dOf","¾í","dOq","¾î","dj","¾ï","djr",
			"¾ğ","djs","¾ñ","djsw","¾ò","dje","¾ó","djf","¾ô","djfr","¾õ","djfa","¾ö","dja","¾÷","djq","¾ø","djqt","¾ù","djt",
			"¾ú","djT","¾û","djd","¾ü","djw","¾ı","djz","¾ş","djv","¿¡","dp","¿¢","dpr","¿£","dps","¿¤","dpf","¿¥","dpa",
			"¿¦","dpq","¿§","dpt","¿¨","dpd","¿©","du","¿ª","dur","¿«","duR","¿¬","dus","¿­","duf","¿®","dufa","¿¯","dufq",
			"¿°","dua","¿±","duq","¿²","duqt","¿³","dut","¿´","duT","¿µ","dud","¿¶","dux","¿·","duv","¿¸","dug","¿¹","dP",
			"¿º","dPs","¿»","dPf","¿¼","dPa","¿½","dPq","¿¾","dPt","¿¿","dPT","¿À","dh","¿Á","dhr","¿Â","dhs","¿Ã","dhf",
			"¿Ä","dhfr","¿Å","dhfa","¿Æ","dhft","¿Ç","dhfg","¿È","dha","¿É","dhq","¿Ê","dht","¿Ë","dhd","¿Ì","dhc","¿Í","dhk",
			"¿Î","dhkr","¿Ï","dhks","¿Ğ","dhkf","¿Ñ","dhka","¿Ò","dhkq","¿Ó","dhkt","¿Ô","dhkT","¿Õ","dhkd","¿Ö","dho","¿×","dhor",
			"¿Ø","dhos","¿Ù","dhoa","¿Ú","dhot","¿Û","dhod","¿Ü","dhl","¿İ","dhlr","¿Ş","dhls","¿ß","dhlf","¿à","dhla","¿á","dhlq",
			"¿â","dhlt","¿ã","dhld","¿ä","dy","¿å","dyr","¿æ","dys","¿ç","dyf","¿è","dya","¿é","dyq","¿ê","dyr","¿ë","dyd",
			"¿ì","dn","¿í","dnr","¿î","dns","¿ï","dnf","¿ğ","dnfr","¿ñ","dnfa","¿ò","dna","¿ó","dnq","¿ô","dnt","¿õ","dnd",
			"¿ö","dnj","¿÷","dnjr","¿ø","dnjs","¿ù","dnjf","¿ú","dnja","¿û","dnjq","¿ü","dnjT","¿ı","dnjd","¿ş","dnp","À¡","dnpr",
			"À¢","dnps","À£","dnpf","À¤","dnpa","À¥","dnpq","À¦","dnpd","À§","dnl","À¨","dnlr","À©","dnls","Àª","dnlf","À«","dnla",
			"À¬","dnlq","À­","dnlt","À®","dnld","À¯","db","À°","dbr","À±","dbs","À²","dbf","À³","dba","À´","dbq","Àµ","dbt",
			"À¶","dbd","À·","dbc","À¸","dm","À¹","dmr","Àº","dms","À»","dmf","À¼","dmfv","À½","dma","À¾","dnq","À¿","dmt",
			"ÀÀ","dmd","ÀÁ","dmw","ÀÂ","dmc","ÀÃ","dmz","ÀÄ","dmx","ÀÅ","dmv","ÀÆ","dmg","ÀÇ","dml","ÀÈ","dmls","ÀÉ","dmlf",
			"ÀÊ","dmla","ÀË","dmlt","ÀÌ","dl","ÀÍ","dlr","ÀÎ","dls","ÀÏ","dlf","ÀĞ","dlfr","ÀÑ","dlfa","ÀÒ","dlfg","ÀÓ","dla",
			"ÀÔ","dlq","ÀÕ","dlt","ÀÖ","dlT","À×","dld","ÀØ","dlw","ÀÙ","dlv","ÀÚ","wk","ÀÛ","wkr","ÀÜ","wks","Àİ","wksg",
			"ÀŞ","wke","Àß","wkf","Àà","wkfa","Àá","wka","Àâ","wkq","Àã","wkt","Àä","wkT","Àå","wkd","Àæ","wkw","Àç","wo",
			"Àè","wor","Àé","wos","Àê","wof","Àë","woa","Àì","woq","Àí","wot","Àî","woT","Àï","wod","Àğ","wi","Àñ","wir",
			"Àò","wis","Àó","wisg","Àô","wif","Àõ","wia","Àö","wid","À÷","wO","Àø","wOs","Àù","wOf","Àú","wj","Àû","wjr",
			"Àü","wjs","Àı","wjf","Àş","wjfa","Á¡","wja","Á¢","wjq","Á£","wjt","Á¤","wjd","Á¥","wjw","Á¦","wp","Á§","wpr",
			"Á¨","wps","Á©","wpf","Áª","wpa","Á«","wpq","Á¬","wpt","Á­","wpd","Á®","wu","Á¯","wus","Á°","wuf","Á±","wua",
			"Á²","wuq","Á³","wuT","Á´","wud","Áµ","wP","Á¶","wh","Á·","whr","Á¸","whs","Á¹","whf","Áº","whfa","Á»","wha",
			"Á¼","whq","Á½","wht","Á¾","whd","Á¿","whw","ÁÀ","whc","ÁÁ","whg","ÁÂ","whk","ÁÃ","whkr","ÁÄ","whkf","ÁÅ","whkq",
			"ÁÆ","whkt","ÁÇ","whkd","ÁÈ","who","ÁÉ","whoT","ÁÊ","whod","ÁË","whl","ÁÌ","whls","ÁÍ","whlf","ÁÎ","whla","ÁÏ","whlq",
			"ÁĞ","whlt","ÁÑ","whld","ÁÒ","wy","ÁÓ","wyr","ÁÔ","wys","ÁÕ","wyd","ÁÖ","wn","Á×","wnr","ÁØ","wns","ÁÙ","wnf",
			"ÁÚ","wnfr","ÁÛ","wnfa","ÁÜ","wna","Áİ","wnq","ÁŞ","wnt","Áß","wnd","Áà","wnj","Áá","wnjT","Áâ","wnp","Áã","wnl",
			"Áä","wnlr","Áå","wnls","Áæ","wnlf","Áç","wnla","Áè","wnlq","Áé","wnlt","Áê","wb","Áë","wbs","Áì","wbf","Áí","wba",
			"Áî","wm","Áï","wmr","Áğ","wms","Áñ","wmf","Áò","wma","Áó","wmq","Áô","wmt","Áõ","wmd","Áö","wl","Á÷","wlr",
			"Áø","wls","Áù","wle","Áú","wlf","Áû","wlfa","Áü","wla","Áı","wlq","Áş","wlt","Â¡","wld","Â¢","wlw","Â£","wlx",
			"Â¤","wlv","Â¥","Wk","Â¦","Wkr","Â§","Wks","Â¨","Wksg","Â©","Wkf","Âª","Wkfq","Â«","Wka","Â¬","Wkq","Â­","Wkt",
			"Â®","WkT","Â¯","Wkd","Â°","Wo","Â±","Wor","Â²","Wos","Â³","Wof","Â´","Woa","Âµ","Woq","Â¶","Wot","Â·","WoT",
			"Â¸","Wod","Â¹","Wi","Âº","Wis","Â»","Wid","Â¼","Wj","Â½","Wjr","Â¾","Wjs","Â¿","Wjf","ÂÀ","Wja","ÂÁ","Wjq",
			"ÂÂ","Wjt","ÂÃ","WjT","ÂÄ","Wjd","ÂÅ","Wp","ÂÆ","Wpd","ÂÇ","Wu","ÂÈ","WuT","ÂÉ","Wh","ÂÊ","Whr","ÂË","Whs",
			"ÂÌ","Whf","ÂÍ","Wha","ÂÎ","Whq","ÂÏ","Wht","ÂĞ","Whd","ÂÑ","Whc","ÂÒ","Whk","ÂÓ","Whkr","ÂÔ","Whkf","ÂÕ","WhkT",
			"ÂÖ","Who","Â×","WhoT","ÂØ","Whl","ÂÙ","Whls","ÂÚ","Whlf","ÂÛ","Whla","ÂÜ","Whlq","Âİ","Wyd","ÂŞ","Wn","Âß","Wnr",
			"Âà","Wns","Âá","Wnf","Ââ","Wna","Âã","Wnq","Âä","Wnd","Âå","Wnj","Âæ","WnjT","Âç","Wnjd","Âè","Wnl","Âé","Wb",
			"Âê","Wm","Âë","Wma","Âì","Wmt","Âí","Wmd","Âî","Wl","Âï","Wlr","Âğ","Wls","Âñ","Wlf","Âò","Wla","Âó","Wlq",
			"Âô","Wld","Âõ","Wlw","Âö","Wlg","Â÷","ck","Âø","ckr","Âù","cks","Âú","cksg","Âû","ckf","Âü","cka","Âı","ckq",
			"Âş","ckt","Ã¡","ckT","Ã¢","ckd","Ã£","ckw","Ã¤","co","Ã¥","cor","Ã¦","cos","Ã§","cof","Ã¨","coa","Ã©","coq",
			"Ãª","cot","Ã«","coT","Ã¬","cod","Ã­","ci","Ã®","cis","Ã¯","cisg","Ã°","cif","Ã±","cia","Ã²","cid","Ã³","cj",
			"Ã´","cjr","Ãµ","cjs","Ã¶","cjf","Ã·","cja","Ã¸","cjq","Ã¹","cjt","Ãº","cjT","Ã»","cjd","Ã¼","cp","Ã½","cpr",
			"Ã¾","cps","Ã¿","cpf","ÃÀ","cpa","ÃÁ","cpq","ÃÂ","cpt","ÃÃ","cpd","ÃÄ","cu","ÃÅ","cus","ÃÆ","cuT","ÃÇ","cP",
			"ÃÈ","cPs","ÃÉ","cPd","ÃÊ","ch","ÃË","chr","ÃÌ","chs","ÃÍ","chf","ÃÎ","cha","ÃÏ","chq","ÃĞ","cht","ÃÑ","cha",
			"ÃÒ","chk","ÃÓ","chks","ÃÔ","chkf","ÃÕ","chkd","ÃÖ","chl","Ã×","chls","ÃØ","chlf","ÃÙ","chla","ÃÚ","chlq","ÃÛ","chlt",
			"ÃÜ","chld","Ãİ","cy","ÃŞ","cya","­h","cyq","Ãß","cn","Ãà","cnr","Ãá","cns","Ãâ","cnf","Ãã","cna","Ãä","cnq",
			"Ãå","cnt","Ãæ","cnd","Ãç","cnj","Ãè","cnjT","Ãé","cnp","Ãê","cnps","®X","cnpt","Ãë","cnl","Ãì","cnls","Ãí","cnlf",
			"Ãî","cnla","Ãï","cnlq","Ãğ","cnlt","Ãñ","cnld","Ãò","cb","Ãó","cbs","Ãô","cbf","Ãõ","cba","Ãö","cbd","Ã÷","cm",
			"Ãø","cmr","Ãù","cms","Ãú","cmf","Ãû","cma","Ãü","cmq","Ãı","cmt","Ãş","cmd","Ä¡","cl","Ä¢","clr","Ä£","cls",
			"Ä¤","cle","Ä¥","clf","Ä¦","clfr","Ä§","cla","Ä¨","clq","Ä©","clt","Äª","cld","Ä«","zk","Ä¬","zkr","Ä­","zks",
			"Ä®","zkf","Ä¯","zka","Ä°","zkq","Ä±","zkt","Ä²","zkd","Ä³","zo","Ä´","zor","Äµ","zos","Ä¶","zof","Ä·","zoa",
			"Ä¸","zoq","Ä¹","zot","Äº","zoT","Ä»","zod","Ä¼","zi","Ä½","zir","Ä¾","zid","Ä¿","zj","ÄÀ","zjr","ÄÁ","zjs",
			"ÄÂ","zje","ÄÃ","zjf","ÄÄ","zja","ÄÅ","zjq","ÄÆ","zjt","ÄÇ","zjT","ÄÈ","zjd","ÄÉ","zp","ÄÊ","zp","ÄË","zps",
			"ÄÌ","zpf","ÄÍ","zpa","ÄÎ","zpq","ÄÏ","zpt","ÄĞ","zpd","ÄÑ","zu","ÄÒ","zus","ÄÓ","zuf","ÄÔ","zua","ÄÕ","zuq",
			"ÄÖ","zut","Ä×","zuT","ÄØ","zud","ÄÙ","zP","ÄÚ","zh","ÄÛ","zhr","ÄÜ","zhs","Äİ","zhf","ÄŞ","zha","Äß","zhq",
			"Äà","zht","Äá","zhd","Äâ","zhk","Äã","zhkr","Ää","zhks","Äå","zhkf","Äæ","zhka","Äç","zhkd","Äè","zho","Äé","zhod",
			"Äê","zhl","Äë","zhlf","Äì","zy","Äí","zn","Äî","znr","Äï","zns","Äğ","znf","Äñ","zna","Äò","znq","Äó","znt",
			"Äô","znd","Äõ","znj","Äö","znjs","Ä÷","znjf","Äø","znjd","Äù","znp","Äú","znpd","Äû","znl","Äü","znlr","Äı","znls",
			"Äş","znlf","Å¡","znla","Å¢","znlq","Å£","znlt","Å¤","znld","Å¥","zb","Å¦","zbs","Å§","zbf","Å¨","zba","Å©","zm",
			"Åª","zmr","Å«","zms","Å¬","zmf","Å­","zma","Å®","zmq","Å¯","zmd","Å°","zl","Å±","zlr","Å²","zls","Å³","zlf",
			"Å´","zla","Åµ","zlq","Å¶","zlt","Å·","zld","Å¸","xk","Å¹","xkr","Åº","xks","Å»","xkf","Å¼","xkfr","Å½","xka",
			"Å¾","xkq","Å¿","xkt","ÅÀ","xkT","ÅÁ","xkd","ÅÂ","xo","ÅÃ","xor","ÅÄ","xos","ÅÅ","xof","ÅÆ","xoa","ÅÇ","xoq",
			"ÅÈ","xot","ÅÉ","xoT","ÅÊ","xod","ÅË","xi","ÅÌ","xid","ÅÍ","xj","ÅÎ","xjr","ÅÏ","xjs","ÅĞ","xjf","ÅÑ","xjfa",
			"ÅÒ","xja","ÅÓ","xjq","ÅÔ","xjt","ÅÕ","xjT","ÅÖ","xjd","Å×","xp","ÅØ","xpr","ÅÙ","xps","ÅÚ","xpf","ÅÛ","xpa",
			"ÅÜ","xpq","Åİ","xpt","ÅŞ","xpd","Åß","xu","Åà","xus","Åá","xuT","Åâ","xP","Åã","xPs","Åä","xh","Åå","xhr",
			"Åæ","xhs","Åç","xhf","Åè","xha","Åé","xhq","Åê","xht","Åë","xhd","Åì","xhv","Åí","xhk","Åî","xhks","Åï","xho",
			"Åğ","xhl","Åñ","xhls","Åò","xhlt","Åó","xhld","Åô","xy","Åõ","xn","Åö","xnr","Å÷","xns","Åø","xnf","Åù","xna",
			"Åú","xnq","Åû","xnt","Åü","xnd","Åı","xnj","Åş","xnjT","Æ¡","xnp","Æ¢","xnl","Æ£","xnlr","Æ¤","xnls","Æ¥","xnlf",
			"Æ¦","xnla","Æ§","xnlq","Æ¨","xnld","Æ©","xb","Æª","xbs","Æ«","xb","Æ¬","xba","Æ­","xbd","Æ®","xm","Æ¯","xmr",
			"Æ°","xms","Æ±","xme","Æ²","xmf","Æ³","xmfa","Æ´","xma","Æµ","xmq","Æ¶","xmt","Æ·","xml","Æ¸","xls","Æ¹","xmlf",
			"Æº","xmla","Æ»","xmlq","Æ¼","xl","Æ½","xlr","Æ¾","xls","Æ¿","xlf","ÆÀ","xla","ÆÁ","xlq","ÆÂ","xlt","ÆÃ","xld",
			"ÆÄ","vk","ÆÅ","vkr","ÆÆ","vkR","ÆÇ","vks","ÆÈ","vkf","ÆÉ","vkfa","ÆÊ","vka","ÆË","vkq","ÆÌ","vkt","ÆÍ","vkT",
			"ÆÎ","vkd","ÆÏ","vkx","ÆĞ","vko","ÆÑ","vor","ÆÒ","vos","ÆÓ","vof","ÆÔ","voa","ÆÕ","voq","ÆÖ","vot","Æ×","voT",
			"ÆØ","vod","ÆÙ","vi","ÆÚ","vir","ÆÛ","vj","ÆÜ","vjr","Æİ","vjs","ÆŞ","vjf","Æß","vja","Æà","vjq","Æá","vjt",
			"Æâ","vjT","Æã","vjd","Æä","vp","Æå","vpr","Ææ","vps","Æç","vpf","Æè","vpa","Æé","vpq","Æê","vpt","Æë","vpd",
			"Æì","vu","Æí","vus","Æî","vuf","Æï","vua","Æğ","vuq","Æñ","vuT","Æò","vud","Æó","vP","Æô","vPf","Æõ","vPq",
			"Æö","vPt","Æ÷","vh","Æø","vhr","Æù","vhs","Æú","vhf","Æû","vha","Æü","vhq","Æı","vht","Æş","vhd","Ç¡","vhk",
			"Ç¢","vhkd","Ç£","vhl","Ç¤","vhls","Ç¥","vy","Ç¦","vys","Ç§","vyf","Ç¨","vyq","Ç©","vyt","Çª","vn","Ç«","vnr",
			"Ç¬","vns","Ç­","vne","Ç®","vnf","Ç¯","vnfa","Ç°","vna","Ç±","vnq","Ç²","vnt","Ç³","vnd","Ç´","vnj","Çµ","vnjd",
			"Ç¶","vnl","Ç·","vnls","Ç¸","vnlf","Ç¹","vnla","Çº","vnlt","Ç»","vb","Ç¼","vbs","Ç½","vbf","Ç¾","vba","Ç¿","vbt",
			"ÇÀ","vbd","ÇÁ","vm","ÇÂ","vms","ÇÃ","vmf","ÇÄ","vma","ÇÅ","vmq","ÇÆ","vmt","ÇÇ","vl","ÇÈ","vlr","ÇÉ","vls",
			"ÇÊ","vlf","ÇË","vla","ÇÌ","vlq","ÇÍ","vlt","ÇÎ","vld","ÇÏ","gk","ÇĞ","gkr","ÇÑ","gks","ÇÒ","gkf","ÇÓ","gkfx",
			"ÇÔ","gka","ÇÕ","gkq","ÇÖ","gkt","Ç×","gkd","ÇØ","go","ÇÙ","gor","ÇÚ","gos","ÇÛ","gof","ÇÜ","goa","Çİ","goq",
			"ÇŞ","got","Çß","goT","Çà","god","Ád","gog","Çá","gi","Çâ","gid","Çã","gj","Çä","gjr","Çå","gjs","Çæ","gjf",
			"Çç","gjfa","Çè","gja","Çé","gjq","Çê","gjt","Çë","gjd","Çì","gp","Çí","gpr","Çî","gps","Çï","gpf","Çğ","gpa",
			"Çñ","gpq","Çò","gpt","Çó","gpd","Çô","gu","Çõ","gur","Çö","gus","Ç÷","guf","Çø","gua","Çù","guq","Çú","gut",
			"Çû","guT","Çü","gud","Çı","gP","Çş","gPs","È¡","gPf","È¢","gPq","È£","gh","È¤","ghr","È¥","ghs","È¦","ghf",
			"È§","ghfx","È¨","gha","È©","ghq","Èª","ght","È«","ghd","È¬","ghx","È­","ghk","È®","ghkr","È¯","ghks","È°","ghkf",
			"È±","ghkt","È²","ghkd","È³","gho","È´","ghor","Èµ","ghos","È¶","ghot","È·","ghod","È¸","ghl","È¹","ghlr","Èº","ghls",
			"È»","ghlf","È¼","ghlq","È½","ghlt","È¾","ghld","È¿","gy","ÈÀ","gys","ÈÁ","gyf","ÈÂ","gyq","ÈÃ","gyt","ÈÄ","gn",
			"ÈÅ","gnr","ÈÆ","gns","ÈÇ","gnf","ÈÈ","gnfx","ÈÉ","gna","ÈÊ","gnt","ÈË","gnd","ÈÌ","gnj","ÈÍ","gnjs","ÈÎ","gnjf",
			"ÈÏ","gnja","ÈĞ","gnjd","ÈÑ","gnp","ÈÒ","gnpr","ÈÓ","gnps","ÈÔ","gnpf","ÈÕ","gnpd","ÈÖ","gnl","È×","gnlr","ÈØ","gnls",
			"ÈÙ","gnlf","ÈÚ","gnla","ÈÛ","gnlq","ÈÜ","gnlt","Èİ","gnld","ÈŞ","gb","Èß","gbr","Èà","gbs","Èá","gbf","Èâ","gba",
			"Èã","gbt","Èä","gbd","Èå","gm","Èæ","gmr","Èç","gms","Èè","gmsg","Èé","gme","Èê","gmf","Èë","gmfr","Èì","gma",
			"Èí","gmq","Èî","gmt","Èï","gmd","Èğ","gmx","Èñ","gml","Èò","gmls","Èó","gmlf","Èô","gmla","Èõ","gmlq","Èö","gmld",
			"È÷","gl","Èø","glr","Èù","gls","Èú","glf","Èû","gla","Èü","glq","Èı","glt","Èş","gld"};

#define STR_MAX	80
#define HAN_MAX 2364
#define	MOUM	19
char jamo[34]="rRseEfaqQtTdwWczxvgkoiOjpuPhynbml";
char* han_jamo[33]={"¤¡","¤¢","¤¤","¤§","¤¨","¤©","¤±","¤²","¤³","¤µ","¤¶","¤·","¤¸","¤¹","¤º","¤»","¤¼","¤½","¤¾",
					"¤¿","¤À","¤Á","¤Â","¤Ã","¤Ä","¤Å","¤Æ","¤Ç","¤Ë","¤Ì","¤Ğ","¤Ñ","¤Ó"};

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
	//°Ë»ö ¼Óµµ Çâ»óÀ» À§ÇÑ »¡¸®°¡±â
	int mp=0;
	char* check_text[19]={"°¡","±î","³ª","´Ù","µû","¶ó","¸¶","¹Ù","ºü","»ç","½Î","¾Æ","ÀÚ","Â¥","Â÷","Ä«","Å¸","ÆÄ","ÇÏ"};
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

	//ÀÔ·Â ¹Ş±â
	strcpy(han, "");	

	//¸ğÀ½, ÀÚÀ½ ÇÑ±Û ¾Æ´Ô ¿©ºÎ È®ÀÎ(ÀÚÀ½ ÀÎ ºÎºĞÀ» Æ÷ÀÎÆ®·Î µÎ¾î¼­ º¯È¯¿¡ µµ¿òÀÌ µÇ°Ô ÇÔ)
	for(unsigned int i=1; i<strlen(text); i++)
	{
		//ÇÑ±Û¿¡ ´ëÄ¡ ¾È µÇ´Â ´ë¹®ÀÚ¸¦ ¼Ò¹®ÀÚ·Î ¹Ù²Ş
		if(text[i]>='A' && text[i]<='Z' && text[i]!='Q' && text[i]!='W' && text[i]!='E'	&& text[i]!='R'
			&& text[i]!='T' && text[i]!='O' && text[i]!='P')text[i]+=0x20;

		//ÀÚÀ½ÀÎÁö ¸ğÀ½ÀÎÁö ±¸ºĞ
		for(int j=0; j<33; j++)
			if(text[i]==jamo[j])
			{
				if(j>=MOUM)
				{
					begin[pp]=i-1;
					if(pp>0)end[pp-1]=i-2;
					pp++;
					i++;	//ÀÌÁß ¸ğÀ½ÀÇ °æ¿ì¸¦ »ı°¢ÇØ¼­ ´ÙÀ½À» ³Ñ±è(ÀÌÁß ¸ğÀ½À» ²÷¾î ÀĞ¾î ±ÛÀÚ°¡ µÇ´Â °æ¿ì´Â ¾øÀ¸¹Ç·Î ¿¹: µŞ»ê(O) µÎ¤Ó¤µ»ê(X))
				}
				break;
			}
	}
	if(pp>0)end[pp-1]=strlen(text)-1;

	//Æ÷ÀÎÆ® Á¶Á¤(ÇÑ±ÛÀÎ ºÎºĞ¸¸ ÇØ¼®)
	for(int i=0; i<pp; i++)
	{
		char buffer[10];
		int bp=0;
		bool head=true;	//¾Õ ºÎºĞÀÎÁö ±ÛÀÚ µŞºÎºĞÀÎÁö ¾Ë¾Æ³¿(Àß¶ó³»¾ß ÇÏ´Â ºÎºĞ ¿¹: 1@°¡2$ -> 1@Àº head=trueÀÎ »óÅÂ, 2$´Â head=falseÀÎ »óÅÂ¿¡¼­ ÀĞÀ½
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

		//¿µ¹®À» ÇÑ±Û·Î ÃÆÀ» ¶§ ÀÏÄ¡ÇÏ´Â ±ÛÀÚ Ã£±â
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

			//Ã£Áö ¸ø ÇßÀ» ¶§ ¿µ¿ªÀ» ÁÙ¿©¼­ Ã£¾Æº½
			if(!find)
			{
				buffer[strlen(buffer)-1]=NULL;
				end[i]--;
			}
			else break;
		}

		//ÀĞÁö ¸øÇÏ´Â ÇÑ±ÛÀÓÀ» Ç¥½Ã
		if(strlen(buffer)<=1)begin[i]=9999;
	}

	//º¯È¯
	unsigned int pp2=0;
	for(int i=0; i<strlen(text); i++)
	{
		while(begin[pp2]==9999)pp2++;

		//ÇÑ±ÛÀÎ ºÎºĞ
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
		//¾Æ´Ñ ºÎºĞ
		else
		{			
			//ÇÑ±Û ÀÏ¶§´Â ²÷¾îÁø ¸ğÀ½ ÀÚÀ½À¸·Î Ç¥Çö
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
			//°á±¹ ¾øÀ» ¶© ¹®ÀÚ ±×´ë·Î
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
//´ëÈ­ Ã³¸® Å¬·¡½º
class CDlg
{
protected:
	//¼±ÅÃÁö¸¦ À§ÇÑ º¯¼ö
	char* select_point[300];
	bool row;
	int n_of_e;
	int x, y, gap, line;
	int col, xmax, ymax, width;
	char dlg_buffer[1024];
	int select;

	//´ë»ç Ãâ·ÂÀ» À§ÇÑ º¯¼ö
	int tp, sp;
	bool ani_end;
	int length;
	RECT text, shadow;

	char han_name[161];	

	//´ëÈ­Ã¢ º¸ÀÌ±â
	void ShowDlg();

public:
	//´ëÈ­Ã¢ ¸ğ¾ç¸¸ Ãâ·Â
	void ShowDlg(int vx, int vy, int vwidth, int vline);

	//´ëÈ­ °ü·Ã ¸Ş¼Òµå
	void MakeSelection(char* input_dlg, int vx, int vy, int vgap, int vline, int start=0);
	int Selecting();
	int Select(char* input_dlg, int vx, int vy, int vgap, int vline, int vstart=0, bool cancel=false);
	void MakeText(char* content, int vx, int vy, int vwidth, int vline);
	void Print(char* content, int vx, int vy, int vwidth, int vline);
	int Printing();
	char* InputHan(int vx, int vy, int str_max);
};

#define SNR_START 4

//´ëÈ­ ¸¸µé±â
void CDlg::MakeText(char* content, int vx, int vy, int vwidth, int vline)
{
	x=vx; y=vy; width=Max(3,vwidth); line=vline;

	length=strlen(content);
	strcpy(dlg_buffer,content);
	ani_end=false;
	row=false;

	//´ëÈ­¸¦ Ãâ·ÂÇÏ±â ¾Ë¸Â°Ô Ã³¸®
	sp=SNR_START;	//Ã³¸®µÇ°í ÀÖ´Â Æ÷ÀÎÆ®
	int space=-1;	//ÀÌÀü±îÁö Ã³¸®ÇÑ Æ÷ÀÎÆ®, space°¡ ÀÖ´ø °÷
	int senlen=0;	//¹®ÀåÀÇ ±æÀÌ, ÇÑ±Û Æ÷ÀÎÆ®
	
	while(sp<length)
	{
		if(dlg_buffer[sp]&0x80)//ÇÑ±Û
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
			if(space<0)	//½ºÆäÀÌ½º°¡ ÇÏ³ªµµ ¾øÀ»¶§ °­Á¦ ÁÙ º¯È¯
			{
				//»çÀÌ¿¡ CRÀ» ³Ö¾îÁÜ
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
	n_of_e=0;		//ÁÙ¹Ù²Ş È½¼ö
	tp=0;			//Ãâ·ÂÇÏ´Â ´Ü¾î
	//´ëÈ­Ã¢À» À§ÇÑ °è»ê
	xmax=x+width*10;
	ymax=y+line*20;
}

//´ëÈ­ Ãâ·Â(Á¤ÁöµÈ »óÅÂ¿¡¼­)
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

//´ëÈ­Ãâ·Â(½Ç½Ã°£À¸·Î)
int CDlg::Printing()
{
	static char text_buffer[1024];

	if(!ani_end)
	{
		if(sp<length && n_of_e<line)
		{
			if(dlg_buffer[sp]=='\n')n_of_e++;
			text_buffer[tp]=dlg_buffer[sp];
			if(dlg_buffer[sp]&0x80)	//ÇÑ±ÛÀÏ °æ¿ì 2Ä­ ÀüÁø
			{
				sp++; tp++;
				text_buffer[tp]=dlg_buffer[sp];
			}
			text_buffer[tp+1]=NULL;
			//Æ÷ÀÎÅÍ Áõ°¡
			sp++; tp++;
		}
		else ani_end=true;
	}
	else
	{
		//´ÙÀ½À¸·Î
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
	//´ëÈ­ Ãâ·Â
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

	//°¡·Î¼±ÅÃÁöÀÎÁö ¼¼·Î ¼±ÅÃÁöÀÎÁö È®ÀÎ (¸¶ÀÌ³Ê½º´Â °¡·Î ¼±ÅÃÁö)
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
		

	//´ëÈ­¸¦ Ãâ·ÂÇÏ±â ¾Ë¸Â°Ô Ã³¸®
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
	//½ºÅ©¸³Æ®¿¡¼­ ÀĞ¾î ¿Â ¹®ÀåÀÇ ³¡¿¡ À½Ç¥°¡ »ı±â´Â °Í ¹æÁö
	if(dlg_buffer[i-1]==13)
	{
		dlg_buffer[i-1]=NULL;
	}

	//´ëÈ­Ã¢ Ç¥½Ã¿¡ ÇÊ¿äÇÑ °Íµé °è»ê
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
	//´ëÈ­»óÀÚ&¼±ÅÃ¹Ù
	int sx, sy;
	if(row)
	{
		//À§ ¾Æ·¡ ÀÌµ¿
		if(GetKey(2,10))select=Min(n_of_e,select+1);
			else if(GetKey(1,10))select=Max(0,select-1);
		//ÁÂ¿ì ÀÌµ¿
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
		//À§ ¾Æ·¡ ÀÌµ¿
		if(GetKey(3,10))select=Min(n_of_e,select+1);
			else if(GetKey(0,10))select=Max(0,select-1);

		//ÁÂ¿ì ÀÌµ¿
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

	//¼±ÅÃÁöµé
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
	//Àß¸øµÈ °ª º¸Á¤(ÆíÀÇ»ó º¯¼ö¸¦ Àü¿ªÀ¸·Î ÇØ¼­ °íÄ¡±â ³­°¨ÇÏ¹Ç·Î ÀÌ·¸°Ô µğ¹ö±× ÀåÄ¡¸¦ ³Ö¾îµÒ)
	width=MaxMin(width, 3, 100);
	col=MaxMin(col, 1, 30);
	line=MaxMin(line, 1, 30);

	//À§
	jdd->DrawPicture(backbuffer,"Dlg1",x-5,y-5,NULL);
	for(int i=0; i<width; i++)jdd->DrawPicture(backbuffer,"Dlg2",x+(i*10),y-5,NULL);
	jdd->DrawPicture(backbuffer,"Dlg3",xmax,y-5,NULL);
	//Áß°£
	int ysize=(row)?col:line;

	for(int i=0; i<ysize; i++)
	{
		int tmp_y=y+i*20;
		jdd->DrawPicture(backbuffer,"Dlg4",x-5,tmp_y,NULL);
		for(int j=0; j<width; j++)jdd->DrawPicture(backbuffer,"Dlg5",x+(j*10),tmp_y,NULL);
		jdd->DrawPicture(backbuffer,"Dlg4",xmax,tmp_y,NULL);
	}
	//¾Æ·¡
	jdd->DrawPicture(backbuffer,"Dlg7",x-5,ymax,NULL);
	for(int i=0; i<width; i++)jdd->DrawPicture(backbuffer,"Dlg2",x+(i*10),ymax,NULL);
	jdd->DrawPicture(backbuffer,"Dlg9",xmax,ymax,NULL);
}

//ÇÑ±ÛÀÔ·Â
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

		//ÀÔ·ÂµÈ °Í Ç¥½Ã
		Process();
		Screen();
		ShowDlg(vx, vy, str_max*11/10, 1);
		jdd->DrawText(backbuffer,han_name,font20,x+1,y+1,JColor(0,0,0));
		jdd->DrawText(backbuffer,han_name,font20,x,y,JColor(255,255,255));

		int eng_len=(int)strlen(eng_buffer);
		//ÀÔ·Â
		if(eng_len<80)
		{
			//¹®ÀÚÅ°
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
			//½ºÆäÀÌ½º
			else if(GetKey(6))
			{
				strcat(eng_buffer, " ");
			}
			//¼ıÀÚ, Æ¯¼ö ¹®ÀÚ
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
		//Æ¯¼ö Å°
		if(GetKey(4))break;
		else if(GetKey(58, 10))
		{
			if(eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
		}

		//ÀÔ·Â Á¤º¸ °»½Å
		if(old_len!=eng_len)
		{
			strcpy(han_name, m_han.EngToHan(eng_buffer));
			//ÀÔ·Â ÇÑ°è
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
//´ëÈ­ Ã³¸® Å¬·¡½º+½ºÅ©¸³Æ® ÀĞ±â ±â´É
class CHugeDlg:public CDlg
{
	int snrs;			//½Ã³ª¸®¿À ÇÑ°è
	char** snr;			//½Ã³ª¸®¿À Æ÷ÀÎÅÍ
	int mark_no;		//Ã¥°¥ÇÇ ¼ıÀÚ
	int bookmark[1000];	//Ã¥°¥ÇÇ

	int process_snr;	//Ã³¸® ÁßÀÎ ½Ã³ª¸®¿À
	int answer;			//Ã³¸® Áß ¾ò´Â °á°ú°ª

	bool IsBookMark(int no);
	void Function(int command);

public:
	void Script(int script_no);

	CHugeDlg(char* dialog_file);
	~CHugeDlg();
};

#define KEY1 8
#define KEY2 4

//´ëÈ­ ÆÄÀÏ ºÒ·¯¿À±â
CHugeDlg::CHugeDlg(char* dialog_file)
{	
	if(fp=fopen(dialog_file,"rb"))
	{
		//´ë»ç ¼ö ÀĞ±â
		fread(&snrs,sizeof(int),1,fp);
		//Ã¥°¥ÇÇ ÀĞ±â
		fread(&mark_no,sizeof(int),1,fp);
		fread(&bookmark,sizeof(int),mark_no,fp);
		//´ë»ç³»¿ë ÀĞ±â
		snr=new char*[snrs];
		for(int snr_no=0; snr_no<snrs; snr_no++)
		{			
			//ÆÄÀÏ¿¡¼­ ÀĞ±â
			int text_size;
			fread(&text_size,sizeof(int),1,fp);
			snr[snr_no]=new char[text_size/2+1];

			char* buffer=new char[text_size+1];
			fread(buffer,text_size,1,fp);
			buffer[text_size]=NULL;

			//ÇØµ¶
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

//´ëÈ­³»¿ë ºñ¿ì±â
CHugeDlg::~CHugeDlg()
{
	for(int i=0; i<snrs; i++)
		if(snr[i]!=NULL)
		{
			delete[] snr[i];
			snr[i]=NULL;
		}
}

//½ºÅ©¸³Æ® Ã³¸®
void CHugeDlg::Script(int script_no)
{
	int command;
	char buffer[5];
	script_no=bookmark[script_no];

	do{
		//¸í·ÉÀ» ÀĞ¾î µéÀÌ±â
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
//¸í·É Ã³¸® Å¬·¡½º
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

CCommand::CCommand(char* vsource[], int max)	//ÃÊ±âÈ­
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

void CCommand::AddCom(int id)	//»ç¿ëÇÒ ¼ö ÀÖ´Â ¸í·É Ãß°¡
{
	if(count>=command_max)return;	//¸í·É¾î°¡ ²Ë Âü
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,source[id]);
	com_id[count]=id;
	count++;
}

void CCommand::AddComStr(int id, char* str)
{
	if(count>=command_max)return;	//¸í·É¾î°¡ ²Ë Âü
	
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
	if(count<1)return -1;		//»ç¿ë °¡´É ¸í·ÉÀÌ ¾øÀ¸¸é ½ÇÇàÇÏÁö ¾ÊÀ½

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
//ÆÄÀÏ Ã³¸® Å¬·¡½º
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
//¾Ö´Ï¸ŞÀÌ¼Ç Å¬·¡½º
#define LOOP_TYPE_ANI		1
#define EXCHANGE_TYPE_ANI	2

class CAnimation
{
private:
	int x_size, y_size;
	bool walk;							//º¸Çà ¿©ºÎ(±âº»°ª: false)
	int top, bottom, left, right;		//º¸Çà ±×·¡ÇÈÀ» À§ÇÑ »óÇÏÁÂ¿ì ¾Ö´Ï¸ŞÀÌ¼Ç ±×¸²ÀÇ À§Ä¡(YÁÂÇ¥¸¦ Áõ°¡)
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

//CAnimation ¸Ş¼Òµå
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
	if(type==1)	//ÀÏÁ÷¼± ¹æÇâ ¾Ö´Ï¸ŞÀÌ¼Ç
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
	else if(type==2)	//¹İº¹ ¾Ö´Ï¸ŞÀÌ¼Ç
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
//Å¥ Å¬·¡½º
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
	//»ı¼º, ¼Ò¸ê, »ğÀÔ, »èÁ¦
	void Init();
	void Destroy();
	void Insert(unsigned char command, char* content, unsigned char values, ...);
	bool Delete(int id);

	//°ª ¾ò±â
	int GetMax();
	QueData* GetQue(int id);
};

void Que::Init()
{
	//Å¥ °ø°£ È®º¸
	que=new QueData[QUE_MAX];

	//µ¥ÀÌÅÍ ÃÊ±âÈ­
	for(int i=0; i<QUE_MAX; i++)
	{
		que[i].command=-1;
		que[i].value=NULL;
		que[i].content=NULL;
	}

	//ÇÑ°è ÃÊ±âÈ­
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

	//ºó Å¥¿¡ »ğÀÔ
	for(int i=0; i<que_max; i++)
	{
		if(que[i].command<0)
		{
			id=i;
			empty=true;			
			break;
		}
	}

	//»õ·Î¿î ÀÚ¸® ¸¸µé±â
	if(!empty && que_max<QUE_MAX-1)
	{
		id=que_max;
		que_max++;
	}

	//¸í·É
	que[id].command=command;

	//¹®ÀÚ¿­
	if(content!=NULL)
	{
		que[id].content=new char[strlen(content)+1];
		strcpy(que[id].content, content);
	}

	//¼ıÀÚ¿­
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