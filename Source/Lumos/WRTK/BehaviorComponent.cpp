// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "BehaviorComponent.h"
#include "Pawn/WRTK_InteractManager.h"
#include "WRTK/GrabShapeInterface.h"
#include "GrabbableComponent.h"

// Sets default values for this component's properties
UBehaviorComponent::UBehaviorComponent():
	MainGrabComp(nullptr),
	SecondaryGrabComp(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	SetTickGroup(TG_PrePhysics);

	SearchForKnownComponents();
}

void UBehaviorComponent::SearchForKnownComponents()
{
	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UGrabbableComponent::StaticClass());
	for (auto& c : comps)
	{
		UGrabbableComponent* grab = Cast<UGrabbableComponent>(c);
		if (grab != nullptr)
		{
			if (grab->GetName().Equals(MainGrabCompName))
			{
				MainGrabComp = grab;
			}
			else if (grab->GetName().Equals(SecondaryGrabCompName))
			{
				SecondaryGrabComp = grab;
			}
		}
	}

	comps.Empty();
	comps = GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName(GRABSHAPE_TAG));
	for (auto& c : comps)
	{
		if (GrabColliderNames.Contains(FName(*c->GetName())))
		{
			IGrabShapeInterface* grabCollider = Cast<IGrabShapeInterface>(c);
			if (grabCollider != nullptr)
			{
				GrabBoxColliders.Add(grabCollider);
			}
		}
	}
}

// Called every frame
void UBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsAsleep == true)
	{
		return;
	}

	BehaviorTickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBehaviorComponent::OnRelease(const WRTK_CommonInfo::ButtonName button, UWRTK_InteractManager* im)
{
	if (IsAsleep == true)
	{
		return;
	}

	PostOnRelease(button, im);
}

void UBehaviorComponent::OnTouch(const WRTK_CommonInfo::ButtonName button, UWRTK_InteractManager* im)
{
	if (IsAsleep == true)
	{
		return;
	}

	PostOnTouch(button, im);
}

void UBehaviorComponent::OnAxis(const WRTK_CommonInfo::ButtonName button, UWRTK_InteractManager* im)
{
	if (IsAsleep == true)
	{
		return;
	}

	PostOnAxis(button, im);
}
void UBehaviorComponent::OnPress(const WRTK_CommonInfo::ButtonName button, UWRTK_InteractManager* im)
{
	if (IsAsleep == true)
	{
		return;
	}

	PostOnPress(button, im);
}

void UBehaviorComponent::OnAnalogMove(const float x, const float y, UWRTK_InteractManager* im)
{
	if (IsAsleep == true)
	{
		return;
	}

	PostOnAnalogMove(x, y, im);
}

//Interface variants begin
void UBehaviorComponent::OnGrab(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	if (IsAsleep == true)
	{
		return;
	}

	GrabbingInteractor = im;
	GrabShape = coll;

	PostOnGrab(im, coll);
}

void UBehaviorComponent::OnGrabRelease(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	if (IsAsleep == true)
	{
		return;
	}

	GrabbingInteractor = nullptr;
	GrabShape = nullptr;

	PostOnGrabRelease(im, coll);
}

void UBehaviorComponent::OnPinch(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	PinchingInteractor = im;
	PinchShape = coll;

	PostOnPinch(im, coll);
}

void UBehaviorComponent::OnPinchRelease(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	PinchingInteractor = nullptr;
	PinchShape = nullptr;

	PostOnPinchRelease(im, coll);
}
//Interface variants end

void UBehaviorComponent::SwitchToUseSecondaryGrabbable()
{
	if (SecondaryGrabComp == nullptr || GrabBoxColliders.Num() == 0) return;

	// garbbable switch
	for (auto GrabBoxCollider : GrabBoxColliders)
	{
		// activate the secondary grabbable
		GrabBoxCollider->SetGrabbableComponent(SecondaryGrabComp);
	}
}

void UBehaviorComponent::SwitchToUseMainGrabbable()
{
	if (MainGrabComp == nullptr || GrabBoxColliders.Num() == 0) return;

	// garbbable switch
	for (auto GrabBoxCollider : GrabBoxColliders)
	{
		// activate the main grabbable
		GrabBoxCollider->SetGrabbableComponent(MainGrabComp);
	}
}

UWRTK_InteractManager * UBehaviorComponent::GetStrongestInteractor() const
{
	if (GrabbingInteractor != nullptr)
	{
		return GrabbingInteractor;
	}
	else
	{
		return PinchingInteractor;
	}
}

IGrabShapeInterface* UBehaviorComponent::GetStrongestInteractionShape() const
{
	if (GrabShape != nullptr)
	{
		return GrabShape;
	}
	else
	{
		return PinchShape;
	}
}
