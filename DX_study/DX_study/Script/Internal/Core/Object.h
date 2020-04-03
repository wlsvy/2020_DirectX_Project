#pragma once
#include <string>

#define COMPONENT_CONSTRUCTOR(typeName) \
	public: \
	#typeName(GameObject * gameObj) : Component(gameObj) {} \

#define MANAGED_OBJECT(typeName) \
   public: \
    using Base_Type = typeName; \

class Object {
public:
	Object();
	Object(const std::string & name);
	virtual ~Object();

	int GetId() const { return m_Id; }
	virtual void OnGui() {}

	std::string Name = "Object";
	
private:
	void InitObject();

	int m_Id;
};