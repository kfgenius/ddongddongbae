#pragma strict

private var speed = 5;
private var rotSpeed = 120;
private var power = 2000;
private var life = true;

var bullet : Transform;
var shoot_snd : AudioClip;
var hit_snd : AudioClip;
var effect : Transform;
var DamageFlash : GameObject;
var stageName : String;

function Update()
{
	if(jsGameManager.hp <= 0)
	{
		if(life)
		{
			Die();
			life = false;
		}
		
		jsGameManager.hp = 0;
		return;
	}
	
	//이동
	var amtToMove = speed * Time.deltaTime;
	var amtToRot = rotSpeed * Time.deltaTime;
	
	var front = Input.GetAxis("Vertical");
	//var right = Input.GetAxis("Horizontal");
	var ang = Input.GetAxis("Horizontal");
	
	transform.Translate(Vector3.forward * front * amtToMove);
	if(Input.GetButton("Shift"))
	{
		transform.Translate(Vector3.right * ang * amtToMove);
	}
	else
	{
		transform.Rotate(Vector3(0, ang * amtToRot, 0));
	}
	
	transform.position.x = Mathf.Clamp(transform.position.x, -45, 45);
	transform.position.z = Mathf.Clamp(transform.position.z, -45, 45);
	
	//공격
	if(Input.GetButtonDown("Jump") || Input.GetButtonDown("Fire1"))
	{
		var shootPoint = GameObject.Find("shootPoint");
		var bullet = Instantiate(bullet, shootPoint.transform.position, Quaternion.identity);
		Instantiate(effect, shootPoint.transform.position, Quaternion.identity);
		bullet.rigidbody.AddForce(transform.forward * power);
		bullet.rigidbody.AddForce(transform.up * 20);
		Debug.Log("Fire!");
		AudioSource.PlayClipAtPoint(shoot_snd, shootPoint.transform.position, 0.5f);
	}
}


function Fade (start : float, end : float, length : float, currentObject : GameObject)
{
	for(var i = 0.0; i < 1.0; i += Time.deltaTime*(1/length))
	{
		currentObject.guiTexture.color.a = Mathf.Lerp(start, end, i);
		yield;
		currentObject.guiTexture.color.a = end;
	}
}
 
function FlashWhenHit()
{
    Fade (0, 0.3, 0.5, DamageFlash);
    yield WaitForSeconds (.01);
    Fade (0.3, 0, 0.5, DamageFlash);
}

function OnTriggerEnter(coll : Collider)
{
	if(coll.tag == "ENEMY_BULLET")
	{
		AudioSource.PlayClipAtPoint(hit_snd, transform.position);
		FlashWhenHit();
		jsGameManager.hp -= 10;
		
		Destroy(coll.gameObject);
	}
}

function Die()
{
	transform.Rotate(Vector3(0, 0, 90));
	yield WaitForSeconds (5.0f);
	Application.LoadLevel(stageName);
}