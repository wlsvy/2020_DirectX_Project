#include "AnimationClip.h"

void AnimationClip::Update(float _deltaTime)
{

	
}

void AnimationClip::SkeletonAnimationUpdate()
{
}

void AnimationClip::HierarchyBoneAnim(
	BoneChannel * _bone, 
	float _animTime, 
	DirectX::XMMATRIX & _parentTransform, 
	std::vector<bool> & _check, 
	std::vector<DirectX::XMMATRIX> * _result)
{
	//수식 위험지대
	_check[_bone->BoneIndex] = false;

	DirectX::XMVECTOR bonePos, boneQuat, boneScale;
	_bone->positionInterpolate(bonePos, _animTime);
	_bone->rotationInterpolate(boneQuat, _animTime);
	_bone->scaleInterpolate(boneScale, _animTime);

	DirectX::XMMATRIX posMat, rotMat, scaleMat;
	posMat = DirectX::XMMatrixTranslationFromVector(bonePos);
	rotMat = DirectX::XMMatrixRotationQuaternion(boneQuat);
	scaleMat = DirectX::XMMatrixScalingFromVector(boneScale);

	DirectX::XMMATRIX boneTransform = scaleMat * rotMat * posMat;
	DirectX::XMMATRIX boneGlobalMatrix = boneTransform * _parentTransform;
	DirectX::XMMATRIX FinalMatrix = _bone->BoneOffset * boneGlobalMatrix;
	//m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;

	_result->at(_bone->BoneIndex) = FinalMatrix;

	for (int i = 0; i < _bone->mNumChildBone; i++) {
		int childIndex = _bone->ChildBoneIndices[i];
		if (_check[childIndex])
			HierarchyBoneAnim(&mChannel[childIndex], _animTime, boneGlobalMatrix, _check, _result);
	}
}

void AnimationClip::GetResultInTime(float _time, std::vector<DirectX::XMMATRIX> * _result)
{
	DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();

	float TimeInTicks = _time * mTickPerSecond;
	float AnimationTime = fmod(TimeInTicks, mDuration);

	_result->resize(mNumChannel);

	std::vector<bool> check(mChannel.size(), true);
	for (int i = 0; i < mNumChannel; i++) {
		if (check[i])
			HierarchyBoneAnim(&mChannel[i], AnimationTime, identity, check, _result);
	}
}

void BoneChannel::positionInterpolate(DirectX::XMVECTOR & _result, float _animTime)
{
	if (mNumPositionKeys == 1) {
		_result = DirectX::XMLoadFloat3(&PositionKeys[0].Position);
		return;
	}

	int keyIndex;
	for (int i = 0; i < mNumPositionKeys - 1; i++) {
		if (_animTime < (float)PositionKeys[i + 1].Time) {
			keyIndex = i;
			break;
		}
	}

	int nextKeyIndex = keyIndex + 1;
	assert(nextKeyIndex < mNumPositionKeys);

	float keyDuration = (float)(PositionKeys[nextKeyIndex].Time - PositionKeys[keyIndex].Time);
	float factor = (_animTime - (float)PositionKeys[keyIndex].Time) / keyDuration;
	assert(factor >= 0.0f && factor <= 1.0f);

	DirectX::XMVECTOR from = DirectX::XMLoadFloat3(&PositionKeys[keyIndex].Position);
	DirectX::XMVECTOR to = DirectX::XMLoadFloat3(&PositionKeys[nextKeyIndex].Position);
	_result = DirectX::XMVectorLerp(from, to, factor);
}

void BoneChannel::rotationInterpolate(DirectX::XMVECTOR & _result, float _animTime)
{
	if (mNumRotationKeys == 1) {
		_result = DirectX::XMLoadFloat4(&RotationKeys[0].Quaternion);
		return;
	}

	int keyIndex;
	for (int i = 0; i < mNumRotationKeys - 1; i++) {
		if (_animTime < (float)RotationKeys[i + 1].Time) {
			keyIndex = i;
			break;
		}
	}

	int nextKeyIndex = keyIndex + 1;
	assert(nextKeyIndex < mNumRotationKeys);

	float keyDuration = (float)(RotationKeys[nextKeyIndex].Time - RotationKeys[keyIndex].Time);
	float factor = (_animTime - (float)RotationKeys[keyIndex].Time) / keyDuration;
	assert(factor >= 0.0f && factor <= 1.0f);

	DirectX::XMVECTOR from = DirectX::XMLoadFloat4(&RotationKeys[keyIndex].Quaternion);
	DirectX::XMVECTOR to = DirectX::XMLoadFloat4(&RotationKeys[nextKeyIndex].Quaternion);
	_result = DirectX::XMQuaternionSlerp(from, to, factor);
	_result = DirectX::XMQuaternionNormalize(_result);
}

void BoneChannel::scaleInterpolate(DirectX::XMVECTOR & _result, float _animTime)
{
	if (mNumScaleKeys == 1) {
		_result = DirectX::XMLoadFloat3(&ScaleKeys[0].Scale);
		return;
	}

	int keyIndex;
	for (int i = 0; i < mNumScaleKeys - 1; i++) {
		if (_animTime < (float)ScaleKeys[i + 1].Time) {
			keyIndex = i;
			break;
		}
	}

	int nextKeyIndex = keyIndex + 1;
	assert(nextKeyIndex < mNumScaleKeys);

	float keyDuration = (float)(ScaleKeys[nextKeyIndex].Time - ScaleKeys[keyIndex].Time);
	float factor = (_animTime - (float)ScaleKeys[keyIndex].Time) / keyDuration;
	assert(factor >= 0.0f && factor <= 1.0f);

	DirectX::XMVECTOR from = DirectX::XMLoadFloat3(&ScaleKeys[keyIndex].Scale);
	DirectX::XMVECTOR to = DirectX::XMLoadFloat3(&ScaleKeys[nextKeyIndex].Scale);
	_result = DirectX::XMVectorLerp(from, to, factor);
}
