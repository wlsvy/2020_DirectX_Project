#include "cstest.h"

void cstest::Update()
{
	gameObject->transform.translate(0.0f, speed * Timer::GetDeltaTime(), 0.0f);
}