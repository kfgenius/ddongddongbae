#pragma strict

var animator : Animator;
var text : GUIText;
var dialogBox : GUITexture;
var effect : AudioClip;
var black : GUITexture;

private var delay : int;
private var dd : int;
private var dlg_no = 0;

private var dlg = [
	"<사오리>\n이럴 수가! 내가 이런 놈따위에게...",
	"<쉣빠딱>\n잘가라...",
	"<사오리>\n크크큭, 안심하긴 일러...",
	"<사오리>\n난 우리 동네 3대 미소녀 허접 찌끄레기일 뿐!\n아직 2명의 미소녀가 남아있다!",
	"<쉣빠딱>\n이럴수가! 그들이 아직 살아있단 말인가!",
	"Die",
	"<사오리>\n크하하하하!\n너의 지옥이 눈에 선하구나!",
	"FadeOut",
	"<쉣빠딱>\n...",
	"<쉣빠딱>\n학교나 가자."
];

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
				Application.LoadLevel("talk_yaohi");
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
		if(dlg[dlg_no] == "Die")
		{
			AudioSource.PlayClipAtPoint(effect, Vector3(0, 0, 0));
			animator.SetBool("life", false);	
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "FadeOut")
		{
			Fade(0.0f, 1.0f, 1.0, black);
			dlg_no++;
			return;		
		}
	
		var txt_end = Mathf.Min(dlg[dlg_no].Length, delay);
		text.text = dlg[dlg_no].Substring(0, txt_end);
	}
}