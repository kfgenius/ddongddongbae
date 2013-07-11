#pragma strict

function Start ()
{
}

function Update ()
{
}

function OnGUI()
{
	var w = Screen.width / 4;
	var h = Screen.height / 32;
	
	
	var menuNum : int;
	
	if(GUI.Button(Rect(w, h * 20, w * 2, h * 2), "Easy Mode"))menuNum = 1;
	if(GUI.Button(Rect(w, h * 23, w * 2, h * 2), "Normal Mode"))menuNum = 2;
	if(GUI.Button(Rect(w, h * 26, w * 2, h * 2), "Hard Mode"))menuNum = 3;
	if(GUI.Button(Rect(w, h * 29, w * 2, h * 2), "Hell Mode"))menuNum = 4;
	
	if(GUI.Button(Rect(0, Screen.height - 20, 80, 20), "Credit"))menuNum = 5;
	
	switch(menuNum)
	{
		case 1 :
		case 2 :
		case 3 :
		case 4 :
			jsGameManager.difficult = menuNum;
			Application.LoadLevel("talk_saori");
			break;
			
		case 5:
			Application.LoadLevel("Credit");
			break;
	}
}