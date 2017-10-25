// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "GrabbableComponent.h"
#include "Pawn/WRTK_InteractManager.h"
#include "Pawn/WRTK_HandActor.h"
#include "BehaviorComponent.h"

// Sets default values for this component's properties
UGrabbableComponent::UGrabbableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabbableComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto& c : comps)
	{
		for (FString& n : BehaviorNames)
		{
			if (c->GetName() == n && c->IsA(UBehaviorComponent::StaticClass()))
			{
				Behaviors.Add(Cast<UBehaviorComponent>(c));
				break;
			}
		}
		
		for (FString& n : AffectedComponentNames)
		{
			if (c->GetName() == n)
			{
				AffectedComponents.Add(c);
				break;
			}
		}

		for (FString& n : AffectedComponentParentNames)
		{
			if (c->GetName() == n)
			{
				AffectedComponentParents.Add(c);
				break;
			}
		}
	}

	for (auto& b : Behaviors)
	{
		b->GrabbableComp = this;

		//kept to keepold functionality working, but this should be unnecessary for new behaviors
		b->AffectedComponents = AffectedComponents;
		b->AffectedParents = AffectedComponentParents;
	}
}


// Called every frame
void UGrabbableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGrabbableComponent::OnRelease(const WRTK_CommonInfo::ButtonName button, UWRTK_InteractManager* im)
{
	for (auto& b : Behaviors)
	{
		b->OnRelease(button, im);
	}
}

void UGrabbableComponent::OnTouch(const WRTK_CommonInfo::ButtonName button, UWRTK_InteractManager* im)
{
	for (auto& b : Behaviors)
	{
		b->OnTouch(button, im);
	}
}

void UGrabbableComponent::OnAxis(const WRTK_CommonInfo::ButtonName button, UWRTK_InteractManager* im)
{
	for (auto& b : Behaviors)
	{
		b->OnAxis(button, im);
	}
}

void UGrabbableComponent::OnPress(const WRTK_CommonInfo::ButtonName button, UWRTK_InteractManager* im)
{
	for (auto& b : Behaviors)
	{
		b->OnPress(button, im);
	}
}

void UGrabbableComponent::OnAnalogMove(const float x, const float y, UWRTK_InteractManager* im)
{
	for (auto& b : Behaviors)
	{
		b->OnAnalogMove(x, y, im);
	}
}

void UGrabbableComponent::Release()
{
	UWRTK_InteractManager* interactManager = nullptr;
	CurrentIM = nullptr;

	if (CurrentGrabState == GRABBED)
	{
		for (auto& b : Behaviors)
		{
			interactManager = b->GrabbingInteractor;
			b->OnGrabRelease(b->GrabbingInteractor, b->GrabShape);
		}
	}
	else if (CurrentGrabState == PINCHED)
	{
		for (auto& b : Behaviors)
		{
			interactManager = b->PinchingInteractor;
			b->OnPinchRelease(b->PinchingInteractor, b->PinchShape);
		}
	}

	if (interactManager != nullptr)
	{
		interactManager->CurrentlyGrabbedObj = nullptr;
		interactManager->CurrentlyPinchedObj = nullptr;
	}
	CurrentGrabState = UNHELD;
}

//Below here lies all the IGrabShapeInterface variants for the OnGrab, OnPinch and the release functions
void UGrabbableComponent::OnGrab(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	if (CurrentGrabState != GrabbableState::UNHELD)
	{
		if (ShouldSwitchHandsOnRegrabOrPinch == true)
		{
			Release();
		}
		else
		{
			return;
		}
	}

	for (auto& b : Behaviors)
	{
		b->OnGrab(im, coll);
	}

	im->ClosestGrabbableCollider = coll;

	CurrentIM = im;
	im->CurrentlyGrabbedObj = this;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), GrabPickUpSound, im->HandActor->HandMesh->GetComponentLocation());

	CurrentGrabState = GrabbableState::GRABBED;
}

void UGrabbableComponent::OnGrabRelease(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	if (im)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GrabReleaseSound, im->HandActor->HandMesh->GetComponentLocation());

	Release();
}

void UGrabbableComponent::OnPinch(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	if (CurrentGrabState != GrabbableState::UNHELD)
	{
		if (ShouldSwitchHandsOnRegrabOrPinch == true)
		{
			Release();
		}
		else
		{
			return;
		}
	}

	for (auto& b : Behaviors)
	{
		b->OnPinch(im, coll);
	}

	im->ClosestPinchableCollider = coll;

	CurrentIM = im;
	im->CurrentlyPinchedObj = this;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PinchPickUpSound, im->HandActor->HandMesh->GetComponentLocation());

	CurrentGrabState = GrabbableState::PINCHED;
}

void UGrabbableComponent::OnPinchRelease(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	if(im)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PinchReleaseSound, im->HandActor->HandMesh->GetComponentLocation());

	Release();
}