#pragma strict

var text : GUIText;
var dialogBox : GUITexture;
var bgm : AudioClip;
var effect : AudioClip;
var black : GUITexture;
var explosion : Transform;

var body : Transform;

private var delay : int;
private var dd : int;
private var dlg_no = 0;

private var dlg = [
	"FadeIn",
	"<어휴>\n크헉! 이건 말도 안돼!",
	"<어휴>\n안 돼! 난 죽기 싫어!",
	"FadeOut",
	"Effect",
	"<어휴>\n크아아아아악!",
	"Die",
	"FadeIn",
	"<쉣빠딱>\n모두 끝났어...",
	"<경찰>\n이봐...",
	"<쉣빠딱>\n네?",
	"<경찰>\n이 여자애들을 네가 모두 죽인거냐?",
	"<쉣빠딱>\n앗! 그게...",
	"<경찰>\n변명해도 소용없다!\n넌 묵비권을 행사할 수 없으며 고문을 받을 권리가 있다.",
	"<쉣빠딱>\n크흑!",
	"<쉣빠딱>\n사식은 햄버그 스테이크로 부탁해요.",
	"BgmOn",
	"<경찰>\n뻥이야.",
	"<쉣빠딱>\n엥?",
	"<경찰>\n굿 잡!",
	"<경찰>\n아아... 이제야 마을 골치거리인 미소녀들이 없어졌군.\n이제 이 마을도 살아날 거야.",
	"<경찰>\n넌 정말 용감한 일을 했어.\n경찰이 되어보는 게 어때?",
	"<쉣빠딱>\n경찰요? 제가?",
	"<경찰>\n그래.",
	"FadeOut",
	"그렇게 나는 경찰이 되었다.",
	"그리고 오늘도 정의를 위해서",
	"붉은 봉 들고 교통 정리를 하고 있다.",
	"반복되는 지겨운 일상이지만\n이런 평화도 미소녀를 물리쳤기 때문이겠지.",
	"도시의 평화를 지키는 쉣빠닥! 30세!",
	"솔로다!",
	"제작 : 똥똥배\nBGM : 창도 단편제 음악\n효과음 : RPG2000 리소스",
	"THE END"
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
			if(dlg_no < dlg.GetLength(0) - 1)
			{
				delay = 0;
				dd = 0;
				dlg_no++;
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
			//Instantiate(explosion, transform.position, Quaternion.identity);
			Destroy(body.gameObject);
			Destroy(explosion.gameObject);
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "FadeIn")
		{
			Fade(1.0f, 0.0f, 1.0f, black);
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "FadeOut")
		{
			Fade(0.0f, 1.0f, 1.0f, black);
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "Effect")
		{
			AudioSource.PlayClipAtPoint(effect, Vector3(0, 0, 0));		
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