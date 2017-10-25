// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SwitchListener.h"
#include "LockAndKeySwitch.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUMOS_API ULockAndKeySwitch : public USwitchListener
{
	GENERATED_BODY()

public:
	ULockAndKeySwitch();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LockAndKeySwitch)
	class UOverlapSwitch* OverlapSwitch;

public:
	class USegmentConstraintBehavior* SegmentConstraintBehavior;
	class USceneComponent* OriginalParent;

	//name of the start waypoint that the SegmentConstraint will use
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SegmentConstraint)
	FString StartWaypointName;

	//name of the end waypoint that the SegmentConstraint will use
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SegmentConstraint)
	FString EndWaypointName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LockAndKeySwitch)
	bool ShouldActAsKeycardLock = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	FString LockUpMarkerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationMonitor)
	float KeyRotateToUnlockAngle = 180.0f;

protected:
	class URotationMonitor* rotationMonitor;
	class USceneComponent* lockUpMarker;
	class USceneComponent* keyUpMarker;

protected:
	virtual void DoPostSwitchEffect(USwitchListener* switchListener) override;
	
protected:
	AActor* Key;

public:
	//if the key is below this percentage on the SegmentConstraint, it will be released from the constraint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LockAndKeySwitch)
	float SegmentPercentMin = 1.0f;

	//if the key is above this percentage on the SegmentConstraint, it will be considered fully inside the lock
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LockAndKeySwitch)
	float SegmentPercentMax = 70.0f;

protected:
	bool isUnlocked = false;

public:
	virtual bool IsOn() override { return isUnlocked; }

protected:
	void OnKeyLeaveThreshold();
	
	UFUNCTION()
	void OnKeyReachedTargetAngle(float targetAngle);
};