#include <algorithm>

#include "ScriptBehaviourManager.h"
<<<<<<< HEAD:DX_study/DX_study/Engine/ScriptBehaviourManager.cpp
#include "../Component/ScriptBehaviour.h"

void ScriptBehaviourManager::RegisterComponent(const std::shared_ptr<ScriptBehaviour>& compo)
=======
#include "Component/ScriptBehaviour.h"

void ScriptBehaviourManager::Script_INIT(ScriptBehaviour * _script)
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/ScriptBehaviourManager.cpp
{
	m_ScriptBuffer.push_back(compo);
}

void ScriptBehaviourManager::DeregisterComponent(const std::shared_ptr<ScriptBehaviour>& compo)
{
<<<<<<< HEAD:DX_study/DX_study/Engine/ScriptBehaviourManager.cpp
	auto iter = std::find(m_ScriptBuffer.begin(), m_ScriptBuffer.end(), compo);
=======
	//Update() 실행
	typedef std::vector<std::shared_ptr<ScriptBehaviour>>::iterator ScriptIterator;
	typedef std::shared_ptr<ScriptBehaviour> Script_ptr;
	ScriptIterator begin = mScriptBuffer->begin();
	ScriptIterator end = mScriptBuffer->end();
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/ScriptBehaviourManager.cpp

	if (iter != m_ScriptBuffer.end()) {
		m_ScriptBuffer.erase(iter);
	}
}

void ScriptBehaviourManager::Update()
{
	for (auto& ptr : m_ScriptBuffer) {
		if (ptr->Enabled && 
			ptr->GameObject.enabled) 
		{
			ptr->Update();
		}
	}
}

void ScriptBehaviourManager::Start()
{
<<<<<<< HEAD:DX_study/DX_study/Engine/ScriptBehaviourManager.cpp
	for (auto& ptr : m_ScriptBuffer) {
		if (ptr->Enabled &&
			ptr->GameObject.enabled)
		{
			ptr->Start();
=======
	//스크립트 Start() 실행
	typedef std::vector<std::shared_ptr<ScriptBehaviour>>::iterator ScriptIterator;
	typedef std::shared_ptr<ScriptBehaviour> Script_ptr;
	ScriptIterator begin = mScriptBuffer->begin();
	ScriptIterator end = mScriptBuffer->end();

	for (ScriptIterator iter = begin; iter != end;) {
		if ((*iter).use_count() <= 1) {
			//ref count가 1이면 게임 오브젝트에서 컴포넌트가 삭제되었다는 것 -> 여기서도 삭제
			iter = mScriptBuffer->erase(iter);
			end = mScriptBuffer->end();
			continue;
		}

		Script_ptr script = (*iter);

		bool Component_valid = script.get()->enabled;
		bool GameObject_valid = script.get()->gameObject->enabled;
		if (Component_valid && GameObject_valid) {
			Script_INIT(script.get());
			script.get()->Start();
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/ScriptBehaviourManager.cpp
		}
	}
}
