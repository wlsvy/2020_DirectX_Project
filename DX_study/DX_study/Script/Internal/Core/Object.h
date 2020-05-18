#pragma once
#include <string>
#include <memory>

#define MANAGED_OBJECT(typeName) \
   public: \
    using ManagedType = typeName; \

class Object abstract : public std::enable_shared_from_this<Object> {
public:
	Object();
	Object(const std::string & name);
	Object(const Object &);
	Object(Object&&);
	virtual ~Object() {}
	Object& operator=(const Object &);

	int GetId() const { return m_Id; }

	std::shared_ptr<Object> GetPtr() {
		return shared_from_this();
	}
	template<typename T>
	std::shared_ptr<T> GetPtr() {
		return std::dynamic_pointer_cast<T>(shared_from_this());
	}

	virtual void OnGui(const char* option = "op") {}

	std::string Name = "Object";
	
private:
	void InitObject();

	int m_Id;
};