#include "Color.h"

Color::Color() : color(0) {}

Color::Color(unsigned int val) : color(val) {}

Color::Color(BYTE r, BYTE g, BYTE b) : Color(r, g, b, 255) {}

Color::Color(BYTE r, BYTE g, BYTE b, BYTE a) {
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = a;
}

Color::Color(const Color & src) : color(src.color) {}

Color & Color::operator=(const Color & src)
{
	this->color = src.color;
	return *this;
}

bool Color::operator==(const Color & rhs) const
{
	return color==rhs.color;
}

bool Color::operator!=(const Color & rhs) const
{
	return color != rhs.color;
}

constexpr BYTE Color::GetR() const
{
	return this->rgba[0];
}

constexpr BYTE Color::GetG() const
{
	return this->rgba[1];
}

constexpr BYTE Color::GetB() const
{
	return this->rgba[2];
}

constexpr BYTE Color::GetA() const
{
	return this->rgba[3];
}

void Color::SetR(BYTE r)
{
	this->rgba[0] = r;
}

void Color::SetG(BYTE g)
{
	this->rgba[1] = g;
}

void Color::SetB(BYTE b)
{
	this->rgba[2] = b;
}

void Color::SetA(BYTE a)
{
	this->rgba[3] = a;
}

FColor::FColor() : FColor(1.0f, 1.0f, 1.0f, 1.0f) {}

FColor::FColor(BYTE4 r, BYTE4 g, BYTE4 b) : FColor(r, g, b, 1.0f) {}

FColor::FColor(BYTE4 r, BYTE4 g, BYTE4 b, BYTE4 a)
{
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = a;
}

constexpr BYTE4 FColor::GetR() const
{
	return rgba[0];
}

void FColor::SetR(BYTE4 r)
{
	rgba[0] = r;
}

constexpr BYTE4 FColor::GetG() const
{
	return rgba[1];
}

void FColor::SetG(BYTE4 g)
{
	rgba[1] = g;
}

constexpr BYTE4 FColor::GetB() const
{
	return rgba[2];
}

void FColor::SetB(BYTE4 b)
{
	rgba[2] = b;
}

constexpr BYTE4 FColor::GetA() const
{
	return rgba[3];
}

void FColor::SetA(BYTE4 a)
{
	rgba[3] = a;
}

float * FColor::Get_ptr() 
{
	return rgba;
}
