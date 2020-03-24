#include "GroundedTest.h"
<<<<<<< HEAD
#include "../Physics.h"
#include "../Input.h"
=======
#include "../PhysicsManager.h"
#include <reactphysics3d/reactphysics3d.h>
>>>>>>> parent of cb3481a... refactoring

void GroundTest::Start()
{
	chracterController = GetComponent<CharacterController>();
}

void GroundTest::Update()
{
	isGrounded = chracterController->IsGrounded();
	testTime += Time->GetDeltaTime();
	testGrounded = GroundRayCast();

<<<<<<< HEAD
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
=======
	/*if (testTime > 5.0f) {
		testTime = 0.0f;
		
		
		testGrounded = GroundRayCast();
	}*/
	
	
	//if (!isGrounded) {
	//	movement.y -= 0.1f * Time->GetDeltaTime();
	//		//gameObject->transform.translate(0.0f, -40.0f * Time->GetDeltaTime(), 0.0f);
	//}
	//else movement.y = 0.0f;

	if (InputKeyboard->GetKeyDown('T')) {
		movement.y += 4.0f * Time->GetDeltaTime();
		//gameObject->transform.translate(0.0f, 20.0f * Time->GetDeltaTime(), 0.0f);
	}

	if (InputKeyboard->GetKeyDown('J')) {
		movement.x = 10.0f * Time->GetDeltaTime();
		//InputKeyboard->
		//gameObject->transform.translate(15.0f * Time->GetDeltaTime(), 0.0f, 0.0f);
	}

	if (InputKeyboard->GetKeyDown('K')) {
		movement.x = -10.0f * Time->GetDeltaTime();
		//InputKeyboard->
		//gameObject->transform.translate(15.0f * Time->GetDeltaTime(), 0.0f, 0.0f);
>>>>>>> parent of cb3481a... refactoring
	}

	if (!Input::GetKey('J') && !Input::GetKey('K')) {
		movement.x = 0.0f;
	}

<<<<<<< HEAD
	if (Input::GetKeyDown('O')) {
		movement.z = 10.0f * Timer::GetDeltaTime();
	}

	if (Input::GetKeyDown('L')) {
		movement.z = -10.0f * Timer::GetDeltaTime();
=======
	if (InputKeyboard->GetKeyDown('O')) {
		movement.z = 10.0f * Time->GetDeltaTime();
		//InputKeyboard->
		//gameObject->transform.translate(15.0f * Time->GetDeltaTime(), 0.0f, 0.0f);
	}

	if (InputKeyboard->GetKeyDown('L')) {
		movement.z = -10.0f * Time->GetDeltaTime();
		//InputKeyboard->
		//gameObject->transform.translate(15.0f * Time->GetDeltaTime(), 0.0f, 0.0f);
>>>>>>> parent of cb3481a... refactoring
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
