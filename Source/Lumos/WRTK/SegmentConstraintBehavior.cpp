// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "SegmentWaypoint.h"
#include "Pawn/WRTK_InteractManager.h"
#include "Pawn/WRTK_HandActor.h"
#include "GrabSphereCollider.h"
#include "SegmentConstraintBehavior.h"
#include "WRTK/HapticFeedbackBehavior.h"

//for testing WWise
#include "AkAudioDevice.h"
#include "AkComponent.h"
#include "AkAudioEvent.h"

#define SMALL_FLOAT_TOLERANCE 0.05f

void USegmentConstraintBehavior::BeginPlay()
{
	Super::BeginPlay();

	SetTickGroup(TG_EndPhysics);

	WaypointOwningActor = GetOwner();
	SearchForKnownComponents();

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UHapticFeedbackBehavior::StaticClass());
	for (auto& c : comps)
	{
		auto hf = Cast<UHapticFeedbackBehavior>(c);
		OnAddHapticFeedback.AddDynamic(hf, &UHapticFeedbackBehavior::AddHapticCurrentFrameAmplitude);
	}
}

void USegmentConstraintBehavior::SearchForKnownComponents()
{
	Super::SearchForKnownComponents();

	// find initial start and end waypoints

	if (WaypointOwningActor != nullptr)
	{
		TArray<UActorComponent*> comps = WaypointOwningActor->GetComponentsByClass(USegmentWaypoint::StaticClass());
		for (auto& c : comps)
		{
			if (c->IsA(USegmentWaypoint::StaticClass()))
			{
				if (c->GetName() == StartWaypointName)
				{
					CurrentStartWaypoint = Cast<USegmentWaypoint>(c);
				}
				else if (c->GetName() == EndWaypointName)
				{
					CurrentEndWaypoint = Cast<USegmentWaypoint>(c);
				}
			}
		}
	}

	if (bShouldSnapToStartWaypoint)
	{
		for (UActorComponent* c : AffectedComponents)
		{
			auto sc = Cast<USceneComponent>(c);
			if (sc != nullptr && CurrentStartWaypoint != nullptr)
			{
				sc->SetWorldLocation(CurrentStartWaypoint->GetComponentLocation());
			}
		}
	}

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UAudioComponent::StaticClass());
	for (auto& c : comps)
	{
		auto ac = Cast<UAudioComponent>(c);
		if (ac->GetName() == AudioComponentName || AudioComponentName.Len() == 0)
		{
			audioComponent = ac;
			audioComponent->SetVolumeMultiplier(0.0f);

			audioComponent->Play();
		}
	}

	//currently all this block does is show that WWise works
	comps = GetOwner()->GetComponentsByClass(UAkComponent::StaticClass());
	for (auto& c : comps)
	{
		auto ac = Cast<UAkComponent>(c);
		if (ac->GetName() == AkComponentName || AudioComponentName.Len() == 0)
		{
			akComponent = ac;
			if (akComponent->AkAudioEvent != nullptr)
			{
				//we pass in an empty FString for this call because we don't need an event name
				akComponent->PostAkEvent(akComponent->AkAudioEvent, FString());

				//for reference
				//akComponent->SetRTPCValue(FString("valueName"), 0.75f, 1);
			}
			
			FAkAudioDevice* AudioDevice = FAkAudioDevice::Get();
			if (AudioDevice != nullptr)
			{
				uint32 WwiseID = AudioDevice->GetIDFromString("MyString");
			}
		}
	}
}

void USegmentConstraintBehavior::BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::BehaviorTickComponent(DeltaTime, TickType, ThisTickFunction);

	//TODO: ENSURE GRABBABLE BEGINPLAY HAPPENS BEFORE BEHAVIOR BEGINPLAY AND MOVE THIS TO BEHAVIOR BEGINPLAY
	//this shouldn't be in tick; ideally it's in beginplay or some similar setup function but we need it to happen after grabbable's begin play and can't guarantee that yet
	if (lastFramePositions.Num() == 0)
	{
		initializeLastFramePositionsAndVelocities();
	}

	for (int i = 0; i < AffectedComponents.Num(); ++i)
	{
		if (CurrentStartWaypoint != nullptr)
		{
			ConstrainAndSimulatePhysics(i, DeltaTime);
		}
	}

	if (audioComponent != nullptr && lastFrameVelocities.Num() > 0)
	{
		float goalVolumeMultiplier = FMath::FInterpTo(audioComponent->VolumeMultiplier, BaseVolumeMultiplier * getNormalizedSpeed(0), DeltaTime, VolumeChangeInterpSpeed);
		audioComponent->SetVolumeMultiplier(goalVolumeMultiplier);

		//for debugging
		//UE_LOG(LogTemp, Warning, TEXT("volumeMultiplier is %f"), audioComponent->VolumeMultiplier);
	}

	if (fabs(GetPercentTravelled() - 1.0f) < SMALL_FLOAT_TOLERANCE && !bReachEnd) {
		bReachEnd = true;
		OnReachEnd.Broadcast();
	}
	else if(fabs(GetPercentTravelled() - 1.0f) > SMALL_FLOAT_TOLERANCE &&bReachEnd)
	{
		bReachEnd = false;
		OnLeaveEnd.Broadcast();
	}
}

FVector USegmentConstraintBehavior::FindClosestPointOnLine(FVector StartWaypoint, FVector EndWaypoint, FVector Point)
{
	FVector StartToPoint = Point - StartWaypoint;
	FVector StartToEnd = EndWaypoint - StartWaypoint;
	StartToEnd.Normalize();
	float Distance = FVector::Distance(StartWaypoint, EndWaypoint);
	float Change = FVector::DotProduct(StartToEnd, StartToPoint);

	if (Change <= 0)
		return StartWaypoint;
	if (Change >= Distance)
		return EndWaypoint;

	FVector asdf = StartToEnd * Change;
	FVector ClosestPoint = StartWaypoint + asdf;
	return ClosestPoint;
}

void USegmentConstraintBehavior::ConstrainAndSimulatePhysics(int componentIndex, float DeltaTime)
{
	USceneComponent* sc = Cast<USceneComponent>(AffectedComponents[componentIndex]);
	if (sc == nullptr)
	{
		return;
	}

	FVector approximatedVelocity;
	if ((bIsBeingGrabbed || bIsBeingPinched) && GetStrongestInteractor() != nullptr)
	{
		if (ShouldRotateWithHand == true)
		{
			FRotator rot = GetStrongestInteractor()->HandActor->PinchSphereComponent->GetComponentRotation();
			if (bIsBeingPinched == false)
			{
				rot = GetStrongestInteractor()->HandActor->GrabSphereComponent->GetComponentRotation();
			}

			sc->SetWorldRotation(rot);
		}

		// if there exists an endwaypoint already, just travel along that one
		if (CurrentEndWaypoint != nullptr)
		{
			FVector handLocation;
			
			if(bIsBeingPinched)
				handLocation = GetStrongestInteractor()->HandActor->PinchSphereComponent->GetComponentLocation();
			else
				handLocation = GetStrongestInteractor()->HandActor->GrabSphereComponent->GetComponentLocation();

			if (ShouldSnapToHand == true)
			{
				CurrentLocation = FindClosestPointOnLine(CurrentStartWaypoint->GetComponentLocation(), CurrentEndWaypoint->GetComponentLocation(), handLocation);

				//for debugging
				//DrawDebugSphere(GetWorld(), CurrentLocation, 0.5f, 10, FColor::Yellow);
			}
			else
			{
				//only move the object by the displacement between the grabLocMarker and the point on the line closest to the hand
				FVector markerToHandLoc = handLocation - grabLocMarker->GetComponentLocation();

				CurrentLocation += markerToHandLoc;
				CurrentLocation = FindClosestPointOnLine(CurrentStartWaypoint->GetComponentLocation(), CurrentEndWaypoint->GetComponentLocation(), CurrentLocation);

				//for debugging
				//DrawDebugSphere(GetWorld(), grabLocMarker->GetComponentLocation(), 0.5f, 5, FColor::Red);
				//DrawDebugSphere(GetWorld(), CurrentLocation, 0.5f, 5, FColor::Green);
			}

			CurrentLocation = FMath::Lerp(sc->GetComponentLocation(), CurrentLocation, DeltaTime * InterpSpeed);
			sc->SetWorldLocation(CurrentLocation, bShouldSweep, nullptr, ETeleportType::None);
		}
		//otherwise determine which endpoint to use
		else
		{
			// determine which endpoint to use
		}

		if (lastFrameVelocities.Num() > componentIndex)
		{
			//add haptic effect based on max expected speed
			float normalizedSpeed = getNormalizedSpeed(componentIndex);
			OnAddHapticFeedback.Broadcast(normalizedSpeed);
		}
	}
	//simulate physics if not being held
	else if (ShouldMaintainVelocityOnRelease == true && lastFrameVelocities.Num() > componentIndex)
	{
		FVector newPosBeforeProjectOntoLine = sc->GetComponentLocation() + lastFrameVelocities[componentIndex] * DeltaTime;
		FVector newPosProjectedOntoLine = FindClosestPointOnLine(CurrentStartWaypoint->GetComponentLocation(), CurrentEndWaypoint->GetComponentLocation(), newPosBeforeProjectOntoLine);
		sc->SetWorldLocation(newPosProjectedOntoLine, bShouldSweep, nullptr, ETeleportType::None);

		//for debugging
		//DrawDebugSphere(GetWorld(), newPosProjectedOntoLine, 8.0f, 10, FColor::Green);
	}

	if (lastFrameVelocities.Num() > componentIndex && lastFrameVelocities.Num() > componentIndex)
	{
		approximatedVelocity = (sc->GetRelativeTransform().GetLocation() - lastFramePositions[componentIndex]) / DeltaTime;
		lastFrameVelocities[componentIndex] = approximatedVelocity * FrictionMultiplier;
		lastFramePositions[componentIndex] = sc->GetRelativeTransform().GetLocation();
	}
}

float USegmentConstraintBehavior::GetPercentTravelled()
{
	float distance = 0;
	if (AffectedComponents[0]->IsA(USceneComponent::StaticClass()) && CurrentStartWaypoint != nullptr && CurrentEndWaypoint != nullptr)
	{
		FVector ComponentLocation = Cast<USceneComponent>(AffectedComponents[0])->GetComponentLocation();
		FVector StartLocation = CurrentStartWaypoint->GetComponentLocation();
		FVector EndLocation = CurrentEndWaypoint->GetComponentLocation();
		float TotalDistance = FVector::Distance(StartLocation, EndLocation);
		float CurrentDistance = FVector::Distance(ComponentLocation, StartLocation);
		distance = CurrentDistance / TotalDistance;
	}

	return distance;
}

void USegmentConstraintBehavior::PostOnGrab(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	Super::PostOnGrab(im, coll);

	if (bIsBeingGrabbed || bIsBeingPinched)
		return;

	if (im != nullptr && coll != nullptr)
		im->HandAnimInstance->SetHoldingObject(true, coll->GetGrabAnim());

	bIsBeingGrabbed = true;

	OnAttach(im, coll);
}

void USegmentConstraintBehavior::PostOnGrabRelease(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	Super::PostOnGrabRelease(im, coll);

	bIsBeingGrabbed = false;

	OnDetach(im, coll);
}

void USegmentConstraintBehavior::PostOnPinch(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	Super::PostOnPinch(im, coll);

	if (bIsBeingGrabbed || bIsBeingPinched)
		return;

	if (im != nullptr && coll != nullptr)
		im->HandAnimInstance->SetHoldingObject(true, coll->GetPinchAnim());

	bIsBeingPinched = true;

	OnAttach(im, coll);
}

void USegmentConstraintBehavior::OnAttach(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	if (bShouldAttachHand)
		im->HandActor->AttachDummyHandToComponent(true, Cast<USceneComponent>(AffectedComponents[0]), FAttachmentTransformRules::KeepWorldTransform);

	if (GetStrongestInteractor()->HandActor != nullptr && AffectedComponents[0] != nullptr)
	{
		USphereComponent* handSphere;
		if (bIsBeingGrabbed == true)
		{
			handSphere = GetStrongestInteractor()->HandActor->GrabSphereComponent;
		}
		else
		{
			handSphere = GetStrongestInteractor()->HandActor->PinchSphereComponent;
		}
		FVector handLoc = handSphere->GetComponentLocation();

		USceneComponent* base = Cast<USceneComponent>(AffectedComponents[0]);
		if (grabLocMarker != nullptr)
		{
			grabLocMarker->DestroyComponent();
			grabLocMarker = nullptr;
		}
		grabLocMarker = NewObject<USceneComponent>(base);
		grabLocMarker->AttachToComponent(base, FAttachmentTransformRules::KeepRelativeTransform);
		grabLocMarker->RegisterComponent();
		grabLocMarker->SetWorldLocation(handLoc);
	}
}

void USegmentConstraintBehavior::OnDetach(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	if (bShouldAttachHand)
		im->HandActor->AttachDummyHandToComponent(false, Cast<USceneComponent>(AffectedComponents[0]), FAttachmentTransformRules::KeepWorldTransform);

	if (im != nullptr && coll != nullptr)
		im->HandAnimInstance->SetHoldingObject(false, coll->GetGrabAnim());
}


void USegmentConstraintBehavior::PostOnPinchRelease(UWRTK_InteractManager* im, class IGrabShapeInterface* coll)
{
	Super::PostOnPinchRelease(im, coll);

	bIsBeingPinched = false;

	OnDetach(im, coll);
}

void USegmentConstraintBehavior::initializeLastFramePositionsAndVelocities()
{
	for (int i = 0; i < AffectedComponents.Num(); ++i)
	{
		//add empty vector for initialization
		lastFramePositions.Add(FVector());
		lastFrameVelocities.Add(FVector(0, 0, 0));

		auto sc = Cast<USceneComponent>(AffectedComponents[i]);
		if (sc != nullptr)
		{
			lastFramePositions[i] = sc->GetRelativeTransform().GetLocation();
		}
	}
}

float USegmentConstraintBehavior::getNormalizedSpeed(float componentIndex)
{
	return FMath::Min(lastFrameVelocities[componentIndex].Size(), SpeedExpectedUpperBound) / SpeedExpectedUpperBound;
}