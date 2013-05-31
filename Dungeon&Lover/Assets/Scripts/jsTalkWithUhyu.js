#pragma strict

var text : GUIText;
var dialogBox : GUITexture;
var bgm : AudioClip;

var body : Transform;

private var delay : int;
private var dd : int;
private var dlg_no = 0;

private var dlg = [
	"BgmOn",
	"<쉣빠딱>\n크헉, 이게 도대체...?",
	"<???>\n오랫만이야... 쉣빠딱",
	"<쉣빠딱>\n이 목소리는!!",
	"<쉣빠딱>\n이럴 수가... 넌 분명 죽었을 텐데?",
	"FadeIn",
	"<쉣빠딱>\n어휴!",
	"<어휴>\n크크크... 미안하지만 이렇게 멀쩡히 살아있지.",
	"<쉣빠딱>\n그렇다면 우리 동네 미소녀가 이렇게 된 것이 다 너의 주술?",
	"<어휴>\n아니, 핵이잖여. 은근슬쩍 설정 바꾸지 마.",
	"<쉣빠딱>\n하긴 중간에 설정 바뀌는 만화 좀 짜증나지.",
	"<어휴>\n어쨌든 난 너에게 복수하기 위해 인간을 버리고 이렇게 다시 태어났다.",
	"<쉣빠딱>\n넌 처음부터 인간이 아니었어...\n그건 그렇치고 원한이라니...",
	"<어휴>\n......",
	"<어휴>\n까먹었다. 하지만 내 속에서 끓어오르는 분노는 그치지 않아!",
	"<어휴>\n목말라~ 타오른다! 괴롭다! 으어어어...",
	"<쉣빠딱>\n어디서 본 건 있어가지고 흉내내긴.",
	"<쉣빠딱>\n나도 피하지 않겠어!",
	"<어휴>\n와라! 쉣빠딱!\n하늘로 가자꾸나!"
];

function Start () {
	text.material.color = Color.black;
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