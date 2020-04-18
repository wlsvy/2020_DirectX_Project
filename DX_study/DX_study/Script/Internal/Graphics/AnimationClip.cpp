#include "AnimationClip.h"

void AnimationClip::HierarchyBoneAnim(
	const BoneChannel & channel, 
	float time, 
	const DirectX::XMMATRIX & parentTransform, 
	std::vector<bool> & check, 
	std::vector<DirectX::XMMATRIX> & result)
{
	check[channel.mBoneIndex] = false;

	DirectX::XMMATRIX posMat = DirectX::XMMatrixTranslationFromVector(channel.positionInterpolate(time));
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationQuaternion(channel.rotationInterpolate(time));
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScalingFromVector(channel.scaleInterpolate(time));

	DirectX::XMMATRIX boneTransform = scaleMat * rotMat * posMat;
	DirectX::XMMATRIX boneGlobalMatrix = boneTransform * parentTransform;
	DirectX::XMMATRIX FinalMatrix = channel.mBoneOffset * boneGlobalMatrix;

	result[channel.mBoneIndex] = FinalMatrix;

	for (auto i : channel.mChildBoneIndex) {
		if (check[i])
			HierarchyBoneAnim(Channels[i], time, boneGlobalMatrix, check, result);
	}
}

void AnimationClip::GetResultInTime(float time, std::vector<DirectX::XMMATRIX> & result)
{
	float TimeInTicks = time * mTickPerSecond;
	float AnimationTime = fmod(TimeInTicks, mDuration);

	std::vector<bool> check(Channels.size(), true);
	for (int i = 0; i < mNumChannel; i++) {
		if (check[i])
			HierarchyBoneAnim(Channels[i], AnimationTime, DirectX::XMMatrixIdentity(), check, result);
	}
}

const DirectX::XMVECTOR &  BoneChannel::positionInterpolate(float time) const
{
	if (mNumPositionKeys == 1) {
		return DirectX::XMLoadFloat3(&mPositionKeys[0].mPosition);
	}

	int keyIndex;
	for (int i = 0; i < mNumPositionKeys - 1; i++) {
		if (time < (float)mPositionKeys[i + 1].mTime) {
			keyIndex = i;
			break;
		}
	}

	int nextKeyIndex = keyIndex + 1;

	float keyDuration = (float)(mPositionKeys[nextKeyIndex].mTime - mPositionKeys[keyIndex].mTime);
	float factor = (time - (float)mPositionKeys[keyIndex].mTime) / keyDuration;

	DirectX::XMVECTOR from = DirectX::XMLoadFloat3(&mPositionKeys[keyIndex].mPosition);
	DirectX::XMVECTOR to = DirectX::XMLoadFloat3(&mPositionKeys[nextKeyIndex].mPosition);
	return DirectX::XMVectorLerp(from, to, factor);
}

const DirectX::XMVECTOR &  BoneChannel::rotationInterpolate(float time) const
{
	if (mNumRotationKeys == 1) {
		return DirectX::XMLoadFloat4(&mRotationKeys[0].mQuaternion);
	}

	int keyIndex;
	for (int i = 0; i < mNumRotationKeys - 1; i++) {
		if (time < (float)mRotationKeys[i + 1].mTime) {
			keyIndex = i;
			break;
		}
	}

	int nextKeyIndex = keyIndex + 1;

	float keyDuration = (float)(mRotationKeys[nextKeyIndex].mTime - mRotationKeys[keyIndex].mTime);
	float factor = (time - (float)mRotationKeys[keyIndex].mTime) / keyDuration;

	DirectX::XMVECTOR from = DirectX::XMLoadFloat4(&mRotationKeys[keyIndex].mQuaternion);
	DirectX::XMVECTOR to = DirectX::XMLoadFloat4(&mRotationKeys[nextKeyIndex].mQuaternion);
	return DirectX::XMQuaternionNormalize(
		DirectX::XMQuaternionSlerp(from, to, factor));
}

const DirectX::XMVECTOR & BoneChannel::scaleInterpolate(float time) const
{
	if (mNumScaleKeys == 1) {
		return DirectX::XMLoadFloat3(&mScaleKeys[0].mScale);
	}

	int keyIndex;
	for (int i = 0; i < mNumScaleKeys - 1; i++) {
		if (time < (float)mScaleKeys[i + 1].mTime) {
			keyIndex = i;
			break;
		}
	}

	int nextKeyIndex = keyIndex + 1;

	float keyDuration = (float)(mScaleKeys[nextKeyIndex].mTime - mScaleKeys[keyIndex].mTime);
	float factor = (time - (float)mScaleKeys[keyIndex].mTime) / keyDuration;

	DirectX::XMVECTOR from = DirectX::XMLoadFloat3(&mScaleKeys[keyIndex].mScale);
	DirectX::XMVECTOR to = DirectX::XMLoadFloat3(&mScaleKeys[nextKeyIndex].mScale);
	return DirectX::XMVectorLerp(from, to, factor);
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
	DirectX::XMMATRIX FinalMatrix = channel1.mBoneOffset * boneGlobalMatrix;

	result[boneIndex] = FinalMatrix;

	for (auto i : channel1.mChildBoneIndex) {
		if (check[i])
			HierarchyBlendBoneAnim(clip1->Channels[i].mBoneIndex, blendFactor, clip1, clip2, time1, time2, boneGlobalMatrix, check, result);
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
	float clip1Time = fmod(time1 * clip1->mTickPerSecond, clip1->mDuration);
	float clip2Time = fmod(time2 * clip2->mTickPerSecond, clip2->mDuration);

	std::vector<bool> check(clip1->Channels.size(), true);
	for (int i = 0; i < clip1->mNumChannel; i++) {
		if (check[i])
			HierarchyBlendBoneAnim(clip1->Channels[i].mBoneIndex, blendFactor, clip1, clip2, clip1Time, clip2Time, DirectX::XMMatrixIdentity(), check, result);
	}
}