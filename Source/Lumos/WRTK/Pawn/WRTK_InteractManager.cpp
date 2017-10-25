// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "../GrabbableComponent.h"
#include "../GrabSphereCollider.h"
#include "../GrabBoxCollider.h"
#include "../GrabShapeInterface.h"
#include "WRTK_HandActor.h"
#include "WRTK_InteractManager.h"


// Sets default values for this component's properties
UWRTK_InteractManager::UWRTK_InteractManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;



	// ...
}


// Called when the game starts
void UWRTK_InteractManager::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UWRTK_InteractManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//TArray<UPrimitiveComponent*> OverlappingComponents;
	
	//WARNING I HAVENT TESTED THE CODE BELOW YET. UNCOMMENT TO TRY CAUSE IM SCARED - Jason
	// find closest grabbable object
	//ClosestGrabbableCollider = GetClosestGrabObject();
	// find closest pinchable object
	//ClosestPinchableCollider = GetClosestPinchObject();
}

//Helper Functions
IGrabShapeInterface* UWRTK_InteractManager::GetClosestGrabObject(bool bIsGrab)
{
	int32 ClosestDistance = -5, ClosestIndex = 0;
	TArray<UPrimitiveComponent*> OverlappingComponents;

	if (bIsGrab)
		GrabSphereComponent->GetOverlappingComponents(OverlappingComponents);
	else
		PinchSphereComponent->GetOverlappingComponents(OverlappingComponents);

	for (int32 i = 0; i < OverlappingComponents.Num(); i++)
	{
		UPrimitiveComponent* const obj = OverlappingComponents[i];
		//Check if 
		UGrabSphereCollider* GrabSphere = Cast<UGrabSphereCollider>(obj);
		UGrabBoxCollider* GrabBox = Cast<UGrabBoxCollider>(obj);
		if (GrabSphere) {

			// only run if bIsGrab matches the sphere collider's grab type
			if ((GrabSphere->NoticesFistGrab() && bIsGrab) || (GrabSphere->NoticesPinch() && !bIsGrab))
			{
				FVector DistVect;

				if (bIsGrab)
					DistVect = (HandActor->GrabSphereComponent->GetComponentLocation() - obj->GetComponentLocation());
				else
					DistVect = (HandActor->PinchSphereComponent->GetComponentLocation() - obj->GetComponentLocation());

				int32 CurrDist = DistVect.SizeSquared();
				//Check if 
				if ((ClosestDistance == -5) || (CurrDist < ClosestDistance))
				{
					ClosestDistance = CurrDist;
					ClosestIndex = i;
				}
			}
			
		}
		else if (GrabBox)
		{
			// only run if bIsGrab matches the sphere collider's grab type
			if ((GrabBox->GetNoticesFistGrab() && bIsGrab) || (GrabBox->GetNoticesPinch() && !bIsGrab))
			{
				FVector DistVect;

				if (bIsGrab)
					DistVect = (HandActor->GrabSphereComponent->GetComponentLocation() - obj->GetComponentLocation());
				else
					DistVect = (HandActor->PinchSphereComponent->GetComponentLocation() - obj->GetComponentLocation());

				int32 CurrDist = DistVect.SizeSquared();
				//Check if 
				if ((ClosestDistance == -5) || (CurrDist < ClosestDistance))
				{
					ClosestDistance = CurrDist;
					ClosestIndex = i;
				}
			}
		}
	}
	return Cast<IGrabShapeInterface>(OverlappingComponents[ClosestIndex]);
}

// todo: punching
bool UWRTK_InteractManager::IsFistClenched()
{
	return true;
}


// TODO grabbing
void UWRTK_InteractManager::GripPressed()
{
	if (CurrentlyPinchedObj != nullptr)
		return;

	bIsGripDown = true;

	if (CurrentlyGrabbedObj)
		CurrentlyGrabbedObj->OnGrab(this, ClosestGrabbableCollider);
	else
	{
		ClosestGrabbableCollider = GetClosestGrabObject(true);
		if (ClosestGrabbableCollider)
		{
			UGrabbableComponent* GrabbableComponent = ClosestGrabbableCollider->GetGrabbableComponent();
			if (GrabbableComponent)
			{
				GrabbableComponent->OnGrab(this, ClosestGrabbableCollider);
				CurrentlyGrabbedObj = GrabbableComponent;
			}
		}
	}
	
}

void UWRTK_InteractManager::GripReleased()
{
	bIsGripDown = false;

	//Call Objects OnGrabRelease(), and reset CurrentlyGrabbedObj
	if (CurrentlyGrabbedObj)
	{
		CurrentlyGrabbedObj->OnGrabRelease(this, ClosestGrabbableCollider);
		CurrentlyGrabbedObj = NULL;
	}
}

void UWRTK_InteractManager::GripAxis(float amount)
{
	HandAnimInstance->SetGripAxis(amount);
}

void UWRTK_InteractManager::TriggerCapTouched()
{
	if(HandAnimInstance != NULL)
		HandAnimInstance->SetTriggerCap(true);
}

void UWRTK_InteractManager::TriggerCapUntouched()
{
	if (HandAnimInstance != NULL)
		HandAnimInstance->SetTriggerCap(false);
}

// TODO pinching
void UWRTK_InteractManager::TriggerPressed()
{
	if (HandAnimInstance != NULL)
		HandAnimInstance->SetTriggerDown(true);

	if (CurrentlyGrabbedObj != nullptr)
		return;


	if (CurrentlyPinchedObj)
	{
		CurrentlyPinchedObj->OnPinch(this, ClosestPinchableCollider);
	}
	else
	{
		ClosestPinchableCollider = GetClosestGrabObject(false);

		if (ClosestPinchableCollider)
		{
			UGrabbableComponent* GrabbableComponent = ClosestPinchableCollider->GetGrabbableComponent();
			if (GrabbableComponent)
			{
				GrabbableComponent->OnPinch(this, ClosestPinchableCollider);
				CurrentlyPinchedObj = GrabbableComponent;
			}
		}
	}
}

void UWRTK_InteractManager::TriggerReleased()
{
	if (HandAnimInstance != NULL)
		HandAnimInstance->SetTriggerDown(false);

	if (CurrentlyPinchedObj)
	{
		CurrentlyPinchedObj->OnPinchRelease(this, ClosestGrabbableCollider);
		CurrentlyPinchedObj = NULL;
	}
}

void UWRTK_InteractManager::TriggerAxis(float amount)
{
	if (HandAnimInstance != NULL)
		HandAnimInstance->SetTriggerAxis(amount);
}

void UWRTK_InteractManager::ThumbCapTouched()
{
	if (HandAnimInstance != NULL)
		HandAnimInstance->SetThumbCap(0, true);
}

void UWRTK_InteractManager::ThumbCapUntouched()
{
	if (HandAnimInstance != NULL)
		HandAnimInstance->SetThumbCap(0, false);
}

void UWRTK_InteractManager::CapTouched1()
{
	if (HandAnimInstance != NULL)
		HandAnimInstance->SetThumbCap(1, true);
}

void UWRTK_InteractManager::CapUntouched1()
{
	if (HandAnimInstance != NULL)
		HandAnimInstance->SetThumbCap(1, false);
}

void UWRTK_InteractManager::CapTouched2()
{
	if (HandAnimInstance != NULL)
		HandAnimInstance->SetThumbCap(2, true);
}

void UWRTK_InteractManager::CapUntouched2()
{
	if (HandAnimInstance != NULL)
		HandAnimInstance->SetThumbCap(2, false);
}

void UWRTK_InteractManager::Pressed1()
{

}

void UWRTK_InteractManager::Released1()
{

}

void UWRTK_InteractManager::Pressed2()
{

}

void UWRTK_InteractManager::Released2()
{

}


// collisoin
void UWRTK_InteractManager::OnGrabOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*//this is fucked
	UGrabSphereCollider* coll = Cast<UGrabSphereCollider>(OtherComp);
	if (coll) 
	{
		ClosestGrabbableCollider = coll;
	}*/
}

void UWRTK_InteractManager::OnGrabOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void UWRTK_InteractManager::OnPinchOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void UWRTK_InteractManager::OnPinchOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}