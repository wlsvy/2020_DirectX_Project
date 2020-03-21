#pragma once
#include "Behaviour.h"

class AnimationClip;

class Animator : public Behaviour {
	friend class Engine;
	friend class AnimationManager;
public:
	using ComponentTag = AnimationComponentTag;

	Animator(GameObject_v2 & obj);
	void Update(float _deltaTime);
	void Play();
	void Stop();
	void SetAnimClip(AnimationClip * _clip);
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