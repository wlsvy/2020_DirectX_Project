#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>

class AnimationManager;

struct PositionKey {
	DirectX::XMFLOAT3 Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	float Time = 0.0f;
};

struct RotationKey {
	DirectX::XMFLOAT4 Quaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	float Time = 0.0f;
};

struct ScaleKey {
	DirectX::XMFLOAT3 Scale = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	float Time = 0.0f;
};

struct BoneChannel {
	void BoneUpdate();
	void positionInterpolate(DirectX::XMVECTOR & _result, float _animTime);
	void rotationInterpolate(DirectX::XMVECTOR & _result, float _animTime);
	void scaleInterpolate(DirectX::XMVECTOR & _result, float _animTime);

	std::string ChannelName = "";
	int ChannelIndex = -1;
	int BoneIndex = -1;

	int mNumPositionKeys = 0;
	int mNumRotationKeys = 0;
	int mNumScaleKeys = 0;

	short mNumChildBone = 0;

	std::vector<PositionKey> PositionKeys;
	std::vector<RotationKey> RotationKeys;
	std::vector<ScaleKey> ScaleKeys;
	
	std::vector<int> ChildBoneIndices;

	DirectX::XMMATRIX ParentTransform; //좀 특별하게 다뤄야 함
	DirectX::XMMATRIX BoneOffset;
	DirectX::XMMATRIX GlobalInverseTransform;

	
};

class AnimationClip {
	friend class AnimationManager;
public:
	void GetResultInTime(float _time, std::vector<DirectX::XMMATRIX> * _result);

	std::string mClipName;

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