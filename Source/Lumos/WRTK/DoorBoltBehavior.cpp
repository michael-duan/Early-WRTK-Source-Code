// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "DoorBoltBehavior.h"


#include "WRTK/Pawn/WRTK_HandActor.h"
#include "WRTK/Pawn/WRTK_InteractManager.h"
#include "WRTK/GrabSphereCollider.h"
#include "WRTK/GrabbableComponent.h"
#include "WRTK/LeverBehavior.h"
#include "SegmentConstraintBehavior.h"
#include "WRTK/WRTK_Math.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

void UDoorBoltBehavior::BeginPlay()
{
	Super::BeginPlay();
	bool foundControlBolt = false;
	bool foundOtherBolt = false;
	bool foundControlBoltBehavior = false;
	bool foundOtherBoltBehavior = false;
	bool foundGrabbable = false;
	bool foundDoorBehavior = false;

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto& c : comps)
	{
		//auto sc = Cast<USceneComponent>(c);
		auto sc = c;
		if (sc == nullptr)
		{
			continue;
		}
		if (sc->GetName() == ControlBoltName)
		{
			foundControlBolt = true;
			ControlBolt = Cast<USceneComponent>(sc);
			if (foundOtherBolt && foundControlBoltBehavior && foundOtherBoltBehavior && foundGrabbable && foundDoorBehavior)
			{
				break;
			}
		}

		else if (sc->GetName() == OtherBoltName)
		{
			foundOtherBolt = true;
			OtherBolt = Cast<USceneComponent>(sc);
			if (foundControlBolt && foundControlBoltBehavior && foundOtherBoltBehavior && foundGrabbable && foundDoorBehavior)
			{
				break;
			}
		}
		else if (sc->GetName() == ControlBoltBehaviorName)
		{
			foundControlBoltBehavior = true;
			ControlBoltBehavior = Cast<USegmentConstraintBehavior>(sc);
			if (foundControlBolt && foundOtherBolt && foundOtherBoltBehavior && foundGrabbable && foundDoorBehavior)
			{
				break;
			}
		}

		else if (sc->GetName() == GrabbableName)
		{
			foundGrabbable = true;
			Grabbable = Cast<UGrabbableComponent>(sc);
			if (foundControlBolt && foundOtherBolt && foundControlBoltBehavior && foundDoorBehavior)
			{
				break;
			}
		}

		else if (sc->GetName() == OtherBoltBehaviorName)
		{
			foundOtherBoltBehavior = true;
			OtherBoltBehavior = Cast<USegmentConstraintBehavior>(sc);
			if (foundControlBolt && foundOtherBolt && foundControlBoltBehavior && foundGrabbable && foundDoorBehavior)
			{
				break;
			}
		}
		else if (sc->GetName() == DoorBehaviorName)
		{
			foundDoorBehavior = true;
			DoorBehavior = Cast<ULeverBehavior>(sc);
			if (foundControlBolt && foundOtherBolt && foundControlBoltBehavior && foundGrabbable && foundOtherBoltBehavior)
			{
				break;
			}
		}


	}

	if (!(foundControlBolt && foundControlBoltBehavior && foundOtherBolt && foundOtherBoltBehavior && foundGrabbable && foundDoorBehavior))
	{
		UE_LOG(LogTemp, Warning, TEXT("Door in %s is missing components for Bolt Behavior."), *GetOwner()->GetName());
		SetActive(false);
		return;
	}

	InitialPosition = ControlBolt->GetComponentLocation();
}



// Called every frame
void UDoorBoltBehavior::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (isResettingPosition) {
		if (ControlBolt) {
			ControlBolt->SetWorldLocation(FMath::Lerp(ControlBolt->GetComponentLocation(), InitialPosition, DeltaTime * ResetRate));
		}
	}
}

void UDoorBoltBehavior::PostOnGrab(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnGrab(im, shape);
	isResettingPosition = false;

}

void UDoorBoltBehavior::PostOnGrabRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnGrabRelease(im, shape);

	if (!ControlBoltBehavior || !OtherBoltBehavior || !Grabbable || !DoorBehavior)
	{
		return;
	}
	if (ControlBoltBehavior->GetPercentTravelled() < 0.80 || (OtherBoltBehavior->GetPercentTravelled() < 0.80)) {
		isResettingPosition = true;
	}
	else {

		//Grabbable->BehaviorNames.Add(DoorBehaviorName);
		//if (DoorBehavior) {
		//	Grabbable->Behaviors.Add(Cast<UBehaviorComponent>(DoorBehavior));
		//}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));

	}

}


void UDoorBoltBehavior::StopMoving()
{

}