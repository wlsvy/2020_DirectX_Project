#include "AnimationManager.h"

#include <algorithm>

#include "../Component/Animator.h"

void AnimationManager::RegisterComponent(const std::shared_ptr<Animator>& compo)
{
	m_AnimatorBuffer.push_back(compo);
}

void AnimationManager::DeregisterComponent(const std::shared_ptr<Animator>& compo)
{
	auto iter = std::find(m_AnimatorBuffer.begin(), m_AnimatorBuffer.end(), compo);

	if (iter != m_AnimatorBuffer.end()) {
		m_AnimatorBuffer.erase(iter);
	}
}

void AnimationManager::Update()
{
	for (auto iter : m_AnimatorBuffer) {
		if (iter->Enabled &&
			iter->GameObject.enabled &&
			iter->m_IsRunning) 
		{
			iter->Update(Timer::GetDeltaTime());
		}

	}
}