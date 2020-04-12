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
	float m_MoveSpeed = 6.0f;
	float m_RunSpeed = 30.0f;
	float m_RotateSpeed = 300.0f;
};