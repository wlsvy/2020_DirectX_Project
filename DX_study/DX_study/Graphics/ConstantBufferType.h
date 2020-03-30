#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader {
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
	//float xOffset = 0;	//4bytes
	//float yOffset = 0;	//4bytes
};

struct CB_VS_vertexshader_2d {
	DirectX::XMMATRIX wvpMatrix;
};

//8 bytes -> 16 bytes(padding) -> ������ ���� �ݵ�� �ؾ� ��. ���� ��ġ �߿�

struct CB_PS_light {
	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;
	//������� 16����Ʈ

	DirectX::XMFLOAT3 dynamicLightColor;
	float dynamicLightStrength;
	//������� �� 16 ����Ʈ

	DirectX::XMFLOAT3 dynamicLightPosition;
	
	float dynamicLightAttenuation_a;
	float dynamicLightAttenuation_b;
	float dynamicLightAttenuation_c;
};