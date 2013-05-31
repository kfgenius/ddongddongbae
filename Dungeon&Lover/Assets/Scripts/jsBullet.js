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
	if(coll.tag == "BLOCK")
	{
		Destroy(gameObject);
	}
}