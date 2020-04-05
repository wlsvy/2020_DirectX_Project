#include "CamMove.h"

#include "../../Util/Input.h"
#include "../../Util/Time.h"
#include "../Transform.h"
#include "../../Internal/Core/GameObject.h"

using DirectX::operator*;

void CamMove::Update()
{
	float dt = Time::GetDeltaTime();
	Transform& tf = m_GameObject->GetTransform();

	if (Input::IsMouseRightDown()) {
		tf.rotate(Input::GetMouseDeltaY() * m_RotateSpeed * dt, Input::GetMouseDeltaX() * m_RotateSpeed * dt, 0.0f);
	}

	float speed = m_MoveSpeed;
	if (Input::GetKey(Input::SHIFT)) {
		speed = m_FastMoveSpeed;
	}

	if (Input::GetKey('W')) {
		tf.translate(tf.GetForwardVector() * speed * dt);
	}
	if (Input::GetKey('S')) {
		tf.translate(tf.GetBackwardVector() * speed * dt);
	}
	if (Input::GetKey('A')) {
		tf.translate(tf.GetLeftVector() * speed * dt);
	}
	if (Input::GetKey('D')) {
		tf.translate(tf.GetRightVector() * speed * dt);
	}
	if (Input::GetKey(Input::SPACE)) {
		tf.translate(0.0f, speed * dt, 0.0f);
	}
	if (Input::GetKey('Z')) {
		tf.translate(0.0f, -speed * dt, 0.0f);
	}
}
