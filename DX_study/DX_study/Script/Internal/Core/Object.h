#pragma once
#include <string>
#include <memory>

#define OBJECT_DESCRIPTION(type, baseType) \
   public: \
	##type() : Object(#type) {} \
    using BaseType = baseType; \

#define MANAGED_OBJECT(typeName) \
   public: \
	##typeName() : Object(#typeName) {} \
    using BaseType = typeName; \

class Object {
public:
	Object();
	Object(const std::string & name);
	virtual ~Object() {}

	int GetId() const { return m_Id; }

	virtual void OnGui() {}

	std::string Name = "Object";
	
private:
	void InitObjectID();

	int m_Id;
};

template<typename T>
std::shared_ptr<T> CreateInstance() {
	
	
}

template<typename T>
void Destroy() {

}