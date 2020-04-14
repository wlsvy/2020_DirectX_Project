#pragma once
#include "../Behaviour.h"

class Animator;
class AnimationClip;
class Transform;

class CharacterMove : public Behaviour {
	BEHAVIOUR_CONSTRUCTOR(CharacterMove)
public:
	void Update() override;
	void Init();

private:
	std::weak_ptr<Animator> m_Anim;
	std::weak_ptr<AnimationClip> m_WalkClip;
	std::weak_ptr<AnimationClip> m_IdleClip;
	std::weak_ptr<AnimationClip> m_RunClip;
	Transform* m_Transform;
	float m_MoveSpeed = 10.1f;
	float m_RunSpeed = 50.5f;
	float m_RotateSpeed = 200.0f;
};