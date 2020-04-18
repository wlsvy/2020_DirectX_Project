#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "../Core/Object.h"

struct PositionKey {
	PositionKey(float x, float y, float z, float time) : mPosition(x, y, z), mTime(time) {}
	DirectX::XMFLOAT3 mPosition;
	float mTime;
};

struct RotationKey {
	RotationKey(float x, float y, float z, float w, float time) : mQuaternion(x, y, z, w), mTime(time) {}
	DirectX::XMFLOAT4 mQuaternion;
	float mTime;
};

struct ScaleKey {
	ScaleKey(float x, float y, float z, float time) : mScale(x, y, z), mTime(time) {}
	DirectX::XMFLOAT3 mScale;
	float mTime;
};

struct BoneChannel {
	std::string mChannelName = "";
	int mBoneIndex = -1;

	short mNumPositionKeys = 0;
	short mNumRotationKeys = 0;
	short mNumScaleKeys = 0;

	short mNumChildBone = 0;

	std::vector<PositionKey> mPositionKeys;
	std::vector<RotationKey> mRotationKeys;
	std::vector<ScaleKey> mScaleKeys;
	
	std::vector<int> mChildBoneIndex;
	DirectX::XMMATRIX mBoneOffset;

public:
	const DirectX::XMVECTOR & positionInterpolate(float time) const;
	const DirectX::XMVECTOR & rotationInterpolate(float time) const;
	const DirectX::XMVECTOR & scaleInterpolate(float time) const;
};

class AnimationClip : public Object {
	MANAGED_OBJECT(AnimationClip)
public:
	static void BlendAnimation(
			const std::shared_ptr<AnimationClip> & clip1,
			const std::shared_ptr<AnimationClip> & clip2,
			float time1,
			float time2,
			float blendFactor,
			std::vector<DirectX::XMMATRIX> & result);

	void GetResultInTime(float time, std::vector<DirectX::XMMATRIX> & result);

	short mNumChannel;
	std::vector<BoneChannel> Channels;

	float mDuration;
	float mTickPerSecond;

private:
	void HierarchyBoneAnim(
		const BoneChannel & channel,
		float animTime, 
		const DirectX::XMMATRIX & parentTransform, 
		std::vector<bool> & check,
		std::vector<DirectX::XMMATRIX> & result);
};