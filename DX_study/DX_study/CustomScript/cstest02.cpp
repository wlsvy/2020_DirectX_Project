#include "cstest02.h"

void cstest02::Start()
{
	tmpcs = GetComponent<cstest>();
}

void cstest02::Update()
{
	gameObject->transform.rotate(0.0f, Timer::GetDeltaTime() * tmpcs->speed, 0.0f);
}
