#pragma strict

static var hp : int;
var txtHP : GUIText;
var color : Color;

private var test : int;

function Start () {
	hp = 100;
	txtHP.material.color = color;
}

function OnGUI()
{
	txtHP.text = "HP : " + hp;
}

function Update ()
{
}