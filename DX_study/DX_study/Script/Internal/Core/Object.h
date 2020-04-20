#pragma once
#include <string>

#define MANAGED_OBJECT(typeName) \
   public: \
    using ManagedType = typeName; \

class Object {
public:
	Object();
	Object(const std::string & name);
	Object(const Object &);
	Object(Object&&);
	virtual ~Object() {}
	Object& operator=(const Object &);

	int GetId() const { return m_Id; }
	virtual void OnGui() {}

	std::string Name = "Object";
	
private:
	void InitObject();

	int m_Id;
};