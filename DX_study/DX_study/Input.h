#pragma once

namespace Input {
	bool GetKey(const unsigned char key);
	bool GetKeyDown(const unsigned char key);
	bool GetKeyUp(const unsigned char key);

	bool IsMouseLeftDown();
	bool IsMouseMiddleDown();
	bool IsMouseRightDown();

	int GetMousePosX();
	int GetMousePosY();
}