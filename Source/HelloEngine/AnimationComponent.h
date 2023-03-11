#pragma once
#include "Component.h"

struct ResourceAnimation;


class Animation;

class AnimationComponent : public Component
{
public:
	AnimationComponent(GameObject* gameObject);
	~AnimationComponent();

	void ChangeAnimation(uint animUID);

	void PlayAnimation();
	void StopAnimation();
	void ResumeAnimation();
	void PauseAnimation();

	void UpdateAnimation();

	void Serialization(json& j) override;
	void DeSerialization(json& j) override;

	bool IsPlaying() { return isPlaying; };

	void OnEditor() override;

private:
	void AnimationDropArea();
	
	bool GetLoop(bool value) { return isLoop; }
	void SetLoop(bool value) { isLoop = value; }
	void SetStayLast(bool value) { isStayLast = value; }
	bool GetStayLast(bool value) { return isStayLast; }

private:

	ResourceAnimation* _resource = nullptr;

	float animDuration = 0;
	float currentTime = 0;
	float speedMultiplier = 1;

	std::vector<float4x4> finalBoneMatrices;
	Animation* currentAnimation = nullptr;


	bool isPlaying;
	bool isPaused;
	bool isLoop;
	bool isStayLast;
};

