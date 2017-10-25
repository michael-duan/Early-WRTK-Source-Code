// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "RetrievableBehaviorComponent.h"
#include "Pawn/WRTK_InteractManager.h"
#include "RetrieveTriggerBox.h"


// Called when the game starts
void URetrievableBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	StartPosition = GetOwner()->GetActorLocation();
	StartRotation = GetOwner()->GetActorRotation();

	affectedComponent = nullptr;
	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto comp : comps)
	{
		if (comp->GetName() == AffectedMesh && comp->IsA(USceneComponent::StaticClass()))
		{
			affectedComponent = Cast<USceneComponent>(comp);
			StartPosition = affectedComponent->GetComponentLocation();
			StartRotation = affectedComponent->GetComponentRotation();
		}
	}
}

void URetrievableBehaviorComponent::Retrieve()
{
	if (IsAsleep) return;
	UWRTK_InteractManager* si = GetStrongestInteractor();
	if (held) return; // Don't teleport if currently interracted with

	// Go to specific respawn point if it is set, otherwise go to starting point
	// Need to disable physics during teleportation or skeletal meshes go crazy and yell at you
	GetOwner()->DisableComponentsSimulatePhysics();
	if (RespawnLocations.Num() > 0)
	{
		// Find the closest position to the actors current position
		FVector closestLocation = RespawnLocations[0]->GetActorLocation();
		FRotator closestRotation = RespawnLocations[0]->GetActorRotation();
		FVector currLoc = GetOwner()->GetActorLocation();
		int closestDistance = FVector::DistSquared(closestLocation, currLoc);
		for (auto points : RespawnLocations)
		{
			FVector curr = points->GetActorLocation();
			int currDist = FVector::DistSquared(curr, currLoc);
			if (currDist < closestDistance)
			{
				closestDistance = currDist;
				closestLocation = curr;
				closestRotation = points->GetActorRotation();
			}
		}
		if (affectedComponent == nullptr)
			GetOwner()->TeleportTo(closestLocation, closestRotation, false, true);
		else
			affectedComponent->SetWorldLocationAndRotation(closestLocation, closestRotation);
	}
	else
	{
		if (affectedComponent == nullptr)
			GetOwner()->TeleportTo(StartPosition, StartRotation, false, true);
		else
			affectedComponent->SetWorldLocationAndRotation(StartPosition, StartRotation);
		
	}
	GetOwner()->DisableComponentsSimulatePhysics();
	
}

void URetrievableBehaviorComponent::RetrieveAfterDelay()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(handle);
	GetOwner()->GetWorldTimerManager().SetTimer(handle, this, &URetrievableBehaviorComponent::Retrieve, RespawnDelay);
}

void URetrievableBehaviorComponent::SetEnabled(bool b)
{
	if (b)
	{

	}
	else
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(handle);
		IsAsleep = true;
	}
}

void URetrievableBehaviorComponent::PostOnGrabRelease(UWRTK_InteractManager* wim, class IGrabShapeInterface* gsi)
{
	Super::PostOnGrabRelease(wim, gsi);
	held = false;
	CheckForReturnOnRelease();
}

void URetrievableBehaviorComponent::PostOnPinchRelease(UWRTK_InteractManager* wim, class IGrabShapeInterface* gsi)
{
	Super::PostOnPinchRelease(wim, gsi);
	held = false;
	CheckForReturnOnRelease();
}

void URetrievableBehaviorComponent::PostOnGrab(UWRTK_InteractManager* wim, class IGrabShapeInterface* gsi)
{
	Super::PostOnGrab(wim, gsi);
	held = true;
	CheckForReturnOnRelease();
	GetOwner()->GetWorldTimerManager().ClearTimer(handle);
}

void URetrievableBehaviorComponent::PostOnPinch(UWRTK_InteractManager* wim, class IGrabShapeInterface* gsi)
{
	Super::PostOnPinchRelease(wim, gsi);
	held = true;
	CheckForReturnOnRelease();
	GetOwner()->GetWorldTimerManager().ClearTimer(handle);
}

void URetrievableBehaviorComponent::CheckForReturnOnRelease()
{
	TSet<AActor*> overlappingActors;
	GetOwner()->GetOverlappingActors(overlappingActors, ARetrieveTriggerBox::StaticClass());

	if(overlappingActors.Num() > 0)
	{
		RetrieveAfterDelay();
	}
}