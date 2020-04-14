#include "CharacterMove.h"

#include "../../Util/Input.h"
#include "../../Util/Time.h"
#include "../Transform.h"
#include "../Animator.h"
#include "../../Internal/Core/GameObject.h"
#include "../../Internal/Core/ObjectPool.h"
#include "../../Internal//Graphics/AnimationClip.h"

using DirectX::operator*;

void CharacterMove::Update() {

	bool isMove = false;

	if (Input::GetKey(Input::LEFT_ARROW)) {
		m_Transform->rotate(0.0f, -m_RotateSpeed * Time::GetDeltaTime(), 0.0);
		isMove = true;
	}
	if (Input::GetKey(Input::RIGHT_ARROW)) {
		m_Transform->rotate(0.0f, m_RotateSpeed * Time::GetDeltaTime(), 0.0);
		isMove = true;
	}
	if (Input::GetKey(Input::UP_ARROW)) {
		m_Transform->translate(m_Transform->GetBackwardVector() * m_MoveSpeed * Time::GetDeltaTime());
		isMove = true;
	}
	if (Input::GetKey(Input::DOWN_ARROW)) {
		m_Transform->translate(m_Transform->GetForwardVector() * m_MoveSpeed * Time::GetDeltaTime());
		isMove = true;
	}

	if (isMove) {
		if (m_Anim.lock()->Clip == m_IdleClip.lock()) {
			m_Anim.lock()->Clip = m_WalkClip.lock();
		}
	}
	else {
		if (m_Anim.lock()->Clip == m_WalkClip.lock()) {
			m_Anim.lock()->Clip = m_IdleClip.lock();
		}
	}
}

void CharacterMove::Init()
{
	m_Transform = &m_GameObject->GetTransform();
	m_Anim = m_GameObject->GetComponent<Animator>();
	m_RunClip = Pool::Find<AnimationClip>("Y Bot_Running");
	m_IdleClip = Pool::Find<AnimationClip>("Y Bot_Idle");
	m_WalkClip = Pool::Find<AnimationClip>("Y Bot_Walking");
}