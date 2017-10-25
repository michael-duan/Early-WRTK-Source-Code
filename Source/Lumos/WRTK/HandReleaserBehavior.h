// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "HandReleaserBehavior.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddHapticFeedbackSignature_HandReleaser, float, amplitudeToAdd);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUMOS_API UHandReleaserBehavior : public UBehaviorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHandReleaserBehavior();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void SearchForKnownComponents() override;

public:
	//the name of the shape that will cause the hand to release when the hand leaves the shape
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HandReleaser)
	FString ReleaseShapeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HandReleaser)
	float AmplitudeDampingFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HandReleaser)
	float Frequency;

	virtual void PostOnGrab(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinch(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnGrabRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinchRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	void OnAttach(class UWRTK_InteractManager*, class IGrabShapeInterface*);
	void OnDetach(class UWRTK_InteractManager*, class IGrabShapeInterface*);

protected:
	UPrimitiveComponent* ReleaseShape;

protected:
	UFUNCTION()
	void TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	//the maximum distance expected between the controller and the dummy hand (used for scaling the ghost hand and haptic effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HandReleaser)
	float MaxExpectedHandDistance = 25.0f;

	//the minimum distance between the hand and the edge of the release shape (normalized 0-1) to show the ghost hands
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HandReleaser)
	float MinHandDistanceToShowGhostHands = 0.5f;

	//the amount we should subtract from the ghost hand opacity (normalized 0-1; typically equal to MinHandDistanceToShowGhostHands)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HandReleaser)
	float GhostHandOpacitySubtractAmount = 0.5f;

public:
	//the minimum distance between the hand and the edge of the release shape (normalized 0-1) to play a haptic effect (i.e. a bottom threshold on vibration)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HandReleaser)
	float MinHandDistanceToPlayEffect = 0.5f;

	//the amount we should subtract from the percentage used for the haptic effect (normalized 0-1; typically equal to MinHandDistanceToPlayEffect)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HandReleaser)
	float HapticPercentageSubtractAmount = 0.5f;

	UPROPERTY(BlueprintAssignable, Category = HandReleaser)
	FOnAddHapticFeedbackSignature_HandReleaser OnAddHapticFeedback;
};
