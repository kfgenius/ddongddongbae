#pragma strict

var text : GUIText;
var dialogBox : GUITexture;
var bgm : AudioClip;

var body : Transform;
var black : GUITexture;

private var delay : int;
private var dd : int;
private var dlg_no = 0;

#if def KOREAN

private var dlg = [
	"FadeIn",
	"<쉣빠딱>\n... 아무도 없나...",
	"<???>\n역시 용감하군\n내 학생다워.",
	"<쉣빠딱>\n마즈키 선생님...",
	"In",
	"BgmOn",
	"<마즈키>\n크크큭...\n내가 귀엽게 키운 미소녀 제자 2명을 없애다니...",
	"<마즈키>\n네 녀석의 피는 무슨 색이냐!",
	"<쉣빠딱>\n빨간색.",
	"<마즈키>\n정상이군.",
	"<마즈키>\n어쨌든...",
	"<마즈키>\n난 네가 싫지 않아.\n마지막 기회를 주지.",
	"<마즈키>\n나의 애완동물이 되면 살려주마.",
	"<쉣빠딱>\n당신의 애완동물이 될 바에는\n배부른 소크라테스가 되겠소!",
	"<마즈키>\n흑흑흑... 내가 널 잘못 가르쳤구나.",
	"<마즈키>\n그럼 어쩔 수 없지.",
	"<마즈키>\n널 사회에서 매장하는 게 나의 의무.",
	"<마즈키>\n잘 봐둬라. 나의 스텝은 모든 공격을 흘려보낸다!"
];

#else

private var dlg = [
	"FadeIn",
	"<SPT>\n...\nIs anybody there?",
	"<???>\nYou're so brave. My true pupil.",
	"<SPT>\nTeacher Mazuki...",
	"In",
	"BgmOn",
	"<Mazuki>\nKukuku...\nYou eliminated two of my dear rosebud pupils...",
	"<Mazuki>\nWhat color is your blood?",
	"<SPT>\nRed.",
	"<Mazuki>\nNormal.",
	"<Mazuki>\nAnyway...",
	"<Mazuki>\nI don't have a bad feeling on you.\nHere's your last chance.",
	"<Mazuki>\nYou may survive if you agree to be my pet.",
	"<SPT>\nI would rather be a Socrates satisfied than a pet of yours!",
	"<Mazuki>\nOh... my lessons ware so in vain.",
	"<Mazuki>\nNow there's no way left.",
	"<Mazuki>\nIt's my obligation to put you out of this society!",
	"<Mazuki>\nHave a look on my step,\nwhich slips through every attacks on me!"
];

#endif

function Start () {
	text.material.color = Color.black;
	text.fontSize = Screen.width  / 26;
	//yaohi.renderer.material.mainTexture.color = Color.black;

	delay = 0;
	dd = 0;
}

function Update ()
{
	if(dlg_no >= dlg.GetLength(0))return;

	dd++;
	
	if(dd > 3)
	{
		delay++;
		dd = 0;
	}

	if(Input.GetButtonDown("Jump") || Input.GetButtonDown("Fire1"))
	{
		if(delay >= dlg[dlg_no].Length)
		{
			delay = 0;
			dd = 0;
			dlg_no++;
			
			if(dlg_no >= dlg.GetLength(0))
			{
				Application.LoadLevel("vs_mazuki");
			}
		}
		else
		{
			delay = dlg[dlg_no].Length;
		}
	}
}

function Fade (start : float, end : float, length : float, currentObject : Transform)
{
	for(var i = 0.0; i < 1.0; i += Time.deltaTime*(1/length))
	{
		currentObject.position.y = Mathf.Lerp(start, end, i);
		yield;
		currentObject.position.y = end;
	}
}

function FadeO (start : float, end : float, length : float, currentObject : GUITexture)
{
	for(var i = 0.0; i < 1.0; i += Time.deltaTime*(1/length))
	{
		currentObject.color.a = Mathf.Lerp(start, end, i);
		yield;
		currentObject.color.a = end;
	}
}

function OnGUI()
{
	text.text = "";

	if(dlg_no < dlg.GetLength(0))
	{
		if(dlg[dlg_no] == "FadeIn")
		{
			FadeO(1.0f, 0.0f, 1.0f, black);
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "In")
		{
			Fade(-4.0f, 0.0f, 1.0f, body);
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "BgmOn")
		{
			AudioSource.PlayClipAtPoint(bgm, Vector3(0, 0, 0));		
			dlg_no++;
			return;		
		}
	
		var txt_end = Mathf.Min(dlg[dlg_no].Length, delay);
		text.text = dlg[dlg_no].Substring(0, txt_end);
	}
}