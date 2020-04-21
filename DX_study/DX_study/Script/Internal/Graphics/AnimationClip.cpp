#include "AnimationClip.h"

void AnimationClip::HierarchyBoneAnim(
	const BoneChannel & channel, 
	float time, 
	const DirectX::XMMATRIX & parentTransform, 
	std::vector<bool> & check, 
	std::vector<DirectX::XMMATRIX> & result)
{
	check[channel.BoneIndex] = false;

	DirectX::XMMATRIX posMat = DirectX::XMMatrixTranslationFromVector(channel.positionInterpolate(time));
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationQuaternion(channel.rotationInterpolate(time));
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScalingFromVector(channel.scaleInterpolate(time));

	DirectX::XMMATRIX boneTransform = scaleMat * rotMat * posMat;
	DirectX::XMMATRIX boneGlobalMatrix = boneTransform * parentTransform;
	DirectX::XMMATRIX FinalMatrix = channel.BoneOffset * boneGlobalMatrix;

	result[channel.BoneIndex] = FinalMatrix;

	for (auto i : channel.ChildBoneIndex) {
		if (check[i])
			HierarchyBoneAnim(Channels[i], time, boneGlobalMatrix, check, result);
	}
}

void AnimationClip::GetResultInTime(float time, std::vector<DirectX::XMMATRIX> & result)
{
	float TimeInTicks = time * TickPerSecond;
	float AnimationTime = fmod(TimeInTicks, Duration);

	std::vector<bool> check(Channels.size(), true);
	for (int i = 0; i < NumChannel; i++) {
		if (check[i])
			HierarchyBoneAnim(Channels[i], AnimationTime, DirectX::XMMatrixIdentity(), check, result);
	}
}

DirectX::XMVECTOR BoneChannel::positionInterpolate(float time) const
{
	if (NumPositionKeys == 1) {
		return PositionKeys[0].Position;
	}

	int keyIndex;
	for (int i = 0; i < NumPositionKeys - 1; i++) {
		if (time < (float)PositionKeys[i + 1].Time) {
			keyIndex = i;
			break;
		}
	}

	float keyDuration = (float)(PositionKeys[keyIndex + 1].Time - PositionKeys[keyIndex].Time);
	float factor = (time - (float)PositionKeys[keyIndex].Time) / keyDuration;

	return DirectX::XMVectorLerp(
		PositionKeys[keyIndex].Position, 
		PositionKeys[keyIndex + 1].Position,
		factor);
}

DirectX::XMVECTOR BoneChannel::rotationInterpolate(float time) const
{
	if (NumRotationKeys == 1) {
		return RotationKeys[0].Quaternion;
	}

	int keyIndex;
	for (int i = 0; i < NumRotationKeys - 1; i++) {
		if (time < (float)RotationKeys[i + 1].Time) {
			keyIndex = i;
			break;
		}
	}

	float keyDuration = (float)(RotationKeys[keyIndex + 1].Time - RotationKeys[keyIndex].Time);
	float factor = (time - (float)RotationKeys[keyIndex].Time) / keyDuration;

	return DirectX::XMQuaternionNormalize(
		DirectX::XMQuaternionSlerp(
			RotationKeys[keyIndex].Quaternion, 
			RotationKeys[keyIndex + 1].Quaternion,
			factor));
}

DirectX::XMVECTOR BoneChannel::scaleInterpolate(float time) const
{
	if (NumScaleKeys == 1) {
		return ScaleKeys[0].Scale;
	}

	int keyIndex;
	for (int i = 0; i < NumScaleKeys - 1; i++) {
		if (time < (float)ScaleKeys[i + 1].Time) {
			keyIndex = i;
			break;
		}
	}

	float keyDuration = (float)(ScaleKeys[keyIndex + 1].Time - ScaleKeys[keyIndex].Time);
	float factor = (time - (float)ScaleKeys[keyIndex].Time) / keyDuration;

	return DirectX::XMVectorLerp(
		ScaleKeys[keyIndex].Scale, 
		ScaleKeys[keyIndex + 1].Scale, 
		factor);
}



void HierarchyBlendBoneAnim(
	int boneIndex,
	float blendFactor,
	const std::shared_ptr<AnimationClip>& clip1,
	const std::shared_ptr<AnimationClip>& clip2,
	float time1,
	float time2,
	const DirectX::XMMATRIX & parentTransform,
	std::vector<bool> & check,
	std::vector<DirectX::XMMATRIX> & result)
{
	check[boneIndex] = false;
	auto& channel1 = clip1->Channels[boneIndex];
	auto& channel2 = clip2->Channels[boneIndex];

	auto pos1 = channel1.positionInterpolate(time1);
	auto pos2 = channel2.positionInterpolate(time2);
	auto quat1 = channel1.rotationInterpolate(time1);
	auto quat2 = channel2.rotationInterpolate(time2);
	auto scale1 = channel1.scaleInterpolate(time1);
	auto scale2 = channel2.scaleInterpolate(time2);

	DirectX::XMMATRIX posMat = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorLerp(pos1, pos2, blendFactor));
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(quat1, quat2, blendFactor)));
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScalingFromVector(DirectX::XMVectorLerp(scale1, scale2, blendFactor));

	DirectX::XMMATRIX boneTransform = scaleMat * rotMat * posMat;
	DirectX::XMMATRIX boneGlobalMatrix = boneTransform * parentTransform;
	DirectX::XMMATRIX FinalMatrix = channel1.BoneOffset * boneGlobalMatrix;

	result[boneIndex] = FinalMatrix;

	for (auto i : channel1.ChildBoneIndex) {
		if (check[i])
			HierarchyBlendBoneAnim(clip1->Channels[i].BoneIndex, blendFactor, clip1, clip2, time1, time2, boneGlobalMatrix, check, result);
	}
}

void AnimationClip::BlendAnimation(
	const std::shared_ptr<AnimationClip>& clip1,
	const std::shared_ptr<AnimationClip>& clip2,
	float time1,
	float time2,
	float blendFactor,
	std::vector<DirectX::XMMATRIX> & result)
{
	float clip1Time = fmod(time1 * clip1->TickPerSecond, clip1->Duration);
	float clip2Time = fmod(time2 * clip2->TickPerSecond, clip2->Duration);

	std::vector<bool> check(clip1->Channels.size(), true);
	for (int i = 0; i < clip1->NumChannel; i++) {
		if (check[i])
			HierarchyBlendBoneAnim(clip1->Channels[i].BoneIndex, blendFactor, clip1, clip2, clip1Time, clip2Time, DirectX::XMMatrixIdentity(), check, result);
	}
}