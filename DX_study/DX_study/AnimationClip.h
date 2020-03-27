#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>

class AnimationManager;

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
	//BoneChannel(const BoneChannel & _boneChannel);
	void BoneUpdate();

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

	DirectX::XMMATRIX mParentNodeTransform; //좀 특별하게 다뤄야 함
	DirectX::XMMATRIX mBoneOffset;
	DirectX::XMMATRIX mGlobalInverseTransform;

public:
	void positionInterpolate(DirectX::XMVECTOR & _result, float _animTime);
	void rotationInterpolate(DirectX::XMVECTOR & _result, float _animTime);
	void scaleInterpolate(DirectX::XMVECTOR & _result, float _animTime);
};

class AnimationClip {
	friend class AnimationManager;
public:
	void GetResultInTime(float _time, std::vector<DirectX::XMMATRIX> * _result);

	std::string Name;

	short mNumChannel;
	std::vector<BoneChannel> mChannel;

	float mDuration;
	float mTickPerSecond;

private:
	void Update(float _deltaTime);
	void SkeletonAnimationUpdate();
	void HierarchyBoneAnim(
		BoneChannel * _bone, 
		float _animTime, 
		DirectX::XMMATRIX & _parentTransform, 
		std::vector<bool> & _check,
		std::vector<DirectX::XMMATRIX> * _result);
};