#include "KeyboardClass.h"

KeyboardClass::KeyboardClass() {
	for (int i = 0; i < 256; i++)
		this->keyStates[i] = false; //키 스테이트 초기화
}

bool KeyboardClass::KeyIsPressed(const unsigned char keycode) {
	return this->keyStates[keycode];
}

bool KeyboardClass::GetKeyDown(const unsigned char keycode)
{
	return this->UpDownStates[keycode] == 1;
}

bool KeyboardClass::GetKeyUp(const unsigned char keycode)
{
	return this->UpDownStates[keycode] == -1;
}

bool KeyboardClass::KeyBufferIsEmpty() {
	return this->keyBuffer.empty();
}

bool KeyboardClass::CharBufferIsEmpty() {
	return this->charBuffer.empty();
}

KeyboardEvent KeyboardClass::ReadKey() {
	if (this->keyBuffer.empty()) { //비어있는지 확인
		return KeyboardEvent();
	}
	else {
		KeyboardEvent e = this->keyBuffer.front();
		this->keyBuffer.pop();
		return e;//이벤트 반환
	}
}
unsigned char KeyboardClass::ReadChar() {
	if (this->charBuffer.empty()) {
		return 0u; //return 0 (NULL char)
	}
	else {
		unsigned char e = this->charBuffer.front();
		this->charBuffer.pop();
		return e;
	}
}

void KeyboardClass::OnKeyPressed(const unsigned char key) {
	this->keyStates[key] = true;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void KeyboardClass::OnKeyReleased(const unsigned char key) {
	this->keyStates[key] = false;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void KeyboardClass::OnChar(const unsigned char key) {
	this->charBuffer.push(key);
}

void KeyboardClass::EnableAutoRepeatKeys() {
	this->autoRepeatKeys = true;
}

void KeyboardClass::DisableAutoRepeatKeys() {
	this->autoRepeatKeys = false;
}

void KeyboardClass::EnableAutoRepeatChars() {
	this->autoRepeatChars = true;
}

void KeyboardClass::DisableAutoRepeatChars() {
	this->autoRepeatChars = false;
}

bool KeyboardClass::IsKeysAutoRepeat() {
	return this->autoRepeatKeys;
}

bool KeyboardClass::IsCharsAutoRepeat() {
	return this->autoRepeatChars;
}

void KeyboardClass::Update()
{
	memset(UpDownStates, 0, sizeof(UpDownStates));

	while (!CharBufferIsEmpty()) {
		unsigned char ch = ReadChar();
	}

	while (!KeyBufferIsEmpty()) {
		KeyboardEvent kbe = ReadKey();
		unsigned char keycode = kbe.GetKeyCode();

		if (kbe.IsPress()) UpDownStates[keycode] = 1;
		else if (kbe.IsRelease()) UpDownStates[keycode] = -1;
	}
}
