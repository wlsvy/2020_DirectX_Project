#pragma once

class Uncopyable {
public:
	Uncopyable(const Uncopyable&) = delete;
	Uncopyable& operator=(const Uncopyable&) = delete;
protected:
	Uncopyable() = default;
	virtual ~Uncopyable() = default;
};