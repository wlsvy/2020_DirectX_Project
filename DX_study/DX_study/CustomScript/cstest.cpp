#include "cstest.h"

void cstest::Update()
{
<<<<<<< HEAD
	GameObject.transform.translate(0.0f, speed * Timer::GetDeltaTime(), 0.0f);
=======
	gameObject->transform.translate(0.0f, speed * Time->GetDeltaTime(), 0.0f);
>>>>>>> parent of cb3481a... refactoring
}