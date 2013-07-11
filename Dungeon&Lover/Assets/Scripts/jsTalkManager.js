#pragma strict

var text : GUIText;
var dialogBox : GUITexture;
var black : GUITexture;
var bgm : AudioClip;

private var delay : int;
private var dd : int;
private var dlg_no = 0;

#if def KOREAN

private var dlg = [
	"으음...",
	"꽤나 오래 잤던 것 같다.",
	"헐... 얼마나 오래 잤는지 내 이름도 가물가물하다.",
	"내 이름이 뭐였더라?",
	"아아, 그래. 난 쉣빠딱...",
	"<???>\n쉣빠딱~~~!! 아직 안 일어 났어?",
	"오 쉣!\n저 끔찍한 목소리는 지긋지긋한 소꿉친구 사오리다.\n죽기 전에 빨리 내려가야 겠다.",
	"FadeIn",
	"BgmOn",
	"<쉣빠딱>\n미안... 늦어서.",
	"<사오리>\n오늘도 또 늦겠잖아. 내가 못 살아.",
	"이 괴물의 이름은 사오리.\n나의 소꿉친구였다.\n놀라운 것은 인간이라는 점이다.",
	"사실 우리 동네 애들이 처음부터 이렇진 않았는데\n어느날 마을에 핵이 떨어진 이후 이렇게 되었다.",
	"<사오리>\n정말이지... 매번 늦잠자고...\n너 때문에 창피해 죽겠어.",
	"<쉣빠딱>\n난 니 얼굴이 창피하다.",
	"<사오리>\n아하하핫!\n지금 농담 너무 웃겼어.",
	"<사오리>\n하지만 말야.",
	"<사오리>\n난 농담을 아주 싫어해!"
];

#else

private var dlg = [
	"Mmm...",
	"I've slept for quite a while.",
	"Oops... The sleep was so long.\nI can hardly remember my own name.",
	"My name, what was it?",
	"Aww... Oh, I see, I'm Shit-pa-tack.",
	"<???>\nShit-pa-tak~~~!!\nStill sleeping?",
	"Oh, Shit!\nThat nightmare of a voice must be that of Saori,\nmy wearing friend from childhood.",
	"Got to hurry before I get killed.",
	"FadeIn",
	"BgmOn",
	"<SPT>\nI'm so sorry... to be late.",
	"<Saori>\nWe are running late again.\nYou're killing me.",
	"This monster is named Saori.\nMy childhood friend.\nShe's a human, to your surprise.",
	"Everyone was not like this mess,\nbut an atomic bomb changed everything.",
	"<Saori>\nMy gosh, always oversleeping...\nShame on you.",
	"<SPT>\nI feel shame on your face.",
	"<Saori>\nAhahaha! What a joke. Nice try.",
	"<Saori>\nBut...",
	"<Saori>\nI don't like joke. Seriously."
];

#endif

function Start () {
	text.material.color = Color.black;
	text.fontSize = Screen.width  / 26;

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
				Application.LoadLevel("vs_saori");
			}
		}
		else
		{
			delay = dlg[dlg_no].Length;
		}
	}
}

function Fade (start : float, end : float, length : float, currentObject : GUITexture)
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
			Fade(1.0f, 0.0f, 1.0, black);
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "BgmOn")
		{
			AudioSource.PlayClipAtPoint(bgm, Vector3(0, 3.3, -2));		
			dlg_no++;
			return;		
		}
	
		var txt_end = Mathf.Min(dlg[dlg_no].Length, delay);
		text.text = dlg[dlg_no].Substring(0, txt_end);
	}
}