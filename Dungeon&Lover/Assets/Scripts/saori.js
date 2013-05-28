#pragma strict

var animator : Animator;
var target : Transform;
static var hp : int;
var snd : AudioClip;
var die_snd : AudioClip;
var shout_snd : AudioClip;
var DamageFlash : GameObject;
var txtHP : GUIText;

private var isHit : boolean;
private var isShout : boolean;
private var life : boolean;
private var delay : int;
private var speed : float;

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
    
function Start ()
{
	life = true;
	hp = 100;
	delay = 0;
	speed = 0;
	isHit = false;
	
	txtHP.material.color = Color.black;
}

function Update ()
{
	var hit : RaycastHit;
	
	if(!animator) return;
	if(life == false)return;

	if(life == true && hp <= 0)
	{
		
		AudioSource.PlayClipAtPoint(die_snd, transform.position);		
		life = false;
		animator.speed = 0.5f;
		animator.SetBool("life", false);
		return;
	}

	animator.SetFloat("speed", speed);
	
	var distance = Vector3.Distance(transform.position, target.transform.position);
	//Debug.Log(distance);
	
	if(distance <= 3.4)
	{
		var state = animator.GetCurrentAnimationClipState(0);
		
		if(state.GetLength(0) > 0)	
		{
			if(state[0].clip.name == "attack")
			{
				if(isShout == false)
				{
					AudioSource.PlayClipAtPoint(shout_snd, transform.position);
					isShout = true;
				}
			}
			else
			{
				isShout = false;
			}
		
			if(state[0].clip.name == "attack_end")
			{
				if(isHit == false)	//공격 성공
				{
					target.rigidbody.AddForce(transform.forward * 200);
					AudioSource.PlayClipAtPoint(snd, transform.position);
					FlashWhenHit();
					jsGameManager.hp -= 10;
					isHit = true;
				}
			}
			else
			{
				isHit = false;
			}
		}
	
		animator.SetBool("attack", true);

		speed = 0;
		delay = 0;
	}
	else
	{
		animator.SetBool("attack", false);
		
		delay += 1;
		
		if(delay > 600)
		{
			speed = 0;
			delay = 0;
		}
		else if(delay > 100)
		{
			speed = 1 + ((100 - hp) / 20);
			animator.speed = speed;
		}
	}

	var amtToMove = speed * 3 * Time.deltaTime;
	transform.LookAt(Vector3(target.position.x, transform.position.y, target.position.z));
	transform.Translate(Vector3.forward * amtToMove);
}

function OnGUI()
{
	txtHP.text = "BOSS : " + hp;
}