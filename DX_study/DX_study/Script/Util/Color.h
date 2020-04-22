#pragma once
typedef unsigned char BYTE;

class Color4Byte {
public:
	Color4Byte();
	Color4Byte(unsigned int val);
	Color4Byte(BYTE r, BYTE g, BYTE b);
	Color4Byte(BYTE r, BYTE g, BYTE b, BYTE a);
	Color4Byte(const Color4Byte& src);

	Color4Byte& operator=(const Color4Byte& src);
	bool operator==(const Color4Byte& rhs) const;
	bool operator!=(const Color4Byte& rhs) const;

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