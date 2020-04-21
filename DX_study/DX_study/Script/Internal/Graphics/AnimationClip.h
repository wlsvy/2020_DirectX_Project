#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "../Core/Object.h"

class SkinnedModel;

struct PositionKey {
	PositionKey(float x, float y, float z, float time) : Position(DirectX::XMVectorSet(x, y, z, 0.0f)), Time(time) {}
	DirectX::XMVECTOR Position;
	float Time;
};

struct RotationKey {
	RotationKey(float x, float y, float z, float w, float time) : Quaternion(DirectX::XMVectorSet(x, y, z, w)), Time(time) {}
	DirectX::XMVECTOR Quaternion;
	float Time;
};

struct ScaleKey {
	ScaleKey(float x, float y, float z, float time) : Scale(DirectX::XMVectorSet(x, y, z, 0.0f)), Time(time) {}
	DirectX::XMVECTOR Scale;
	float Time;
};

struct BoneChannel {
	std::string ChannelName = "";
	int BoneIndex = -1;

	short NumPositionKeys = 0;
	short NumRotationKeys = 0;
	short NumScaleKeys = 0;

	short NumChildBone = 0;

	std::vector<PositionKey> PositionKeys;
	std::vector<RotationKey> RotationKeys;
	std::vector<ScaleKey> ScaleKeys;
	
	std::vector<int> ChildBoneIndex;
	DirectX::XMMATRIX BoneOffset;

public:
	DirectX::XMVECTOR positionInterpolate(float time) const;
	DirectX::XMVECTOR rotationInterpolate(float time) const;
	DirectX::XMVECTOR scaleInterpolate(float time) const;
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

	short NumChannel;
	std::vector<BoneChannel> Channels;
	std::shared_ptr<SkinnedModel> Avatar;

	float Duration;
	float TickPerSecond;

private:
	void HierarchyBoneAnim(
		const BoneChannel & channel,
		float animTime, 
		const DirectX::XMMATRIX & parentTransform, 
		std::vector<bool> & check,
		std::vector<DirectX::XMMATRIX> & result);
};