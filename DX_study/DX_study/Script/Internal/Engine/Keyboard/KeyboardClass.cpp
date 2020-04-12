#include "KeyboardClass.h"

KeyboardClass::KeyboardClass() {
	for (int i = 0; i < 256; i++)
		this->keyStates[i] = false; //Ű ������Ʈ �ʱ�ȭ
}

bool KeyboardClass::KeyIsPressed(const unsigned char keycode) {
	return this->keyStates[keycode];
}

bool KeyboardClass::KeyBufferIsEmpty() {
	return this->keyBuffer.empty();
}

bool KeyboardClass::CharBufferIsEmpty() {
	return this->charBuffer.empty();
}

KeyboardEvent KeyboardClass::ReadKey() {
	if (this->keyBuffer.empty()) { //����ִ��� Ȯ��
		return KeyboardEvent();
	}
	else {
		KeyboardEvent e = this->keyBuffer.front();
		this->keyBuffer.pop();
		return e;//�̺�Ʈ ��ȯ
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
	this->UpDownStates[key] = 1;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void KeyboardClass::OnKeyReleased(const unsigned char key) {
	this->keyStates[key] = false;
	this->UpDownStates[key] = -1;
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
	
	while (!CharBufferIsEmpty()) { ReadChar(); }
	while (!KeyBufferIsEmpty()) { ReadKey(); }
}
