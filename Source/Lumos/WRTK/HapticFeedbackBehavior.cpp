// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "HapticFeedbackBehavior.h"

#include "Haptics/HapticFeedbackEffect_Base.h"
#include "WRTK/Pawn/WRTK_InteractManager.h"
#include "WRTK/Pawn/WRTK_HandActor.h"

UHapticFeedbackBehavior::UHapticFeedbackBehavior() : UBehaviorComponent()
{
	SetTickGroup(TG_PostUpdateWork);
}

void UHapticFeedbackBehavior::BeginPlay()
{
	Super::BeginPlay();

	CurrentFrameAmplitude = InitialFrameAmplitude;
}

void UHapticFeedbackBehavior::BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::BehaviorTickComponent(DeltaTime, TickType, ThisTickFunction);

	//play current frame haptic effect and reset the CurrentFrameAmplitude
	if (IsHeld() == true)
	{
		AWRTK_HandActor* handActor = GetStrongestInteractor()->HandActor;
		EControllerHand handEnum = handActor->HandEnum;
		APlayerController* pc = UGameplayStatics::GetPlayerController(handActor->GetWorld(), 0);
		
		//there are two different ways to play a haptic effect; PlayHapticEffect seems to be the less direct but safer method
		//pc->PlayHapticEffect(DefaultHapticEffect, handEnum, CurrentFrameAmplitude);
		pc->SetHapticsByValue(DefaultFrequency, CurrentFrameAmplitude, handEnum);

		CurrentFrameAmplitude = InitialFrameAmplitude;
	}
}

void UHapticFeedbackBehavior::PlayHapticEvent(float amplitudeModifier, UHapticFeedbackEffect_Base* hapticEffect)
{
	if (IsHeld() == true)
	{
		if (hapticEffect == nullptr)
		{
			hapticEffect = DefaultHapticEffect;
		}

		AWRTK_HandActor* handActor = GetStrongestInteractor()->HandActor;
		EControllerHand handEnum = handActor->HandEnum;
		APlayerController* pc = UGameplayStatics::GetPlayerController(handActor->GetWorld(), 0);
		if (pc != nullptr)
		{
			pc->PlayHapticEffect(hapticEffect, handEnum, BaseEventAmplitude * amplitudeModifier);
		}
	}
}

void UHapticFeedbackBehavior::AddHapticCurrentFrameAmplitude(float amplitudeToAdd)
{
	CurrentFrameAmplitude += amplitudeToAdd;
}

void UHapticFeedbackBehavior::PostOnGrabRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* gs)
{
	Super::PostOnGrabRelease(im, gs);

	CurrentFrameAmplitude = InitialFrameAmplitude;
	playHapticEffectWithFrequencyAndAmplitude(im, DefaultFrequency, CurrentFrameAmplitude);
}

void UHapticFeedbackBehavior::PostOnGrab(class UWRTK_InteractManager* im, class IGrabShapeInterface* gs)
{
	Super::PostOnGrab(im, gs);
	AddHapticCurrentFrameAmplitude(grabAmplitudeIncrement);
}

void UHapticFeedbackBehavior::PostOnPinch(class UWRTK_InteractManager* im, class IGrabShapeInterface* gs)
{
	Super::PostOnPinch(im, gs);
	AddHapticCurrentFrameAmplitude(pinchAmplitudeIncrement);

}

void UHapticFeedbackBehavior::PostOnPinchRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* gs)
{
	Super::PostOnPinchRelease(im, gs);

	CurrentFrameAmplitude = InitialFrameAmplitude;
	playHapticEffectWithFrequencyAndAmplitude(im, DefaultFrequency, CurrentFrameAmplitude);
}

void UHapticFeedbackBehavior::playHapticEffectWithFrequencyAndAmplitude(UWRTK_InteractManager* im, float frequency, float amplitude)
{
	AWRTK_HandActor* handActor = im->HandActor;
	EControllerHand handEnum = handActor->HandEnum;
	APlayerController* pc = UGameplayStatics::GetPlayerController(handActor->GetWorld(), 0);

	pc->SetHapticsByValue(frequency, amplitude, handEnum);
}