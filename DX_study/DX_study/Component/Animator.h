#pragma once
#include <string>

#include "Behaviour.h"

class AnimationClip;

class Animator : public Behaviour {
	friend class Engine;
	friend class AnimationManager;
public:
	using ComponentTag = AnimationComponentTag;

	Animator(const COMPONENT_INIT_DESC & desc);
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