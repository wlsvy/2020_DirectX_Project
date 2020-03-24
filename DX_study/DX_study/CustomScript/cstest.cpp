#include "cstest.h"

void cstest::Update()
{
	GameObject.transform.translate(0.0f, speed * Timer::GetDeltaTime(), 0.0f);
}