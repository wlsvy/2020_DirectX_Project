#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

#include "Behaviour.h"

class AnimationClip;
class SkinnedModel;

class Animator : public Behaviour {
	MANAGED_OBJECT(Animator)
	BEHAVIOUR_CONSTRUCTOR(Animator)
public:
	
	void Update();
	void Play();
	void Stop();

	void SetClip(AnimationClip * _clip);
	void SetClip(const std::string & clipName);

	AnimationClip * GetAnimClip();
	std::shared_ptr<SkinnedModel> m_Model;

	bool IsRunning();

	void OnGui() override;

	~Animator();
	
	float mPlayTime = 0.0f;
	std::vector<DirectX::XMMATRIX> mAnimResult;
private:
	AnimationClip * mClip = nullptr;
	bool m_IsRunning = false;
};