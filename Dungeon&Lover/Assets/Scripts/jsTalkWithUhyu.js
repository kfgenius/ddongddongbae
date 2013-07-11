#pragma strict

var text : GUIText;
var dialogBox : GUITexture;
var bgm : AudioClip;

var body : Transform;

private var delay : int;
private var dd : int;
private var dlg_no = 0;

#if def KOREAN

private var dlg = [
	"BgmOn",
	"<쉣빠딱>\n크헉, 이게 도대체...?",
	"<???>\n오랫만이야... 쉣빠딱",
	"<쉣빠딱>\n이 목소리는!!",
	"<쉣빠딱>\n이럴 수가... 넌 분명 죽었을 텐데?",
	"FadeIn",
	"<쉣빠딱>\n어휴!",
	"<어휴>\n크크크... 미안하지만 이렇게 멀쩡히 살아있지.",
	"<쉣빠딱>\n그렇다면 우리 동네 미소녀가\n이렇게 된 것이 다 너의 주술?",
	"<어휴>\n아니, 핵이잖여. 은근슬쩍 설정 바꾸지 마.",
	"<쉣빠딱>\n하긴 중간에 설정 바뀌는 만화 좀 짜증나지.",
	"<어휴>\n어쨌든 난 너에게 복수하기 위해\n인간을 버리고 이렇게 다시 태어났다.",
	"<쉣빠딱>\n넌 처음부터 인간이 아니었어...\n그건 그렇치고 원한이라니...",
	"<어휴>\n......",
	"<어휴>\n까먹었다.",
	"<어휴>\n하지만 내 속에서 끓어오르는 분노는 그치지 않아!",
	"<어휴>\n목말라~ 타오른다! 괴롭다! 으어어어...",
	"<쉣빠딱>\n어디서 본 건 있어가지고 흉내내긴.",
	"<쉣빠딱>\n나도 피하지 않겠어!",
	"<어휴>\n와라! 쉣빠딱!\n하늘로 가자꾸나!"
];

#else

private var dlg = [
	"BgmOn",
	"<SPT>\nArgh, What the...?",
	"<???>\nIt has been a long time... Shit-pa-tack.",
	"<SPT>\nThe voice!",
	"<SPT>\nOh my, you must be dead-",
	"FadeIn",
	"<SPT>\nGosh!",
	"<Gosh>\nNow I feel sorry to be alive, huh.",
	"<SPT>\nSo you chanted every rosebuds in this town\nto this horrible fate?",
	"<Gosh>\nNo, never, an atomic bomb, you know.\nNo changes on settings.",
	"<SPT>\nWell, comic books which changes background sets sucks.",
	"<Gosh>\nWhatever.\nI forefeited my humanity to have revenge on you!",
	"<SPT>\nYou were not one of mankind at first...\nwhat the hell is your resentment?",
	"<Gosh>\n......",
	"<Gosh>\nI forgot.",
	"<Gosh>\nBut my anger never cease!",
	"<Gosh>\nThirsty.. I'm burning! agony! Arghhhhh....",
	"<SPT>\nYour mimicking makes me sick...",
	"<SPT>\nAll right, come on!",
	"<Gosh>\nCome, Shit-pa-tack! to hell!"
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
				Application.LoadLevel("vs_uhyu");
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

function OnGUI()
{
	text.text = "";

	if(dlg_no < dlg.GetLength(0))
	{
		if(dlg[dlg_no] == "FadeIn")
		{
			Fade(5.0f, 0.0f, 1.0f, body);
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