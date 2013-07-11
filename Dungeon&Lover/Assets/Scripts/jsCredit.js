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
	
	if(GUI.Button(Rect(w, h * 28, w * 2, h * 3), "Back"))menuNum = 1;
	
	switch(menuNum)
	{
		case 1 :
			Application.LoadLevel("Title");
			break;
	}
}