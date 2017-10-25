// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "PickUpBehaviorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddHapticFeedbackSignature_PickUp, float, amplitudeToAdd);

UCLASS(ClassGroup = (WRTK), meta = (BlueprintSpawnableComponent))
class LUMOS_API UPickUpBehaviorComponent : public UBehaviorComponent
{
	GENERATED_BODY()
	
public:
	UPickUpBehaviorComponent();

protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	bool ShouldMoveGrabLocationOntoHand = true;

//members closely related to behavior of the object while being grabbed
public:
	//whether the behavior will try to teleport the object to the hand instead of using physics handle pulling for a snappier, more 1-to-1 feel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	bool ShouldTryToTeleportObjectToHand = true;

	//whether we should check if a teleporting object would collide with something in its destination
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	bool ShouldSweepTeleportDestination = true;

	//whether we should interpolate the teleport while teleporting the object (unless we are very close to the hand)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	bool ShouldInterpolateInTeleportMode = true;

	//the translate distance at which we should start completely teleporting instead of interpolating
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	float MaxTeleportWithoutInterpolateDistance = 0.01f;

	//the interp speed when we begin to teleport after we were using the physhandle but just passed the teleport check
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	float TransitionFromPhysHandleToTeleportInterpSpeed = 50.0f;

	//whether the behavior will switch from teleporting to using the physics handle if the object is far away or rotated differently from the hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	bool ShouldUsePhysicsHandleIfObjectIsFarOrRotated = false;

	//the physics handle will be on if the object is at least this far from the hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	float MinPhysicsHandleTranslateDistance = 20.0f;

	//the physics handle will be on if the object is rotated at least this far from the hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	float MinPhysicsHandleRotateDistance = 20.0f;

	//whether we will reset the object's rotation to mirror the controller when we're using the physics handle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	bool ShouldCorrectPhysHandleRotationToMirrorController = false;

	//whether we will reset the object's translation to mirror the controller when we're using the physics handle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	bool ShouldCorrectPhysHandleLocationToMirrorController = false;

	//whether we will check whether there is a path to the hand to choose between teleporting or phys handle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	bool ShouldSweepPathFromObjectToHand = true;

	//if we have no collision shape, then we will use a sphere of this radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	float SweepSphereRadius = 1.0f;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PickUpBehavior)
	bool bShouldResetOriginalTransformOnRelease = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PickUpBehavior)
	bool bShouldRotate = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PickUpBehavior)
	bool bPhysDragTowardsHand = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickUpBehavior)
	bool bShouldAttachHand = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PickUpBehavior)
	FName PhysicsGrabRootBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PickUpBehavior)
	bool bShouldMoveOnlyThisActor = false;

	class USceneComponent* SavedOriginalAttachParent;
	FTransform SavedRelativeTransform;
	class IGrabShapeInterface* SavedColl;

	bool bIsBeingGrabbed = false;
	bool bIsBeingPinched = false;

	USceneComponent* GetTopMostParent();

protected:
	class AWRTK_HandActor* GrabbingHand;

public:
	UPROPERTY(BlueprintAssignable, Category = PickUpBehavior)
	FOnAddHapticFeedbackSignature_PickUp OnAddHapticFeedback;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PickUpBehavior)
	float GrabAndReleaseHapticAmount = 1.0f;


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = HandWorkaround)
	bool LeftHandIsRescaledRightHand = true;

protected:
	virtual void BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PostOnGrab(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnGrabRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinch(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinchRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;

protected:
	void DetachFromHand(UWRTK_InteractManager* im, IGrabShapeInterface* coll);
	void AttachToHand(bool bIsGrab, class AWRTK_HandActor*, class IGrabShapeInterface*);
	
	//tick subroutine used to manage the physics handle behavior
	void useAndManagePhysHandle(float DeltaTime, USceneComponent* sc, float translateDistanceSq, FVector goalPos, FRotator goalRot);

public:
	//the length of time for which we force the use of the physics handle (instead of teleporting) after a picked up object hits something
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PickUpBehavior)
	float ObjectWasHitRecentlyTimerMaxTime = 0.3f;

protected:
	bool objectWasHitRecently = false;
	FTimerHandle objectWasHitRecentlyTimerHandle;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit);

	//note that we hit something recently so that we use the physics handle (and the appropriate timer)
	void NoteRecentHit();

	//called when objectWasHitRecentlyTimerHandle hits zero
	void OnObjectWasHitRecentlyTimer();

protected:
	bool teleportedLastFrame = false;
};
