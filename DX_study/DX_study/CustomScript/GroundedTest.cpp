#include "GroundedTest.h"
#include "../Physics.h"
#include "../Input.h"

void GroundTest::Start()
{
	chracterController = GetComponent<CharacterController>();
}

void GroundTest::Update()
{
	isGrounded = chracterController->IsGrounded();
	testTime += Timer::GetDeltaTime();
	testGrounded = GroundRayCast();

	if (Input::GetKeyDown('T')) {
		movement.y = 4.0f * Timer::GetDeltaTime();
	}
	if (Input::GetKeyDown('Y')) {
		movement.y = -4.0f * Timer::GetDeltaTime();
	}

	if (Input::GetKeyDown('J')) {
		movement.x = 10.0f * Timer::GetDeltaTime();
	}

	if (Input::GetKeyDown('K')) {
		movement.x = -10.0f * Timer::GetDeltaTime();
	}

	if (!Input::GetKey('J') && !Input::GetKey('K')) {
		movement.x = 0.0f;
	}

	if (Input::GetKeyDown('O')) {
		movement.z = 10.0f * Timer::GetDeltaTime();
	}

	if (Input::GetKeyDown('L')) {
		movement.z = -10.0f * Timer::GetDeltaTime();
	}

	if (!Input::GetKey('O') && !Input::GetKey('L')) {
		movement.z = 0.0f;
	}

	chracterController->Move(movement);
}

bool GroundTest::GroundRayCast()
{
	DirectX::XMFLOAT3 downPos = GameObject.transform.position;
	downPos.y -= GroundTest_Distance;

	return Physics::Raycast(GameObject.transform.position, downPos);
}

void GroundTest::OnGui()
{
	ImGui::DragFloat("GroundTest Distance", &GroundTest_Distance, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat("Test Time", &testTime, 0.1f, -19.0f, 23.0f);
	ImGui::Checkbox("IsGrounded", &isGrounded);
	ImGui::Checkbox("testGrounded", &testGrounded);
}
