#pragma once
#include "../Component/ScriptBehaviour.h"

class GroundTest : public ScriptBehaviour {
public:
	NEVER_USE_CONSTRUCTOR(GroundTest);

	CharacterController * chracterController = nullptr;
	bool isGrounded = false;
	bool testGrounded = false;
	float testTime = 0.0f;
	float GroundTest_Distance = 10.0f;
	DirectX::XMFLOAT3 movement = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	void Start() override;

	void Update() override;

	bool GroundRayCast();

	void OnGui();

};