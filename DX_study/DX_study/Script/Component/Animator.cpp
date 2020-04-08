#include "Animator.h"

#include "../Internal/Graphics/AnimationClip.h"
#include "../Internal/Core/InternalHelper.h"
#include "../Internal/Core/GameObject.h"

void Animator::Update(float _deltaTime)
{
	if (mClip == nullptr) return;

	mClip->GetResultInTime(mPlayTime, &mAnimResult);

	mPlayTime += _deltaTime / 10;
}

void Animator::Play()
{
	m_IsRunning = true;
	//if (mClip != nullptr) gameObject->renderer.drawSkinnedMesh = true;
	//else gameObject->renderer.drawSkinnedMesh = false;
}

void Animator::Stop()
{
	m_IsRunning = false;
	//gameObject->renderer.drawSkinnedMesh = false;
}

void Animator::SetClip(AnimationClip * _clip)
{
	mClip = _clip;
	if (mClip == nullptr) Stop();
	else Play();
		
}

void Animator::SetClip(const std::string & clipName)
{
	//SetClip(Module::GetAnimationClip(clipName).get());
}

AnimationClip * Animator::GetAnimClip()
{
	return mClip;
}

bool Animator::IsRunning()
{
	return m_IsRunning;
}

void Animator::OnGui()
{
	/*if (m_IsRunning) {
		if (ImGui::Button("Stop", ImVec2(150, 0))) Stop();
	} else if (ImGui::Button("Play", ImVec2(150, 0))) Play();*/
	
	//�ִϸ��̼� Ŭ�� �̸�
	//�ִϸ��̼� ���� �ӵ� ��
	//ImGui::Button
}

Animator::~Animator()
{
	//gameObject->renderer.animator = nullptr;
	//gameObject->renderer.drawSkinnedMesh = false;
}
