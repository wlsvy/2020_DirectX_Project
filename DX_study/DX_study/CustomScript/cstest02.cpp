#include "cstest02.h"

void cstest02::Start()
{
	tmpcs = GetComponent<cstest>();
}

void cstest02::Update()
{
<<<<<<< HEAD
	GameObject.transform.rotate(0.0f, Timer::GetDeltaTime() * tmpcs->speed, 0.0f);
=======
	gameObject->transform.rotate(0.0f, Time->GetDeltaTime() * tmpcs->speed, 0.0f);
>>>>>>> parent of cb3481a... refactoring
}
