#include "Input.h"
#include "../Internal/Engine/Engine.h"

bool Input::GetKey(const unsigned char key)
{
	return Engine::Get().GetKeyboard().KeyIsPressed(key);
}

bool Input::GetKeyDown(const unsigned char key)
{
	return Engine::Get().GetKeyboard().GetKeyDown(key);
}

bool Input::GetKeyUp(const unsigned char key)
{
	return Engine::Get().GetKeyboard().GetKeyUp(key);
}

bool Input::IsMouseLeftDown()
{
	return Engine::Get().GetMouse().IsLeftDown();
}

bool Input::IsMouseMiddleDown()
{
	return Engine::Get().GetMouse().IsMiddleDown();
}

bool Input::IsMouseRightDown()
{
	return Engine::Get().GetMouse().IsRightDown();
}

int Input::GetMousePosX()
{
	return Engine::Get().GetMouse().GetPosX();
}

int Input::GetMousePosY()
{
	return Engine::Get().GetMouse().GetPosY();
}

int Input::GetMouseDeltaX()
{
	return Engine::Get().GetMouse().GetDeltaX();
}

int Input::GetMouseDeltaY()
{
	return Engine::Get().GetMouse().GetDeltaY();
}