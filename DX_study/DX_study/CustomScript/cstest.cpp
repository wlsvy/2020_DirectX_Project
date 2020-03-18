#include "cstest.h"

void cstest::Update()
{
	gameObject->transform.translate(0.0f, speed * Time->GetDeltaTime(), 0.0f);
}