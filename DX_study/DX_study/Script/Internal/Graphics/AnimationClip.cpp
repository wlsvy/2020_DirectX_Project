#include "AnimationClip.h"

void AnimationClip::HierarchyBoneAnim(
	const BoneChannel & bone, 
	float time, 
	const DirectX::XMMATRIX & parentTransform, 
	std::vector<bool> & check, 
	std::vector<DirectX::XMMATRIX> & result)
{
	check[bone.mBoneIndex] = false;

	DirectX::XMMATRIX posMat = DirectX::XMMatrixTranslationFromVector(bone.positionInterpolate(time));
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationQuaternion(bone.rotationInterpolate(time));
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScalingFromVector(bone.scaleInterpolate(time));

	DirectX::XMMATRIX boneTransform = scaleMat * rotMat * posMat;
	DirectX::XMMATRIX boneGlobalMatrix = boneTransform * parentTransform;
	DirectX::XMMATRIX FinalMatrix = bone.mBoneOffset * boneGlobalMatrix;

	result[bone.mBoneIndex] = FinalMatrix;

	for (auto i : bone.mChildBoneIndex) {
		if (check[i])
			HierarchyBoneAnim(mChannel[i], time, boneGlobalMatrix, check, result);
	}
}

void AnimationClip::GetResultInTime(float time, std::vector<DirectX::XMMATRIX> & result)
{
	float TimeInTicks = time * mTickPerSecond;
	float AnimationTime = fmod(TimeInTicks, mDuration);

	result.resize(mNumChannel);

	std::vector<bool> check(mChannel.size(), true);
	for (int i = 0; i < mNumChannel; i++) {
		if (check[i])
			HierarchyBoneAnim(mChannel[i], AnimationTime, DirectX::XMMatrixIdentity(), check, result);
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
	assert(nextKeyIndex < mNumPositionKeys);

	float keyDuration = (float)(mPositionKeys[nextKeyIndex].mTime - mPositionKeys[keyIndex].mTime);
	float factor = (time - (float)mPositionKeys[keyIndex].mTime) / keyDuration;
	assert(factor >= 0.0f && factor <= 1.0f);

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
	assert(nextKeyIndex < mNumRotationKeys);

	float keyDuration = (float)(mRotationKeys[nextKeyIndex].mTime - mRotationKeys[keyIndex].mTime);
	float factor = (time - (float)mRotationKeys[keyIndex].mTime) / keyDuration;
	assert(factor >= 0.0f && factor <= 1.0f);

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
	assert(nextKeyIndex < mNumScaleKeys);

	float keyDuration = (float)(mScaleKeys[nextKeyIndex].mTime - mScaleKeys[keyIndex].mTime);
	float factor = (time - (float)mScaleKeys[keyIndex].mTime) / keyDuration;
	assert(factor >= 0.0f && factor <= 1.0f);

	DirectX::XMVECTOR from = DirectX::XMLoadFloat3(&mScaleKeys[keyIndex].mScale);
	DirectX::XMVECTOR to = DirectX::XMLoadFloat3(&mScaleKeys[nextKeyIndex].mScale);
	return DirectX::XMVectorLerp(from, to, factor);
}
