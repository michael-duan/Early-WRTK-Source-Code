// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "SegmentConstraintBehavior.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddHapticFeedbackSignature_SegmentConstraint, float, amplitudeToAdd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeaveEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReachEnd);

UCLASS(ClassGroup = (WRTK), meta = (BlueprintSpawnableComponent))
class LUMOS_API USegmentConstraintBehavior : public UBehaviorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

public:
	// name of the starting waypoint
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = SegmentConstraint)
	FString StartWaypointName;

	// name of the ending waypoint
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = SegmentConstraint)
	FString EndWaypointName;

	// if true, it will snap to the starting waypoint
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = SegmentConstraint)
	bool bShouldSweep = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = SegmentConstraint)
	bool bShouldSnapToStartWaypoint;

	class AActor* WaypointOwningActor;
	class USegmentWaypoint* CurrentStartWaypoint;
	class USegmentWaypoint* CurrentEndWaypoint;

	UFUNCTION(BlueprintCallable, Category = SegmentConstraint)
	float GetPercentTravelled();

protected:
	virtual void BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void PostOnGrab(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnGrabRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinch(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinchRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	
protected:
	void ConstrainAndSimulatePhysics(int componentIndex, float DeltaTime);

	class UWRTK_InteractManager* SavedIM;

	bool bIsBeingGrabbed;
	bool bIsBeingPinched;

	FVector FindClosestPointOnLine(FVector StartWaypoint, FVector EndWaypoint, FVector Point);

	FVector CurrentLocation;
	FVector TargetLocation;
	float SizeShouldTravel;

public:
	virtual void SearchForKnownComponents() override;

public:
	//if true, the object will snap to the hand; otherwise the object will move with the hand as if the initial grab position snaps to the hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SegmentConstraint)
	bool ShouldSnapToHand = false;

private:
	//on grab/pinch, this is the position at which the valve was grabbed/pinched relative to the object; afterward it moves relative to the controller unless ShouldSnapToHand == true
	USceneComponent* grabLocMarker;

public:
	//whether the affected component should continue sliding along the segment upon release based on a velocity derived from its previous positions
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = SegmentConstraint)
	bool ShouldMaintainVelocityOnRelease = true;

	//the lower this value is, the more the affected component's simulated velocity will be dampened, but it should be between 0 and 1
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = SegmentConstraint)
	float FrictionMultiplier = 0.99f;


	//velocity of object in the last frame
	UPROPERTY(BlueprintReadOnly, Category = SegmentConstraint)
	TArray<FVector> lastFrameVelocities;

	UPROPERTY(BlueprintReadOnly, Category = SegmentConstraint)
	float speedSquared;

protected:
	//relative position of the object in the last frame
	TArray<FVector> lastFramePositions;

	void initializeLastFramePositionsAndVelocities();

public:
	//use this to interpolate the object's position while grabbing (lower is slower)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = SegmentConstraint)
	float InterpSpeed = 1.0f;

public:
	UPROPERTY(EditDefaultsOnly, Category = SegmentConstraint)
	bool bShouldAttachHand = true;

	UPROPERTY(BlueprintAssignable, Category = SegmentConstraint)
	FOnAddHapticFeedbackSignature_SegmentConstraint OnAddHapticFeedback;

	UPROPERTY(BlueprintAssignable, Category = SegmentConstraint)
	FOnLeaveEnd OnLeaveEnd;

	UPROPERTY(BlueprintAssignable, Category = SegmentConstraint)
	FOnReachEnd OnReachEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SegmentConstraint)
	bool ShouldRotateWithHand = false;

	void OnAttach(UWRTK_InteractManager* im, class IGrabShapeInterface* coll);

	void OnDetach(UWRTK_InteractManager* im, class IGrabShapeInterface* coll);

public:
	//the upper limit of the expected speed used for getNormalizedSpeed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SegmentConstraint)
	float SpeedExpectedUpperBound = 25.0f;

protected:
	//used for haptics and volume control
	float getNormalizedSpeed(float componentIndex);

public:
	//note: when we implement wwise properly we will only need one of these names
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SegmentConstraintAudio)
	FString AudioComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SegmentConstraintAudio)
	FString AkComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SegmentConstraintAudio)
	float BaseVolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SegmentConstraintAudio)
	float VolumeChangeInterpSpeed = 1.0f;

protected:
	//note: when we implement wwise properly we will only need one of these components
	class UAudioComponent* audioComponent;
	class UAkComponent* akComponent;

private:
	bool bReachEnd = false;
	bool bReachStart = false;
};
