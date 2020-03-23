#include "Animator.h"
#include "../AnimationClip.h"

Animator::Animator(GameObject_v2 & obj) : Behaviour(desc)
{
	mComponentType = COMPONENT_ANIMATION;
	std::strcpy(Name, "Animator");
	GameObject->renderer.animator = this;
	GameObject->renderer.drawSkinnedMesh = false;
}

void Animator::Update(float _deltaTime)
{
	if (mClip == nullptr) return;

	mClip->GetResultInTime(mPlayTime, &mAnimResult);

	mPlayTime += _deltaTime / 10;
}

void Animator::Play()
{
	m_IsRunning = true;
	if (mClip != nullptr) GameObject->renderer.drawSkinnedMesh = true;
	else GameObject->renderer.drawSkinnedMesh = false;
}

void Animator::Stop()
{
	m_IsRunning = false;
	GameObject->renderer.drawSkinnedMesh = false;
}

void Animator::SetAnimClip(AnimationClip * _clip)
{
	mClip = _clip;
	if (mClip == nullptr) Stop();
	else Play();
		
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
	if (m_IsRunning) {
		if (ImGui::Button("Stop", ImVec2(150, 0))) Stop();
	} else if (ImGui::Button("Play", ImVec2(150, 0))) Play();
	
	//애니메이션 클립 이름
	//애니메이션 진행 속도 바
	//ImGui::Button
}

Animator::~Animator()
{
	GameObject->renderer.animator = nullptr;
	GameObject->renderer.drawSkinnedMesh = false;
}
