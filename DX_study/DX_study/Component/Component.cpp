#include "Component.h"
#include "GameObject_v2.h"

<<<<<<< HEAD
Component::Component(GameObject_v2 & obj) : GameObject(obj) {}
=======
Component::Component(const COMPONENT_INIT_DESC & desc) : gameObject(desc.mGameObj), Time(desc.mTime) {}
>>>>>>> parent of cb3481a... refactoring

int Component::getComponentID() {return m_ID;}

void Component::Start() {}

void Component::Update() {}

void Component::FixedUpdate() {}
