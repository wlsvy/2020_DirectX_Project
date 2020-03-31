#pragma once
#include <string>
#include <memory>

class Object {
public:
	Object();
	virtual ~Object() {}

	int GetId() const { return Id; }

	virtual void OnGui() {}

	std::string Name = "Object";
	
private:
	int Id;
};

template<typename T>
std::shared_ptr<T> CreateInstance() {

}

template<typename T>
void Destroy() {

}