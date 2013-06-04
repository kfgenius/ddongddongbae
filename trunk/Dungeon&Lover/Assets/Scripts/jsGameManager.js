#pragma strict

static var hp : int;
static var difficult : int;
var txtHP : GUIText;
var color : Color;

private var test : int;

function Start () {
	hp = 100;
	
	if(jsGameManager.difficult == 3)
	{
		hp /= 2;
	}
	else if(jsGameManager.difficult == 4)
	{
		hp = 1;
	}
	
	txtHP.material.color = color;
}

function OnGUI()
{
	txtHP.text = "HP : " + hp;
}

function Update ()
{
}