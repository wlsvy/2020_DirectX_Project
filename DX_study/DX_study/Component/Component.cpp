#include "Component.h"
#include "GameObject_v2.h"

Component::Component(GameObject_v2 & obj) : gameObject(obj) {}

int Component::getComponentID() {return m_ID;}

void Component::Start() {}

void Component::Update() {}

void Component::FixedUpdate() {}
