#pragma strict

var text : GUIText;
var dialogBox : GUITexture;
var bgm : AudioClip;

var yaohi : Transform;
var black : GUITexture;

private var delay : int;
private var dd : int;
private var dlg_no = 0;


private var dlg = [
	"FadeIn",
	"<쉣빠딱>\n헉헉... 늦진 않았겠지...",
	"<쉣빠딱>\n어라? 하늘이 완전 컴컴한데?\n조금... 늦은게... 아닌 듯?",
	"<???>\n대단하군",
	"<쉣빠딱>\n누구냐!",
	"In",
	"BgmOn",
	"<야오희>\n이 시간에 등교하는 건 나 다음으로 네가 처음이다.",
	"이 녀석은 우리 학교 캡짱, 야오희.\n학교에 잘 안 나온다 싶더니 이 시간에 등교하고 있었다.",
	"<야오희>\n반했어!",
	"<쉣빠딱>\n끄아아악!",
	"<야오희>\n나랑 달밤의 고속도로를 드라이브하지 않겠나?",
	"<쉣빠딱>\n니 얼굴은 고속도로에 갈았니?",
	"<야오희>\n크크큭... 배짱좋군. 제군.",
	"<야오희>\n그 도전 받아주지!",
	"<야오희>\n하지만 넌 나에게 상처 하나 입히지 못 할 것이다!",
	"<야오희>\n철괴!!!"
];

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
				Application.LoadLevel("vs_yaohi");
			}
		}
		else
		{
			delay = dlg[dlg_no].Length;
		}
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

function Fade (start : float, end : float, length : float, currentObject : Transform)
{
	for(var i = 0.0; i < 1.0; i += Time.deltaTime*(1/length))
	{
		currentObject.position.x = Mathf.Lerp(start, end, i);
		yield;
		currentObject.position.x = end;
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
			Fade(4.0f, 0.0f, 1.0f, yaohi);
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