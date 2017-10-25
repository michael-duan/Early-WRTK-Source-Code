// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "DoorBehavior.h"

#include "WRTK/Pawn/WRTK_HandActor.h"
#include "WRTK/Pawn/WRTK_InteractManager.h"
#include "WRTK/GrabSphereCollider.h"
#include "WRTK/WRTK_Math.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

void UDoorBehavior::BeginPlay()
{
	Super::BeginPlay();

	//find wheel root
	bool foundDoorRoot = false;

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto& c : comps)
	{
		auto sc = Cast<USceneComponent>(c);
		if (sc == nullptr)
		{
			continue;
		}
		if (sc->GetName() == DoorRootName)
		{
			foundDoorRoot = true;
			DoorRoot = sc;
			break;
		}
	}

	if ((foundDoorRoot) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Door in %s does not have a root"), *GetOwner()->GetName());
		return;
	}
}

// Called every frame
void UDoorBehavior::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!IsActive()) {
		return;
	}
	//if WheelRoot is null
	if (DoorRoot == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Yellow, TEXT("got null"));
		UE_LOG(LogTemp, Warning, TEXT("Door in %s does not have a root"), *GetOwner()->GetName());
		return;
	}
	else if (DoorRoot->GetComponentRotation().Equals(targetRotation)) {
		return;
	}

	else if (isOpening) {
		DoorRoot->SetWorldRotation(FMath::Lerp(DoorRoot->GetComponentRotation(), targetRotation, DeltaTime * turnRate));
		return;
	}

	if (!IsHeld()) {
		return;
	}

	GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Yellow, TEXT("door root entered"));
	FTransform actorTransform = GetOwner()->GetActorTransform();

	//get the relative location of the controller
	FVector controllerLocWS = GetStrongestInteractor()->HandActor->GrabSphereComponent->GetComponentLocation();
	FVector controllerLocRelative = UKismetMathLibrary::InverseTransformLocation(actorTransform, controllerLocWS);
	//get the angle of rotation
	float result = UKismetMathLibrary::RadiansToDegrees(atan2(controllerLocRelative.Y, controllerLocRelative.X));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::SanitizeFloat(result));
	targetRotation = FRotator(0.0f, result, 0.0f);
	isOpening = true;
}





