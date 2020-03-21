#include "Input.h"
#include "Engine/Engine.h"
#include "Mouse/MouseClass.h"

bool Input::GetKey(const unsigned char key)
{
	return Engine::GetInstance().GetKeyboard().KeyIsPressed(key);
}

bool Input::GetKeyDown(const unsigned char key)
{
	return Engine::GetInstance().GetKeyboard().GetKeyDown(key);
}

bool Input::GetKeyUp(const unsigned char key)
{
	return Engine::GetInstance().GetKeyboard().GetKeyUp(key);
}

bool Input::IsMouseLeftDown()
{
	return Engine::GetInstance().GetMouse().IsLeftDown();
}

bool Input::IsMouseMiddleDown()
{
	return Engine::GetInstance().GetMouse().IsMiddleDown();
}

bool Input::IsMouseRightDown()
{
	return Engine::GetInstance().GetMouse().IsRightDown();
}

int Input::GetMousePosX()
{
	return Engine::GetInstance().GetMouse().GetPosX();
}

int Input::GetMousePosY()
{
	return Engine::GetInstance().GetMouse().GetPosY();
}
