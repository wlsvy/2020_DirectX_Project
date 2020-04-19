#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

#include "Behaviour.h"

class AnimationClip;
class BoneAvatar;

class Animator : public Behaviour {
	MANAGED_OBJECT(Animator)
	COMPONENT_CONSTRUCTOR(Animator, Behaviour)
public:
	void Awake() override;
	void Update() override;
	void Play();
	void Stop();
	bool IsRunning() const { return m_IsRunning; }

	const std::vector<DirectX::XMMATRIX> & GetAnimResult() const { return m_AnimResult; }
	void ChangeClipWithBlending(const std::shared_ptr<AnimationClip>& target, float blendTime);
	void SetClip(const std::shared_ptr<AnimationClip>& clip);
	std::shared_ptr<AnimationClip> GetClip() { return m_Clip; }

	void OnGui() override;

	float Speed = 1.0f;

private:
	std::shared_ptr<AnimationClip> m_Clip;
	std::shared_ptr<AnimationClip> m_BlendClip;
	std::vector<DirectX::XMMATRIX> m_AnimResult;
	float m_PlayTime = 0.0f;
	float m_BlendTime = 0.0f;
	float m_BlendClipPlayTime = 0.0f;
	bool m_IsRunning = false;
	bool m_IsBlending = false;
};