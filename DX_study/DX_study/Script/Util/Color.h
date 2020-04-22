#pragma once
#include <DirectXMath.h>
using BYTE = unsigned char;
using UINT = unsigned int;

class Color4Byte {
public:
	Color4Byte();
	Color4Byte(unsigned int val);
	Color4Byte(BYTE r, BYTE g, BYTE b);
	Color4Byte(BYTE r, BYTE g, BYTE b, BYTE a);
	Color4Byte(const Color4Byte& src);

	Color4Byte& operator=(const Color4Byte& src) { Value = src.Value; return *this; }
	bool operator==(const Color4Byte& rhs) const { return Value == rhs.Value; }
	bool operator!=(const Color4Byte& rhs) const { return Value != rhs.Value; }
	operator UINT() const { return Value; }
	operator DirectX::XMFLOAT4() const { return DirectX::XMFLOAT4((float)RGBA[0] / 255, (float)RGBA[1] / 255, (float)RGBA[2] / 255, (float)RGBA[3] / 255); }

	void Set(float r, float g, float b, float a = 1.0f) { 
		RGBA[0] = (UINT)(r * 255);
		RGBA[1] = (UINT)(g * 255);
		RGBA[2] = (UINT)(b * 255);
		RGBA[3] = (UINT)(a * 255);
	}
	void Set(UINT r, UINT g, UINT b, UINT a) {
		RGBA[0] = r;
		RGBA[1] = g;
		RGBA[2] = b;
		RGBA[3] = a;
	}

	constexpr BYTE GetR() const;
	void SetR(BYTE r);

	constexpr BYTE GetG() const;
	void SetG(BYTE g);

	constexpr BYTE GetB() const;
	void SetB(BYTE b);

	constexpr BYTE GetA() const;
	void SetA(BYTE a);

	union {
		BYTE RGBA[4];
		unsigned int Value;
	};

};

namespace Colors {
	const Color4Byte UnloadedTextureColor(100, 100, 100);
	const Color4Byte UnhandledTextureColor(196, 48, 203);
}