#include "Animator.h"

#include "../Internal/Graphics/AnimationClip.h"
#include "../Internal/Core/InternalHelper.h"
#include "../Internal/Core/GameObject.h"
#include "../Util/Time.h"

void Animator::Update()
{
	if (mClip == nullptr ||
		!m_IsRunning) return;

	mClip->GetResultInTime(mPlayTime, &mAnimResult);

	mPlayTime += Time::GetDeltaTime();
}

void Animator::Play()
{
	m_IsRunning = true;
}

void Animator::Stop()
{
	m_IsRunning = false;
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
	
	//애니메이션 클립 이름
	//애니메이션 진행 속도 바
	//ImGui::Button
}

Animator::~Animator()
{
	//gameObject->renderer.animator = nullptr;
	//gameObject->renderer.drawSkinnedMesh = false;
}
