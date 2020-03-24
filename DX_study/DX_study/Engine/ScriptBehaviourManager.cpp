#include "ScriptBehaviourManager.h"
#include "../Component/ScriptBehaviour.h"

using ScriptIterator = std::vector<std::shared_ptr<ScriptBehaviour>>::iterator;
using Script_ptr = std::shared_ptr<ScriptBehaviour>;

void ScriptBehaviourManager::Script_INIT(ScriptBehaviour * _script)
{
	_script->InputKeyboard = keyboard;
	_script->InputMouse = mouse;
	_script->Physics = mPhysicsManager;
}

void ScriptBehaviourManager::Update()
{
	//Update() ����
	ScriptIterator begin = mScriptBuffer->begin();
	ScriptIterator end = mScriptBuffer->end();

	for (ScriptIterator iter = begin; iter != end;) {
		if ((*iter).use_count() <= 1) {	
			//ref count�� 1�̸� ���� ������Ʈ���� ������Ʈ�� �����Ǿ��ٴ� �� -> ���⼭�� ����
			iter = mScriptBuffer->erase(iter);
			end = mScriptBuffer->end();
			continue;
		}

		Script_ptr script = (*iter);

		bool Component_valid = script.get()->enabled;
		bool GameObject_valid = script.get()->gameObject->enabled;
		if (Component_valid && GameObject_valid) {
			script.get()->Update();
		}
		iter++;
	}
}

void ScriptBehaviourManager::Start()
{
	//��ũ��Ʈ Start() ����
	ScriptIterator begin = mScriptBuffer->begin();
	ScriptIterator end = mScriptBuffer->end();

	for (ScriptIterator iter = begin; iter != end;) {
		if ((*iter).use_count() <= 1) {
			//ref count�� 1�̸� ���� ������Ʈ���� ������Ʈ�� �����Ǿ��ٴ� �� -> ���⼭�� ����
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
		}
		iter++;
	}
}
