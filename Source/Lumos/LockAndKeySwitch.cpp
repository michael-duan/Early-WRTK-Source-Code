// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "LockAndKeySwitch.h"

#include "OverlapSwitch.h"
#include "WRTK/GrabbableComponent.h"
#include "WRTK/SegmentConstraintBehavior.h"
#include "WRTK/SegmentWaypoint.h"
#include "WRTK/Pawn/WRTK_InteractManager.h"
#include "WRTK/GrabShapeInterface.h"
#include "RotationMonitor.h"
#include "WRTK/PickUpBehaviorComponent.h"

#include "Kismet/KismetMathLibrary.h"

ULockAndKeySwitch::ULockAndKeySwitch() : USwitchListener()
{
	OverlapSwitch = CreateDefaultSubobject<UOverlapSwitch>(TEXT("OverlapSwitch"));
}

void ULockAndKeySwitch::BeginPlay()
{
	Super::BeginPlay();

	if (OverlapSwitch != nullptr)
	{
		OverlapSwitch->ListenerActors.Add(GetOwner());
		OverlapSwitch->ListenerComponents.Add(this->GetName());
	}

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto& c : comps)
	{
		auto sc = Cast<USceneComponent>(c);
		if (sc == nullptr) //if component is not a scenecomponent
		{
			continue;
		}

		if (sc->GetName() == LockUpMarkerName)
		{
			lockUpMarker = sc;
		}
	}
}

void ULockAndKeySwitch::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SegmentConstraintBehavior != nullptr)
	{
		//for debugging
		//UE_LOG(LogTemp, Warning, TEXT("percenttravelled is %f"), SegmentConstraintBehavior->GetPercentTravelled());

		if (SegmentConstraintBehavior->GetPercentTravelled() * 100 >= SegmentPercentMax && ShouldActAsKeycardLock == true)
		{
			//for debugging
			//UE_LOG(LogTemp, Warning, TEXT("percent travelled %f is greater than max %f"), SegmentConstraintBehavior->GetPercentTravelled() * 100, SegmentPercentMax);

			//for now we consider the lock unlocked if the key is far enough in
			//TODO: add functionality for turning key
			isUnlocked = true;

			//SegmentConstraintBehavior->OnGrabRelease(SegmentConstraintBehavior->GetStrongestInteractor(), SegmentConstraintBehavior->GetStrongestInteractionShape());
			OnSwitch(this);
		}
		else if (SegmentConstraintBehavior->GetPercentTravelled() * 100 <= SegmentPercentMin)
		{
			//for debugging
			//UE_LOG(LogTemp, Warning, TEXT("percent travelled %f is less than min %f"), SegmentConstraintBehavior->GetPercentTravelled() * 100, SegmentPercentMin);

			OnKeyLeaveThreshold();
		}
	}
}

void ULockAndKeySwitch::DoPostSwitchEffect(USwitchListener* switchListener)
{
	Super::DoPostSwitchEffect(switchListener);

	if (switchListener == OverlapSwitch)
	{
		if (OverlapSwitch->IsOn()) //if the key entered the overlap collision shape
		{
			TArray<AActor*> overlappers;
			OverlapSwitch->CollisionShape->GetOverlappingActors(overlappers, OverlapSwitch->ClassToTriggerOverlap);
			if (overlappers.Num() == 0)
			{
				return;
			}
			Key = overlappers[overlappers.Num() - 1];
			if (Key == nullptr || Key->GetComponentByClass(UBehaviorComponent::StaticClass()) == nullptr) //the key must have an existing behaviorcomponent for the lock to work
			{
				return;
			}

			//check orientation of key to make sure it is inserted tooth-before-handle
			/*
				TODO
			*/

			//add a segment constraint behavior to the key
			if (SegmentConstraintBehavior == nullptr)
			{
				SegmentConstraintBehavior = NewObject<USegmentConstraintBehavior>(Key);
				SegmentConstraintBehavior->RegisterComponent();
				SegmentConstraintBehavior->WaypointOwningActor = GetOwner();
				SegmentConstraintBehavior->StartWaypointName = StartWaypointName;
				SegmentConstraintBehavior->EndWaypointName = EndWaypointName;
				SegmentConstraintBehavior->ShouldMaintainVelocityOnRelease = false;
				SegmentConstraintBehavior->SearchForKnownComponents();

				//for debugging
				//UE_LOG(LogTemp, Warning, TEXT("made segmentconstraintbehavior %s"), *SegmentConstraintBehavior->GetName());
			}

			if (SegmentConstraintBehavior->CurrentStartWaypoint == nullptr)
			{
				return;
			}
			Key->GetRootComponent()->SetWorldRotation(SegmentConstraintBehavior->CurrentStartWaypoint->GetComponentRotation());			

			Key->DetachRootComponentFromParent();
			auto primComp = Cast<UPrimitiveComponent>(Key->GetRootComponent());
			OriginalParent = primComp->GetAttachParent();
			primComp->AttachToComponent(OverlapSwitch->CollisionShape, FAttachmentTransformRules::KeepWorldTransform);
			primComp->SetSimulatePhysics(false);
			//primComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			//put the key's behaviors (except pickupbehavior) to sleep and and get its interactmanager and grabshape
			TArray<UActorComponent*> comps = Key->GetComponentsByClass(UBehaviorComponent::StaticClass());
			UWRTK_InteractManager* im = nullptr;
			IGrabShapeInterface* grabShape = nullptr;
			for (UActorComponent* c : comps)
			{
				UBehaviorComponent* bc = Cast<UBehaviorComponent>(c);
				if (bc->IsA(USegmentConstraintBehavior::StaticClass()) == false)
				{
					//we need to pass some members from the key's previous behavior to the segmentconstraintbehavior
					im = bc->GetStrongestInteractor();
					grabShape = bc->GetStrongestInteractionShape();
					SegmentConstraintBehavior->AffectedComponents = bc->AffectedComponents;

					if (im == nullptr || grabShape == nullptr)
					{
						return;
					}
				}
			}

			//tell all behaviors currently holding the key to release it
			UActorComponent* comp = Key->GetComponentByClass(UGrabbableComponent::StaticClass());
			UGrabbableComponent* grabbableComp = Cast<UGrabbableComponent>(comp);
			if (grabbableComp != nullptr)
			{
				grabbableComp->Release();
			}

			/*
				TODO:
				currently we use the grabShape to determine whether the segmentconstraint should grab or pinch; this is a bit backwards
				it would be better if the interactmanager tracked whether it was pinching or grabbing so we could access that here
			*/

			grabShape->GetGrabbableComponent()->Behaviors.Add(SegmentConstraintBehavior);

			//constrain the key by grabbing/pinching it
			if (grabShape->GetNoticesFistGrab())
			{
				SegmentConstraintBehavior->OnGrab(im, grabShape);

				//for debugging
				//UE_LOG(LogTemp, Warning, TEXT("Key is %s"), *Key->GetName());
				//UE_LOG(LogTemp, Warning, TEXT("We just called segmentconstraintbehavior's ongrab"));
			}
			else
			{
				SegmentConstraintBehavior->OnPinch(im, grabShape);
			}

			//add a rotation monitor to the key
			if (ShouldActAsKeycardLock == false && rotationMonitor == nullptr)
			{
				rotationMonitor = NewObject<URotationMonitor>(Key);
				rotationMonitor->RegisterComponent();
				rotationMonitor->FixedComponent = lockUpMarker;
				{
					keyUpMarker = NewObject<UArrowComponent>(Key);
					rotationMonitor->RegisterComponent();
					FRotator keyUpRot = UKismetMathLibrary::MakeRotFromZ(Key->GetActorUpVector());
					keyUpMarker->SetWorldRotation(keyUpRot);
					keyUpMarker->AttachToComponent(Key->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
				}
				rotationMonitor->RotatingComponent = keyUpMarker;
				rotationMonitor->TargetAngles.Add(KeyRotateToUnlockAngle);
				rotationMonitor->OnReachedTargetAngle.AddDynamic(this, &ULockAndKeySwitch::OnKeyReachedTargetAngle);
			}
		}
	}
}

void ULockAndKeySwitch::OnKeyLeaveThreshold()
{
	//deactivate the segment constraint behavior and reactivate the others
	if (SegmentConstraintBehavior == nullptr || SegmentConstraintBehavior->GetStrongestInteractionShape() == nullptr)
	{
		return;
	}

	UWRTK_InteractManager* tempIM = SegmentConstraintBehavior->GetStrongestInteractor();
	IGrabShapeInterface* tempIF = SegmentConstraintBehavior->GetStrongestInteractionShape();

	bool shouldGrab = SegmentConstraintBehavior->GetStrongestInteractionShape()->GetNoticesFistGrab();
	if (shouldGrab == true)
	{
		SegmentConstraintBehavior->OnGrabRelease(SegmentConstraintBehavior->GetStrongestInteractor(), SegmentConstraintBehavior->GetStrongestInteractionShape());
	}
	else //we should pinch
	{
		SegmentConstraintBehavior->OnPinchRelease(SegmentConstraintBehavior->GetStrongestInteractor(), SegmentConstraintBehavior->GetStrongestInteractionShape());
	}

	TArray<UActorComponent*> comps = Key->GetComponentsByClass(UBehaviorComponent::StaticClass());
	for (UActorComponent* c : comps)
	{
		UBehaviorComponent* bc = Cast<UBehaviorComponent>(c);

		//TODO FIX bug: the key seems to have more than one segment constraint behavior when we hit this line, why?
		if (bc != SegmentConstraintBehavior)
		{
			bc->IsAsleep = false;
			if (shouldGrab == true)
			{
				bc->OnGrab(tempIM, tempIF);
			}
			else //we should pinch
			{
				bc->OnPinch(tempIM, tempIF);
			}
		}
	}

	//for debugging
	//UE_LOG(LogTemp, Warning, TEXT("about to destroy segmentconstraintbehavior %s"), *SegmentConstraintBehavior->GetName());

	//remove the segment constraint component
	SegmentConstraintBehavior->IsAsleep = true;
	SegmentConstraintBehavior->DestroyComponent();
	SegmentConstraintBehavior = nullptr;

	auto primComp = Cast<UPrimitiveComponent>(Key->GetRootComponent());
	//primComp->DetachFromParent(true);
	primComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	//primComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	primComp->SetSimulatePhysics(true);
}

void ULockAndKeySwitch::OnKeyReachedTargetAngle(float targetAngle)
{
	//unlock the door
	isUnlocked = true;
	OnSwitch(this);
}

	


