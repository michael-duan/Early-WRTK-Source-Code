// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "WheelBehavior.h"
#include "WRTK/Pawn/WRTK_HandActor.h"
#include "WRTK/Pawn/WRTK_InteractManager.h"
#include "WRTK/GrabSphereCollider.h"
#include "WRTK/WRTK_Math.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

void UWheelBehavior::BeginPlay()
{
	Super::BeginPlay();

	//find wheel root
	bool foundWheelRoot = false;

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto& c : comps)
	{
		auto sc = Cast<USceneComponent>(c);
		if (sc == nullptr) 
		{
			continue;
		}
		if (sc->GetName() == WheelRootName)
		{
			foundWheelRoot = true;
			WheelRoot = sc;
			break;
		}
	}

	if ((foundWheelRoot) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wheel in %s does not have a root"), *GetOwner()->GetName());
		return;
	}
}

// Called every frame
void UWheelBehavior::BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::BehaviorTickComponent(DeltaTime, TickType, ThisTickFunction);

	//if not held, do nothing
	if (IsHeld() == false)
	{
		return;
	}
	//if WheelRoot is null
	if (WheelRoot == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wheel in %s does not have a root"), *GetOwner()->GetName());
		return;
	}
	float offset = 0.0f;
	
	FTransform actorTransform = GetOwner()->GetActorTransform();
	
	if (GetStrongestInteractor()->HandActor->HandName.Equals(TEXT("LeftHand"))) 
	{
		offset = -180.0f;
	}
	//get the relative location of the controller
	FVector controllerLocWS = GetStrongestInteractor()->HandActor->GrabSphereComponent->GetComponentLocation();
	FVector controllerLocRelative = UKismetMathLibrary::InverseTransformLocation(actorTransform, controllerLocWS);
	//get the angle of rotation
	float result = UKismetMathLibrary::RadiansToDegrees(atan2(controllerLocRelative.Z, controllerLocRelative.X));
	result += offset;
	WheelRoot->SetRelativeRotation(FRotator(result, 0.0f, 0.0f ));;
}

