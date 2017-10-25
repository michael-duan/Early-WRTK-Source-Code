// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "HapticFeedbackBehavior.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUMOS_API UHapticFeedbackBehavior : public UBehaviorComponent
{
	GENERATED_BODY()
	
public:
	UHapticFeedbackBehavior();

protected:
	virtual void BeginPlay() override;

protected:
	virtual void BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	void PostOnGrab(class UWRTK_InteractManager* im, class IGrabShapeInterface* gs) override;
	void PostOnGrabRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* gs) override;
	void PostOnPinch(class UWRTK_InteractManager* im, class IGrabShapeInterface*) override;
	void PostOnPinchRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* gs) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HapticFeedback)
	class UHapticFeedbackEffect_Base* DefaultHapticEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HapticFeedback)
	float DefaultFrequency = 1.0f;

	//this will be the base value of CurrentFrameAmplitude
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HapticFeedback)
	float InitialFrameAmplitude = 0.0f;

	//this will help determine the scale of the effect in PlayHapticEvent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HapticFeedback)
	float BaseEventAmplitude = 1.0f;

	//amplitude increment for grab
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HapticFeedback)
		float grabAmplitudeIncrement = 1.0f;

	//amplitude increment for pinch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HapticFeedback)
		float pinchAmplitudeIncrement = 1.0f;

protected:
	float CurrentFrameAmplitude = -1.0f;

public:
	//amplitudeModifier is multiplied by the behavior's BaseAmplitude to determine the scale of the effect
	void PlayHapticEvent(float amplitudeModifier = 1.0f, class UHapticFeedbackEffect_Base* hapticEffect = nullptr);

	UFUNCTION(BlueprintCallable, Category = HapticFeedback)
	void PlayHapticEventSimple(float amplitudeModifier) { PlayHapticEvent(FMath::Abs(amplitudeModifier), nullptr); }

	UFUNCTION(BlueprintCallable, Category = HapticFeedback)
	void AddHapticCurrentFrameAmplitude(float amplitudeToAdd);

protected:
	void playHapticEffectWithFrequencyAndAmplitude(UWRTK_InteractManager* im, float frequency, float amplitude);
};
