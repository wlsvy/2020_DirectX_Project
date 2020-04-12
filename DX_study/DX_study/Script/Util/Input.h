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
	const unsigned char LEFT_ARROW = 0x25;
	const unsigned char UP_ARROW = 0x26;
	const unsigned char RIGHT_ARROW = 0x27;
	const unsigned char DOWN_ARROW = 0x28;
}