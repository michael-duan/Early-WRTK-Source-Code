// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "ThumbTurnBehavior.h"

#include "Pawn/WRTK_InteractManager.h"
#include "Pawn/WRTK_HandActor.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "GrabSphereCollider.h"
#include "WRTK_Math.h"

#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

void UThumbTurnBehavior::SearchForKnownComponents()
{
	Super::SearchForKnownComponents();

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto& c : comps)
	{
		auto sc = Cast<USceneComponent>(c);
		if (sc == nullptr) //if component is not a scenecomponent
		{
			continue;
		}

		if (sc->GetName() == ThumbEdgeMarkerName)
		{
			ThumbEdgeMarker = sc;
		}
		else if (sc->GetName() == BaseName)
		{
			Base = sc;
		}
		else if (sc->GetName() == CircleNormalMarkerName)
		{
			CircleNormalMarker = sc;
		}
	}
}

void UThumbTurnBehavior::PostOnGrab(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnGrab(im, shape);

	OnAttach(im, shape);

	im->HandAnimInstance->SetHoldingObject(true, shape->GetGrabAnim());

}

void UThumbTurnBehavior::PostOnPinch(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnPinch(im, shape);

	OnAttach(im, shape);

	im->HandAnimInstance->SetHoldingObject(true, shape->GetPinchAnim());

}

void UThumbTurnBehavior::PostOnGrabRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnGrabRelease(im, shape);

	OnDetach(im, shape);
}

void UThumbTurnBehavior::PostOnPinchRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnPinchRelease(im, shape);

	OnDetach(im, shape);
}

void UThumbTurnBehavior::OnAttach(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	FVector circleNormal = CircleNormalMarker->GetForwardVector();
	FVector unprojectedRight = GetStrongestInteractor()->HandActor->GrabSphereComponent->GetRightVector();
	OnGrabRightVector = WRTK_Math::ProjectVectorOntoPlane(unprojectedRight, circleNormal);

	if (bShouldAttachHand)
	{
		im->HandActor->AttachDummyHandToComponent(true, Cast<USceneComponent>(AffectedComponents[0]), FAttachmentTransformRules::KeepWorldTransform);
	}
}

void UThumbTurnBehavior::OnDetach(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	if (bShouldAttachHand)
		im->HandActor->AttachDummyHandToComponent(false, Cast<USceneComponent>(AffectedComponents[0]), FAttachmentTransformRules::KeepWorldTransform);

	im->HandAnimInstance->SetHoldingObject(false, shape->GetPinchAnim());

}

// Called every frame
void UThumbTurnBehavior::BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::BehaviorTickComponent(DeltaTime, TickType, ThisTickFunction);

	//if not held, do nothing
	if (IsHeld() == false)
	{
		return;
	}

	if (ThumbEdgeMarker == nullptr || Base == nullptr)
	{
		return;
	}
	
	FVector circleNormal = CircleNormalMarker->GetForwardVector();

	//if held, move with controller (currently not with respect to deltatime)
	FVector thumbTurnBaseLoc = Base->GetComponentLocation();
	FVector thumbEdgeMarkerLoc = ThumbEdgeMarker->GetComponentLocation();

	//project hand's right-vector onto dial's plane
	FVector currRightVec = GetStrongestInteractor()->HandActor->GrabSphereComponent->GetRightVector();
	FVector projectedRightVec = WRTK_Math::ProjectVectorOntoPlane(currRightVec, circleNormal);

	/*
	//for debugging to make sure our projectedRightVec is correct
	projectedRightVec.Normalize();
	float thumbTurnRadius = (thumbEdgeMarkerLoc - thumbTurnBaseLoc).Size();
	FVector thumbTurnEdgeGoalLoc = Base->GetComponentLocation() + projectedRightVec * thumbTurnRadius;
	DrawDebugSphere(GetWorld(), thumbTurnEdgeGoalLoc, 3.0f, 5, FColor::Red);
	*/

	FVector finalRightVec;
	//rotate the thumb turn
	if (ShouldSnapToHand == true)
	{
		//align projected vector with thumb turn orientation
		finalRightVec = projectedRightVec;
	}
	else
	{
		//get the angle between projectedRightVec and OnGrabRightVec, rotate OnGrabRightVector by that angle
		//also rotate the base's rightvector by that angle and use that to set the new rotation
		float angleBetweenOldAndNewRightVec = WRTK_Math::GetSignedAngleBetweenTwoVectors(projectedRightVec, OnGrabRightVector, circleNormal);
		OnGrabRightVector = OnGrabRightVector.RotateAngleAxis(angleBetweenOldAndNewRightVec, circleNormal);
		finalRightVec = Base->GetRightVector().RotateAngleAxis(angleBetweenOldAndNewRightVec, circleNormal);

		//for debugging
		//UE_LOG(LogTemp, Warning, TEXT("angleBetweenOldAndNewRightVec is %f"), angleBetweenOldAndNewRightVec);
	}

	FVector projectedForwardVec = finalRightVec.RotateAngleAxis(-90.0f, Base->GetUpVector());
	FRotator rot = UKismetMathLibrary::MakeRotationFromAxes(projectedForwardVec, finalRightVec, Base->GetUpVector());
	Base->SetWorldRotation(rot);
}