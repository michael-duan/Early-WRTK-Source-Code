// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RotationMonitor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReachedTargetAngleSignature, float, targetAngle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRotationMonitorReachedTargetSignature, class URotationMonitor*, rotationMonitor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddHapticFeedbackSignature_RotationMonitor, float, amplitudeToAdd);

/*
	This class uses a comparison vector and a SceneComponent to track the angle between the comparison vector and the SceneComponent's forward vector
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUMOS_API URotationMonitor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URotationMonitor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void SearchForKnownComponents();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	bool bIsAsleep;

	USceneComponent* FixedComponent;
	USceneComponent* RotatingComponent;

public:
	/*
		we will use the fixed's component's forward vector to calculate the angle
		it would be useful to use an arrow component for this
		the fixed component MUST NOT be childed to the rotating one
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	FString FixedComponentName;

	/*
		we will use the rotating component's forward and up vectors to calculate the angle
		it would be useful to use an arrow component for this
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	FString RotatingComponentName;

	//if we only care about rotations from -180 to 180 degrees, then this should be 1, but if we want to track cycles (e.g. 3 full clockwise rotations) then this should be higher (e.g. 3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	int32 NumRotationsToTrack = 1;

public:
	//set this to true if we should broadcast every time we hit an angle on DegreeNotificationInterval
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	bool ShouldNotifyOnDegreeInterval = false;

	/*
		if ShouldNotifyOnDegreeInterval == true, then we'll notify as if we reached a target angle
		this should be a positive value
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	int32 DegreeNotificationInterval = 10.0f;

	/*
		add elements if we should broadcast when we hit some target angle (this means we need to check rotation on tick, which might scale expensively)
		these are NOT BOUND from -180 to 180
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	TArray<float> TargetAngles;

	//if true, we will notify when we are at a target angle this frame even if we were also at that target last frame; otherwise we won't
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	bool ShouldNotifySameAngleConsecutively = false;

	//if the current angle is within this threshold of a target angle, we will consider ourselves at that target angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	float CloseEnoughToTargetThreshold = 5.0f;

	//broadcasted when we reach a target angle and holds the target angle (NOT necessarily the actual angle)
	UPROPERTY(BlueprintAssignable, Category = RotationMonitor)
	FOnAddHapticFeedbackSignature_RotationMonitor OnReachedTargetAngle;

	//broadcasted when we reach the target angle and holds this rotation monitor
	UPROPERTY(BlueprintAssignable, Category = RotationMonitor)
	FOnRotationMonitorReachedTargetSignature OnRotationMonitorReachedTarget;

	//calculates the current rotation when called; returns a degree value (this will always be bound from -180 to 180)
	UFUNCTION(BlueprintCallable, Category = RotationMonitor)
	float GetCurrentRotation();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	float HapticBaseStrength = 1.0f;

	UPROPERTY(BlueprintAssignable, Category = RotationMonitor)
	FOnAddHapticFeedbackSignature_RotationMonitor OnAddHapticFeedback;

	UPROPERTY(BlueprintReadOnly, Category = RotationMonitor)
	float lastFrameAngle = 0.0f;

	//the change in angle between the last two frames
	UPROPERTY(BlueprintReadOnly, Category = RotationMonitor)
	float LastFrameAngleChange = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = RotationMonitor)
	float lastTargetHit = 0.0f;

protected:
	UFUNCTION()
	virtual void PostOnReachedTargetAngle(float targetAngle);

	//tracks the number of full clockwise rotations (full counter-clockwise rotations subtract from this total, so this value can be negative)
	int32 clockwiseRotationCounter = 0;

	bool lastFrameAngleIsValid = false;
	bool lastTargetHitIsValid = false;
};
