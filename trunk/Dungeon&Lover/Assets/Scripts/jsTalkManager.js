#pragma strict

var dlg_number : int;
var boss : Transform;
var explosion : Transform;
var animator : Animator;
var text : GUIText;
var dialogBox : GUITexture;
var black : GUITexture;
var bgm : AudioClip;
var effect : AudioClip;
var next_scene : String;

private var delay : int;
private var dd : int;
private var dlg_no = 0;
private var dlg_ref : Object;
private var scroll_dely = 3;

private var dlg1_kor = [
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

private var dlg1_eng = [
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

private var dlg2_kor = [
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

private var dlg2_eng = [
	"<Saori>\nI can't believe this!\nHow this piece of shit could do this to me...",
	"<SPT>\nGood-bye!",
	"<Saori>\nKukuku, your relive too early...",
	"<Saori>\nI'm one of three rosebuds in this neighbourhood!\nTwo still remain!",
	"<SPT>\nWhat! They're still alive?",
	"Die",
	"<Saori>\nKuhahah!\nI already see your nightmare!",
	"FadeOut",
	"<SPT>\n...",
	"<SPT>\nI should head to school."
];

private var dlg3_kor = [
	"FadeIn",
	"<쉣빠딱>\n헉헉... 늦진 않았겠지...",
	"<쉣빠딱>\n어라? 하늘이 완전 컴컴한데?\n조금... 늦은게... 아닌 듯?",
	"<???>\n대단하군",
	"<쉣빠딱>\n누구냐!",
	"YaohiIn",
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
	"<야오희>\n철괴!!!",
	"TIP : 철괴는 공격할 때 잠깐 풀립니다.\n그 때를 노려서 총알을 박아 줍시다."
];

private var dlg3_eng = [
	"FadeIn",
	"<SPT>\nMy gosh... Not late, I suppose...",
	"<SPT>\nOuch, the sky's so dim!\nI must be a little late... or not?",
	"<???>\nImpressive.",
	"<SPT>\nWho are you?",
	"YaohiIn",
	"BgmOn",
	"<Yaohi>\nI have never seen anyone coming to school\nin this late hour other than me.You are the second.",
	"She's Yaohi, the bully of my school.\nI rarely saw her in the school but she was here,\nin this hour.",
	"<Yaohi>\nI'm falling in love with you.",
	"<SPT>\nShit~~~~!!",
	"<Yaohi>\nLet's drive moonlit highways, would you?",
	"<SPT>\nSo you rubbed your face to that end.",
	"<Yaohi>\nOh... what a bold taunt.",
	"<Yaohi>\nChallange accepted!",
	"<Yaohi>\nBut you will never be able to make a scratch on me!",
	"<Yaohi>\nTEKKAI!!!",
	"TIP: TEKKAI goes of temporalily on Yaohi's attack.\nMake your bullet's way on precise timing"
];

private var dlg4_kor = [
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

private var dlg4_eng = [
	"<Yaohi>\nKu... huhu...",
	"<Yaohi>\nMy defeat, decesive and total!",
	"<SPT>\nYaohi...",
	"<Yaohi>\nHu hu...\nYour independancy makes you more... beautiful.",
	"<Yaohi>\nMy last hope... is your hug...",
	"<SPT>\nNever.",
	"Die",
	"<Yaohi>\nKWAAAAA~",
	"<SPT>\nClasses are already over...",
	"<SPT>\nBut I should continue...\nto get rid of the last rosebud.",
	"FadeOut",
	"Oh, now I'm the last hope of the mankind!"
];

private var dlg5_kor = [
	"FadeIn",
	"<쉣빠딱>\n... 아무도 없나...",
	"<???>\n역시 용감하군\n내 학생다워.",
	"<쉣빠딱>\n마즈키 선생님...",
	"MazukiIn",
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

private var dlg5_eng = [
	"FadeIn",
	"<SPT>\n...\nIs anybody there?",
	"<???>\nYou're so brave. My true pupil.",
	"<SPT>\nTeacher Mazuki...",
	"MazukiIn",
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

private var dlg6_kor = [
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

private var dlg6_eng = [
	"<Mazuki>\nArghhh!",
	"<Mazuki>\nGreat shot, nicely done.",
	"<Mazuki>\nPupil must overcome his master, sooner or later.",
	"<Mazuki>\nMy last wish - Can I see your face?",
	"<SPT>\nNoo~O",
	"Die",
	"<Mazuki>\nFuck!",
	"<SPT>\nNow I killed last of these abomination, didn't I?",
	"<SPT>\nOh, what a chill..",
	"<SPT>\nRosebuds shall walk this world no more!"
];

private var dlg7_kor = [
	"BgmOn",
	"<쉣빠딱>\n크헉, 이게 도대체...?",
	"<???>\n오랫만이야... 쉣빠딱",
	"<쉣빠딱>\n이 목소리는!!",
	"<쉣빠딱>\n이럴 수가... 넌 분명 죽었을 텐데?",
	"UhyuIn",
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

private var dlg7_eng = [
	"BgmOn",
	"<SPT>\nArgh, What the...?",
	"<???>\nIt has been a long time... Shit-pa-tack.",
	"<SPT>\nThe voice!",
	"<SPT>\nOh my, you must be dead-",
	"UhyuIn",
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

private var dlg8_kor = [
	"FadeIn",
	"<어휴>\n크헉! 이건 말도 안돼!",
	"<어휴>\n안 돼! 난 죽기 싫어!",
	"FadeOut",
	"Effect",
	"<어휴>\n크아아아아악!",
	"Destroy",
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
	"THE END"
];

private var dlg8_eng = [
	"FadeIn",
	"<Gosh>\nAh, nonsense!",
	"<Gosh>\nNooo! I don't wanna die!",
	"FadeOut",
	"Effect",
	"<Gosh>\nAAAaaaAAAaaaAAArgh!",
	"Destroy",
	"FadeIn",
	"<SPT>\nFinally, it's all over...",
	"<Police>\nHey.",
	"<SPT>\nYes?",
	"<Police>\nYou killed all these girls?",
	"<SPT>\nOh! well, er...",
	"<Police>\nExcuses are futile!\nYou have no right to remain silent and eligible to tortures.",
	"<SPT>\nAlas!",
	"<SPT>\nMy only wish in the jail is a piece of hamburger steak!",
	"BgmOn",
	"<Police>\nNo, just kidding.",
	"<SPT>\nOh?",
	"<Police>\nGOOD JOB!",
	"<Police>\nRosebuds, the town's headaches, are gone now.\nWe can have a hope.",
	"<Police>\nYou did a great deed.\nHow about being a police?",
	"<SPT>\nPolice? me?",
	"<Police>\nOh yeah.",
	"FadeOut",
	"So I became a policeman",
	"And even today, to protect justice,",
	"I give traffic signs with glowing baton in my hand.",
	"Tedious repeating everyday life,\ncame at last over dead rosebuds.",
	"I'm Shit-pa-tack, guardian of the city, aged 30!",
	"Virgin!",
	"THE END"
];

function Start () {
	text.material.color = Color.black;
	text.fontSize = Screen.width  / 26;

	delay = 0;
	dd = 0;

	if(Application.systemLanguage.ToString() == "Korean")
	{
		switch(dlg_number)
		{
			case 1 : dlg_ref = dlg1_kor; break;
			case 2 : dlg_ref = dlg2_kor; break;
			case 3 : dlg_ref = dlg3_kor; break;
			case 4 : dlg_ref = dlg4_kor; break;
			case 5 : dlg_ref = dlg5_kor; break;
			case 6 : dlg_ref = dlg6_kor; break;
			case 7 : dlg_ref = dlg7_kor; break;
			case 8 : dlg_ref = dlg8_kor; break;
		}
	}
	else
	{
		switch(dlg_number)
		{
			case 1 : dlg_ref = dlg1_eng; break;
			case 2 : dlg_ref = dlg2_eng; break;
			case 3 : dlg_ref = dlg3_eng; break;
			case 4 : dlg_ref = dlg4_eng; break;
			case 5 : dlg_ref = dlg5_eng; break;
			case 6 : dlg_ref = dlg6_eng; break;
			case 7 : dlg_ref = dlg7_eng; break;
			case 8 : dlg_ref = dlg8_eng; break;
		}
	}
}

function Update ()
{
	var dlg : String[] = dlg_ref;
	if(dlg_no >= dlg.GetLength(0))return;

	dd++;
	
	if(dd > scroll_dely)
	{
		delay++;
		dd = 0;
	}

	if(Input.GetButtonDown("Jump") || Input.GetButtonDown("Fire1"))
	{
		if(delay >= dlg[dlg_no].Length)
		{
			if(dlg[dlg_no] == "THE END")return;
		
			delay = 0;
			dd = 0;
			dlg_no++;
			
			if(dlg_no >= dlg.GetLength(0))
			{
				Application.LoadLevel(next_scene);
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

function MoveX (start : float, end : float, length : float, currentObject : Transform)
{
	for(var i = 0.0; i < 1.0; i += Time.deltaTime*(1/length))
	{
		currentObject.position.x = Mathf.Lerp(start, end, i);
		yield;
		currentObject.position.x = end;
	}
}

function MoveY (start : float, end : float, length : float, currentObject : Transform)
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
	var dlg : String[] = dlg_ref;

	if(dlg_no < dlg.GetLength(0))
	{
		if(dlg[dlg_no] == "FadeIn")
		{
			Fade(1.0f, 0.0f, 1.0, black);
			dlg_no++;
			return;		
		}
		if(dlg[dlg_no] == "FadeOut")
		{
			Fade(0.0f, 1.0f, 1.0, black);
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "BgmOn")
		{
			AudioSource.PlayClipAtPoint(bgm, Vector3(0, 3.3, -2));		
			dlg_no++;
			return;		
		}
		if(dlg[dlg_no] == "Die")
		{
			AudioSource.PlayClipAtPoint(effect, Vector3(0, 0, 0));
			animator.SetBool("life", false);	
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "YaohiIn")
		{
			MoveX(4.0f, 0.0f, 1.0f, boss);
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "MazukiIn")
		{
			MoveY(-4.0f, 0.0f, 1.0f, boss);
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "UhyuIn")
		{
			MoveY(5.0f, 0.0f, 1.0f, boss);
			dlg_no++;
			return;		
		}
		else if(dlg[dlg_no] == "Effect")
		{
			AudioSource.PlayClipAtPoint(effect, Vector3(0, 0, 0));		
			dlg_no++;
			return;		
		}
		if(dlg[dlg_no] == "Destroy")
		{
			Destroy(boss.gameObject);
			Destroy(explosion.gameObject);
			dlg_no++;
			return;		
		}

		var txt_end = Mathf.Min(dlg[dlg_no].Length, delay);
		text.text = dlg[dlg_no].Substring(0, txt_end);
	}
}