#include <algorithm>

#include "ScriptBehaviourManager.h"
#include "../Component/ScriptBehaviour.h"

void ScriptBehaviourManager::RegisterComponent(const std::shared_ptr<ScriptBehaviour>& compo)
{
	m_ScriptBuffer.push_back(compo);
}

void ScriptBehaviourManager::DeregisterComponent(const std::shared_ptr<ScriptBehaviour>& compo)
{
	auto iter = std::find(m_ScriptBuffer.begin(), m_ScriptBuffer.end(), compo);

	if (iter != m_ScriptBuffer.end()) {
		m_ScriptBuffer.erase(iter);
	}
}

void ScriptBehaviourManager::Update()
{
	for (auto& ptr : m_ScriptBuffer) {
		if (ptr->enabled && 
			ptr->gameObject.enabled) 
		{
			ptr->Update();
		}
	}
}

void ScriptBehaviourManager::Start()
{
	for (auto& ptr : m_ScriptBuffer) {
		if (ptr->enabled &&
			ptr->gameObject.enabled)
		{
			ptr->Start();
		}
	}
}
