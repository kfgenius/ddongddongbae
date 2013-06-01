#pragma strict

var animator : Animator;
var text : GUIText;
var dialogBox : GUITexture;
var effect : AudioClip;

var yaohi : Transform;
var black : GUITexture;

private var delay : int;
private var dd : int;
private var dlg_no = 0;


private var dlg = [
	"<야오희>\n크흑... 후후후...",
	"<야오희>\n나의 전신전령, 명실상부한 패배다!!",
	"<쉣빠딱>\n야오희...",
	"<야오희>\n후후...\n세상에는 손에 넣지 못 하기 때문에 아름다운 것도 있지.",
	"<야오희>\n적어도 마지막은 이 남자 품에서...",
	"<쉣빠딱>\n거절한다!",
	"Die",
	"<야오희>\n끼에에~",
	"<쉣빠딱>\n학교는 이미 끝났지만...",
	"<쉣빠딱>\n가야겠지... 마지막 미소녀를 쓰러뜨리기 위해서.",
	"FadeOut",
	"그렇다, 내가 지면 이제 인류에 희망은 없는 것이다!"
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
				Application.LoadLevel("talk_mazuki");
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