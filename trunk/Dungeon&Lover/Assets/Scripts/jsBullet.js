#pragma strict

var snd : AudioClip;
var fire : Transform;

function Update () {
	if(transform.position.y <= -1.0)
	{
		Destroy(gameObject);
	}
}

function OnTriggerEnter(coll : Collider)
{
	if(coll.tag == "BOSS" && saori.hp > 0)
	{
		//Instantiate(fire, transform.position, Quaternion.identity);
	
		AudioSource.PlayClipAtPoint(snd, transform.position);
		Destroy(gameObject);
		
		saori.hp--;
	}
}