#pragma strict

var text : GUIText;

private var alphaDown = true;

function Start ()
{
	text.fontSize = Screen.width  / 30;
}

function Update ()
{
	if(Input.GetButtonDown("Jump") || Input.GetButtonDown("Fire1"))
	{
		Application.LoadLevel("talk_saori");
	}
	
	if(alphaDown)
	{
		text.material.color.a -= 0.02f;
	}
	else
	{
		text.material.color.a += 0.02f;
	}
	
	if(text.material.color.a <= 0.0f)
	{
		alphaDown = false;
	}
	else if(text.material.color.a >= 1.0f)
	{
		alphaDown = true;
	}
}