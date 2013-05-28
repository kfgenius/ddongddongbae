#pragma strict

static var hp : int;
var txtHP : GUIText;
var gameMode : int;

var enemyBody : Transform;
var enemyHair : Transform;

var testTexture : Texture;

private var test : int;

function Start () {
	hp = 100;
	txtHP.material.color = Color.black;
	
	gameMode = 1;
	
	if(gameMode == 1)
	{
		enemyHair.renderer.material.color = Color.black;
		enemyBody.renderer.material.mainTexture = testTexture;
	}
}

function OnGUI()
{
	txtHP.text = "HP : " + hp;
}

function Update ()
{
}