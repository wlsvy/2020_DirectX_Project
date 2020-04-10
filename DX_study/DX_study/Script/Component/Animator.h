#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

#include "Behaviour.h"

class AnimationClip;
class BoneAvatar;

class Animator : public Behaviour {
	MANAGED_OBJECT(Animator)
	BEHAVIOUR_CONSTRUCTOR(Animator)
public:
	void Update();
	void Play();
	void Stop();
	bool IsRunning() const { return m_IsRunning; }
	const std::vector<DirectX::XMMATRIX> & GetAnimResult() const { return m_AnimResult; }
	void OnGui() override;

	std::shared_ptr<AnimationClip> Clip;
	std::shared_ptr<BoneAvatar> Avatar;
	float Speed = 1.0f;

private:
	std::vector<DirectX::XMMATRIX> m_AnimResult;
	float m_PlayTime = 0.0f;
	bool m_IsRunning = false;
};