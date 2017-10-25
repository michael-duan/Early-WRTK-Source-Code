// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "ValveBehavior.h"

#include "Pawn/WRTK_InteractManager.h"
#include "Pawn/WRTK_HandActor.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "GrabSphereCollider.h"
#include "WRTK_Math.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

void UValveBehavior::SearchForKnownComponents()
{
	Super::SearchForKnownComponents();

	//find handle and base locations
	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto& c : comps)
	{
		auto sc = Cast<USceneComponent>(c);
		if (sc == nullptr) //if component is not a scenecomponent
		{
			continue;
		}

		if (sc->GetName() == ValveCircumferenceMarkerName)
		{
			CircumferenceMarker = sc;
		}
		else if (sc->GetName() == ValveBaseName)
		{
			Base = sc;
		}
		else if (sc->GetName() == CircleNormalMarkerName)
		{
			CircleNormalMarker = sc;
		}
	}
}

void UValveBehavior::PostOnGrab(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnGrab(im, shape);

	FVector handLoc = GetStrongestInteractor()->HandActor->GrabSphereComponent->GetComponentLocation();
	FVector markerLoc = WRTK_Math::ProjectPointOntoCircle(Base->GetComponentLocation(), CircumferenceMarker->GetComponentLocation(), CircleNormalMarker->GetForwardVector(), handLoc);

	if (grabLocMarker != nullptr)
	{
		grabLocMarker->DestroyComponent();
		grabLocMarker = nullptr;
	}
	grabLocMarker = NewObject<USceneComponent>(Base);
	grabLocMarker->AttachToComponent(Base, FAttachmentTransformRules::KeepRelativeTransform);
	grabLocMarker->RegisterComponent();
	grabLocMarker->SetWorldLocation(markerLoc);
}

void UValveBehavior::PostOnPinch(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnPinch(im, shape);

	FVector handLoc = GetStrongestInteractor()->HandActor->GrabSphereComponent->GetComponentLocation();
	FVector markerLoc = WRTK_Math::ProjectPointOntoCircle(Base->GetComponentLocation(), CircumferenceMarker->GetComponentLocation(), CircleNormalMarker->GetForwardVector(), handLoc);

	if (grabLocMarker != nullptr)
	{
		grabLocMarker->DestroyComponent();
		grabLocMarker = nullptr;
	}
	grabLocMarker = NewObject<USceneComponent>(Base);
	grabLocMarker->AttachToComponent(Base, FAttachmentTransformRules::KeepRelativeTransform);
	grabLocMarker->RegisterComponent();
	grabLocMarker->SetWorldLocation(markerLoc);
}

// Called every frame
void UValveBehavior::BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::BehaviorTickComponent(DeltaTime, TickType, ThisTickFunction);

	//if not held, do nothing
	if (IsHeld() == false)
	{
		return;
	}

	if (CircumferenceMarker == nullptr || Base == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("valve in %s does not have both a base and a circumference marker"), *GetOwner()->GetName());
		return;
	}

	//if held, project the controller onto the circle
	FVector valveBaseLocation = Base->GetComponentLocation();
	FVector controllerLoc = GetStrongestInteractor()->HandActor->GrabSphereComponent->GetComponentLocation();
	FVector handleLocation = CircumferenceMarker->GetComponentLocation();
	FVector projectedPoint = WRTK_Math::ProjectPointOntoCircle(valveBaseLocation, handleLocation, CircleNormalMarker->GetForwardVector(), controllerLoc);

	//rotate the valve
	FVector projectedForwardVec;
	FVector projectedRightVec;
	if (ShouldSnapToHand == true)
	{
		projectedForwardVec = projectedPoint - valveBaseLocation;
	}
	else
	{
		//if we are rotating relative to the initial grab location marker, find the angle between the projectedPoint and the grabLocMarker on the circle and rotate the valve's forward by that angle
		float angleFromGrabLocToProjPoint = WRTK_Math::GetAngleBetweenTwoPointsOnACircle(valveBaseLocation, CircleNormalMarker->GetForwardVector(),
																							projectedPoint, grabLocMarker->GetComponentLocation());
		projectedForwardVec = Base->GetForwardVector().RotateAngleAxis(angleFromGrabLocToProjPoint, CircleNormalMarker->GetForwardVector());
	}

	projectedRightVec = projectedForwardVec.RotateAngleAxis(90.0f, Base->GetUpVector());
	FRotator rot = UKismetMathLibrary::MakeRotationFromAxes(projectedForwardVec, projectedRightVec, Base->GetUpVector());
	Base->SetWorldRotation(rot);

	/*
	//for debugging
	DrawDebugSphere(GetWorld(), projectedPoint, 8.0f, 5, FColor::Red);
	*/
}