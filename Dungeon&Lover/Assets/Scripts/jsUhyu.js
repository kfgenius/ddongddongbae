#pragma strict

var target : Transform;
var powerBallHole : Transform;
var powerBall : Transform;
var hp : int;
var snd : AudioClip;
var die_snd : AudioClip;
var shout_snd : AudioClip;
var txtHP : GUIText;
var txtColor : Color;
var black : GUITexture;

private var isHit : boolean;
private var isShout : boolean;
private var life : boolean;
private var delay : int;
private var speed : float;
private var dir : Vector3;

function Start ()
{
	life = true;
	delay = 0;
	speed = 0;
	isHit = false;
	dir = Vector3.up;

	if(jsGameManager.difficult == 1)
	{
		hp /= 2;
	}
		
	txtHP.material.color = txtColor;
}

function Update ()
{
	var hit : RaycastHit;
	
	if(life == false)return;

	if(life == true && hp <= 0)
	{
		
		//AudioSource.PlayClipAtPoint(die_snd, transform.position);		
		life = false;
		
		Die();
		return;
	}

	var amtToMove = speed * Time.deltaTime;
	var amtToRot = 50 * Time.deltaTime;
	//transform.LookAt(Vector3(target.position.x, transform.position.y, target.position.z));
	//powerBallHole.transform.LookAt(target);
	//transform.LookAt(target);
		
	transform.Translate(dir * amtToMove);
	transform.Translate(Vector3.left * amtToMove * 5);
	transform.Rotate(Vector3(0, amtToRot, 0));
	
	if((transform.position.y >= 3))
	{
		dir = Vector3.down;
	}
	else if(transform.position.y <= 0)
	{
		dir = Vector3.up;
	}
	
	speed = 1.0f;
	
	delay++;
	
	if(delay >= 15)
	{
		var fwd = transform.TransformDirection(Vector3.forward);
		var obj = Instantiate(powerBall, powerBallHole.position, Quaternion.identity);
		obj.rigidbody.AddForce(fwd * 500);
		obj.rigidbody.AddForce(Vector3.down * 50);
		//powerBall.LookAt(target);
	
		delay = 0;
	}
}

function OnGUI()
{
	txtHP.text = "BOSS : " + hp;
}

function OnTriggerEnter(coll : Collider)
{
	if(coll.tag == "BULLET" && hp > 0)
	{
		//Instantiate(fire, transform.position, Quaternion.identity);
		//rigidbody.AddForce(coll.transform.forward * 10);
	
		AudioSource.PlayClipAtPoint(snd, transform.position);
		Destroy(coll.gameObject);
		
		//transform.LookAt(Vector3(target.position.x, transform.position.y, target.position.z));
		transform.position.x = Random.RandomRange(-35.0f, 35.0f);
		transform.position.z = Random.RandomRange(-35.0f, 35.0f);
		
		hp--;
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

function Die()
{
	Fade(0.0f, 1.0f, 3.0f, black);
	yield WaitForSeconds(3.0f);
	Application.LoadLevel("talk_uhyu_end");
}