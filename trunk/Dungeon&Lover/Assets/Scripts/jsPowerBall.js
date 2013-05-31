#pragma strict

var snd : AudioClip;

private var delay : int;

function Start()
{
	delay = 0;
}

function Update () {
	//var amtToMove = 10 * Time.deltaTime;
	//transform.Translate(-transform.forward * amtToMove);
	
	delay++;

	if(delay >= 300 || transform.position.y <= -1.0 || transform.position.x >= 60 || transform.position.x <= -60 || transform.position.z >= 60 || transform.position.z <= -60 )
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
	/*else if(coll.tag == "MainCamera")
	{
		Debug.Log("AAAA");
	
		AudioSource.PlayClipAtPoint(snd, transform.position);
		jsGameManager.FlashWhenHit();
		jsGameManager.hp -= 10;
		
		Destroy(gameObject);
	}*/
}