#pragma strict

var animator : Animator;
var text : GUIText;
var dialogBox : GUITexture;
var effect : AudioClip;

var body : Transform;
var black : GUITexture;

private var delay : int;
private var dd : int;
private var dlg_no = 0;

private var dlg = [
	"<마즈키>\n푸헉!",
	"<마즈키>\n흐흐, 훌륭하구나.\n멋진 샷이었다.",
	"<마즈키>\n제자란 언젠가 스승을 넘어야 할 존재지.",
	"<마즈키>\n마지막으로... 얼굴을 보여주겠나?",
	"<쉣빠딱>\n싫어유~",
	"Die",
	"<마즈키>\n썅!",
	"<쉣빠딱>\n이걸로 이 생물들은 멸종 되었겠지?",
	"<쉣빠딱>\n음... 뭐지 이 한기는...",
	"<쉣빠딱>\n모든 미소녀를 쓰러뜨렸을 텐데?"
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
				Application.LoadLevel("talk_uhyu");
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
		if(dlg[dlg_no] == "Die")
		{
			AudioSource.PlayClipAtPoint(effect, Vector3(0, 0, 0));
			animator.SetBool("life", false);	
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "FadeIn")
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
	
		var txt_end = Mathf.Min(dlg[dlg_no].Length, delay);
		text.text = dlg[dlg_no].Substring(0, txt_end);
	}
}