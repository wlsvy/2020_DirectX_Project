#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

#include "Behaviour.h"

class AnimationClip;

class Animator : public Behaviour {
	BEHAVIOUR_CONSTRUCTOR(Animator)
public:
	
	void Update(float _deltaTime);
	void Play();
	void Stop();

	void SetClip(AnimationClip * _clip);
	void SetClip(const std::string & clipName);

	AnimationClip * GetAnimClip();

	bool IsRunning();

	void OnGui() override;

	~Animator();
	
	float mPlayTime = 0.0f;
	std::vector<DirectX::XMMATRIX> mAnimResult;
private:
	AnimationClip * mClip = nullptr;
	bool m_IsRunning = false;
};