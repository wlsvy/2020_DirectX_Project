#include "Animator.h"

#include "Renderable.h"
#include "../Internal/Graphics/AnimationClip.h"
#include "../Internal/Graphics/imGui/imgui.h"
#include "../Internal/Core/InternalHelper.h"
#include "../Internal/Core/GameObject.h"
#include "../Util/Time.h"

void Animator::Awake() {
	m_GameObject->GetRenderer().Anim = m_GameObject->GetComponent<Animator>();
}

void Animator::Update()
{
	if (!m_Clip ||
		!m_IsRunning) 
	{
		return;
	}
	if (m_IsBlending) {
		float blendFactor = m_BlendClipPlayTime / m_BlendTime;
		AnimationClip::BlendAnimation(m_Clip, m_BlendClip, m_BlendClipPlayTime, m_PlayTime, 1 - blendFactor, m_AnimResult);
		m_BlendClipPlayTime += Time::GetDeltaTime() * Speed;

		if (m_BlendClipPlayTime > m_BlendTime) {
			m_IsBlending = false;
			m_PlayTime = m_BlendClipPlayTime;
			m_BlendClipPlayTime = 0.0f;
		}
	}
	else {
		m_Clip->GetResultInTime(m_PlayTime, m_AnimResult);
	}

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

void Animator::ChangeClip(const std::shared_ptr<AnimationClip>& target, float blendTime)
{
	if (m_IsBlending) {
		m_PlayTime = m_BlendClipPlayTime;
		m_BlendClipPlayTime = (1 - m_BlendClipPlayTime / m_BlendTime) * blendTime;
	}

	m_BlendClip = m_Clip;
	m_Clip = target;

	m_BlendTime = blendTime;
	m_IsBlending = true;
}

void Animator::SetClip(const std::shared_ptr<AnimationClip>& clip)
{
	m_Clip = clip;
	m_IsBlending = false;
	m_PlayTime = 0.0f;
	m_AnimResult.resize(clip->mNumChannel);
}

void Animator::OnGui()
{
	ImGui::Text("Clip : ");
	ImGui::SameLine();
	ImGui::Text(m_Clip->Name.c_str());

	if (m_IsBlending) {
		ImGui::Text("Blending Clip : ");
		ImGui::SameLine();
		ImGui::Text(m_BlendClip->Name.c_str());
		ImGui::Text(("Blending Time : " + std::to_string(m_BlendClipPlayTime) +  " - " + std::to_string(m_BlendClipPlayTime / m_BlendTime)).c_str());
	}
}