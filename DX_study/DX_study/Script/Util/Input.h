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
	int GetMouseDeltaX();
	int GetMouseDeltaY();

	const unsigned char SHIFT = 0x10;
	const unsigned char ESC = 0x1B;
	const unsigned char SPACE = 0x20;

}