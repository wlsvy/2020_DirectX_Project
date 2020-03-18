#include "GroundedTest.h"
#include "../PhysicsManager.h"
#include <reactphysics3d/reactphysics3d.h>

void GroundTest::Start()
{
	chracterController = GetComponent<CharacterController>();
}

void GroundTest::Update()
{
	//movement = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	isGrounded = chracterController->IsGrounded();
	testTime += Time->GetDeltaTime();
	testGrounded = GroundRayCast();

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
	}

	if (!InputKeyboard->KeyIsPressed('J') && !InputKeyboard->KeyIsPressed('K')) {
		movement.x = 0.0f;
	}

	if (InputKeyboard->GetKeyDown('O')) {
		movement.z = 10.0f * Time->GetDeltaTime();
		//InputKeyboard->
		//gameObject->transform.translate(15.0f * Time->GetDeltaTime(), 0.0f, 0.0f);
	}

	if (InputKeyboard->GetKeyDown('L')) {
		movement.z = -10.0f * Time->GetDeltaTime();
		//InputKeyboard->
		//gameObject->transform.translate(15.0f * Time->GetDeltaTime(), 0.0f, 0.0f);
	}

	if (!InputKeyboard->KeyIsPressed('O') && !InputKeyboard->KeyIsPressed('L')) {
		movement.z = 0.0f;
	}

	chracterController->Move(movement);
}

bool GroundTest::GroundRayCast()
{
	DirectX::XMFLOAT3 downPos = gameObject->transform.position;
	downPos.y -= GroundTest_Distance;

	std::list<RaycastResult> testRayresultALL;
	//RaycastResult testRayresult;
	//Physics->Raycast(gameObject->transform.position, downPos, testRayresult);
	Physics->RaycastAll(gameObject->transform.position, downPos, testRayresultALL);

	return testRayresultALL.size() == 2;
}

void GroundTest::OnGui()
{
	ImGui::DragFloat("GroundTest Distance", &GroundTest_Distance, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat("Test Time", &testTime, 0.1f, -19.0f, 23.0f);
	ImGui::Checkbox("IsGrounded", &isGrounded);
	ImGui::Checkbox("testGrounded", &testGrounded);
}
