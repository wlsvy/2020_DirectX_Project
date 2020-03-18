#include "Component.h"

Component::Component(const COMPONENT_INIT_DESC & desc) : gameObject(desc.mGameObj), Time(desc.mTime) {}

int Component::getComponentID() {return mComponentID;}

COMPONENT_TYPE Component::getComponentType() {return mComponentType;}

void Component::Start() {}

void Component::Update() {}

void Component::FixedUpdate() {}
