#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "../Core/Object.h"

struct PositionKey {
	DirectX::XMFLOAT3 mPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	float mTime = 0.0f;
};

struct RotationKey {
	DirectX::XMFLOAT4 mQuaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	float mTime = 0.0f;
};

struct ScaleKey {
	DirectX::XMFLOAT3 mScale = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	float mTime = 0.0f;
};

struct BoneChannel {
	std::string mChannelName = "";
	int mChannelIndex = -1;
	int mBoneIndex = -1;

	int mNumPositionKeys = 0;
	int mNumRotationKeys = 0;
	int mNumScaleKeys = 0;

	short mNumChildBone = 0;

	std::vector<PositionKey> mPositionKeys;
	std::vector<RotationKey> mRotationKeys;
	std::vector<ScaleKey> mScaleKeys;
	
	std::vector<int> mChildBoneIndex;

	DirectX::XMMATRIX mParentNodeTransform;
	DirectX::XMMATRIX mBoneOffset;
	DirectX::XMMATRIX mGlobalInverseTransform;

public:
	void positionInterpolate(DirectX::XMVECTOR & _result, float _animTime);
	void rotationInterpolate(DirectX::XMVECTOR & _result, float _animTime);
	void scaleInterpolate(DirectX::XMVECTOR & _result, float _animTime);
};

class AnimationClip : public Object {
	MANAGED_OBJECT(AnimationClip)
public:
	void GetResultInTime(float _time, std::vector<DirectX::XMMATRIX> * _result);

	short mNumChannel;
	std::vector<BoneChannel> mChannel;

	float mDuration;
	float mTickPerSecond;

private:
	void HierarchyBoneAnim(
		BoneChannel * _bone, 
		float _animTime, 
		DirectX::XMMATRIX & _parentTransform, 
		std::vector<bool> & _check,
		std::vector<DirectX::XMMATRIX> * _result);
};