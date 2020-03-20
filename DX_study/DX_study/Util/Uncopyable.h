#pragma once

class Uncopyable {
public:
	Uncopyable() {}
	Uncopyable(const Uncopyable&) = delete;
	Uncopyable& operator=(const Uncopyable&) = delete;
};