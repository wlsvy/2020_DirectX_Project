#pragma once
#include "Behaviour.h"

class AnimationClip;

class Animator : public Behaviour {
	friend class Engine;
	friend class AnimationManager;
public:
	Animator(const COMPONENT_INIT_DESC & desc);
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
	bool mIsRunning = false;
};