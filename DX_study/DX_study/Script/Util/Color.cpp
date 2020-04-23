#include "Color.h"

const Color4Byte Color4Byte::UnloadedTextureColor = Color4Byte(100, 100, 100);
const Color4Byte Color4Byte::UnhandledTextureColor = Color4Byte(196, 48, 203);

Color4Byte::Color4Byte() : Value(0) {}

Color4Byte::Color4Byte(unsigned int val) : Value(val) {}

Color4Byte::Color4Byte(BYTE r, BYTE g, BYTE b) : Color4Byte(r, g, b, 255) {}

Color4Byte::Color4Byte(BYTE r, BYTE g, BYTE b, BYTE a) {
	RGBA[0] = r;
	RGBA[1] = g;
	RGBA[2] = b;
	RGBA[3] = a;
}

Color4Byte::Color4Byte(const Color4Byte & src) : Value(src.Value) {}

constexpr BYTE Color4Byte::GetR() const
{
	return this->RGBA[0];
}

constexpr BYTE Color4Byte::GetG() const
{
	return this->RGBA[1];
}

constexpr BYTE Color4Byte::GetB() const
{
	return this->RGBA[2];
}

constexpr BYTE Color4Byte::GetA() const
{
	return this->RGBA[3];
}

void Color4Byte::SetR(BYTE r)
{
	this->RGBA[0] = r;
}

void Color4Byte::SetG(BYTE g)
{
	this->RGBA[1] = g;
}

void Color4Byte::SetB(BYTE b)
{
	this->RGBA[2] = b;
}

void Color4Byte::SetA(BYTE a)
{
	this->RGBA[3] = a;
}
