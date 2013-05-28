#pragma strict

private var speed = 5;
private var rotSpeed = 120;
private var power = 2000;

var bullet : Transform;
var shoot_snd : AudioClip;
var effect : Transform;

function Update()
{
	//이동
	var amtToMove = speed * Time.deltaTime;
	var amtToRot = rotSpeed * Time.deltaTime;
	
	var front = Input.GetAxis("Vertical");
	var ang = Input.GetAxis("Horizontal");
	
	transform.Translate(Vector3.forward * front * amtToMove);
	transform.Rotate(Vector3(0, ang * amtToRot, 0));
	
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