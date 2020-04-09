#include "Animator.h"

#include "../Internal/Graphics/AnimationClip.h"
#include "../Internal/Core/InternalHelper.h"
#include "../Internal/Core/GameObject.h"
#include "../Util/Time.h"

void Animator::Update()
{
	if (!Clip ||
		!m_IsRunning) 
	{
		return;
	}

	Clip->GetResultInTime(m_PlayTime, m_AnimResult);

	m_PlayTime += Time::GetDeltaTime() * Speed;
}

void Animator::Play()
{
	m_IsRunning = true;
}

void Animator::Stop()
{
	m_IsRunning = false;
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