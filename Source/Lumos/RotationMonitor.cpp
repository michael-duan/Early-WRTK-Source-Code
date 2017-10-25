// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "RotationMonitor.h"

#include "WRTK/WRTK_Math.h"
#include "WRTK/HapticFeedbackBehavior.h"

// Sets default values for this component's properties
URotationMonitor::URotationMonitor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetTickGroup(TG_PostUpdateWork);
}


// Called when the game starts
void URotationMonitor::BeginPlay()
{
	Super::BeginPlay();

	SearchForKnownComponents();

	OnReachedTargetAngle.AddDynamic(this, &URotationMonitor::PostOnReachedTargetAngle);

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UHapticFeedbackBehavior::StaticClass());
	for (auto& c : comps)
	{
		auto hf = Cast<UHapticFeedbackBehavior>(c);
		OnAddHapticFeedback.AddDynamic(hf, &UHapticFeedbackBehavior::AddHapticCurrentFrameAmplitude);
	}
}

void URotationMonitor::SearchForKnownComponents()
{
	//find fixed and rotating components
	bool foundFixedComp = false;
	bool foundRotatingComp = false;
	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto& c : comps)
	{
		auto sc = Cast<USceneComponent>(c);
		if (sc == nullptr) //if component is not a scenecomponent
		{
			continue;
		}

		if (sc->GetName() == FixedComponentName)
		{
			foundFixedComp = true;
			FixedComponent = sc;
		}
		else if (sc->GetName() == RotatingComponentName)
		{
			foundRotatingComp = true;
			RotatingComponent = sc;
		}
	}

	if (foundFixedComp == false || foundRotatingComp == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("RotationMonitor %s did not SearchforKnownComponents correctly"), *GetName());
	}
}

// Called every frame
void URotationMonitor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsAsleep)
		return;

	if (NumRotationsToTrack <= 1 && ShouldNotifyOnDegreeInterval == false && TargetAngles.Num() == 0)
	{
		return;
	}

	if (FixedComponent == nullptr || RotatingComponent == nullptr)
	{
		return;
	}

	//for debugging
	/*FVector v1EndLoc = RotatingComponent->GetComponentLocation() + RotatingComponent->GetForwardVector() * 50.0f;
	FVector v2EndLoc = FixedComponent->GetComponentLocation() + FixedComponent->GetForwardVector() * 50.0f;
	DrawDebugDirectionalArrow(GetWorld(), RotatingComponent->GetComponentLocation(), v1EndLoc, 3.0f, FColor::Orange);
	DrawDebugDirectionalArrow(GetWorld(), RotatingComponent->GetComponentLocation(), v2EndLoc, 3.0f, FColor::Purple);*/

	//get the actual current angle between the vectors
	float currFrameAngle = WRTK_Math::GetSignedAngleBetweenTwoVectors(RotatingComponent->GetForwardVector(), FixedComponent->GetForwardVector(),
																		RotatingComponent->GetUpVector());

	//convert the last frame angle to something between -180 and 180
	float lastFrameAngleMinusRots = lastFrameAngle - 360.0f * clockwiseRotationCounter;
	//convert to 360-degree format for legibility
	float last0360 = lastFrameAngleMinusRots;
	if (last0360 < 0.0f)
	{
		last0360 = 360.0f - FMath::Abs(last0360);
	}
	float curr0360 = currFrameAngle;
	if (curr0360 < 0.0f)
	{
		curr0360 = 360.0f - FMath::Abs(curr0360);
	}

	currFrameAngle = curr0360;
	if (lastFrameAngleIsValid == true)
	{
		if (NumRotationsToTrack > 1)
		{
			//guess whether we "crossed over" the -180/180 degree rotation marker between this frame and the last, and in which direction

			/*
			//for debugging
			UE_LOG(LogTemp, Warning, TEXT("RotationMonitor tick info:"));
			UE_LOG(LogTemp, Warning, TEXT("	currFrameAngle is %f"), currFrameAngle);
			UE_LOG(LogTemp, Warning, TEXT("	lastFrameAngle is %f"), lastFrameAngle);
			UE_LOG(LogTemp, Warning, TEXT("	lastFrameAngleMinusRots is %f"), lastFrameAngleMinusRots);
			*/

			/*
			we might have crossed over if all of the following are true:
				previous angle != current angle
			*/
			if (last0360 != curr0360)
			{
				//if prev angle is between 270 and 360, and current angle is below 90, assume we went clockwise past threshold
				if (last0360 >= 270 && curr0360 <= 90)
				{
					++clockwiseRotationCounter;
				}
				//if prev angle is between 0 and 90 and current angle above 270, assume we went counter-clockwise past threshold
				else if (last0360 <= 90 && curr0360 >= 270)
				{
					--clockwiseRotationCounter;
				}

				//ensure that clockwiseRotationCounter's absolute value never exceeds NumRotationsToTrack
				clockwiseRotationCounter = FMath::Min(FMath::Max(clockwiseRotationCounter, -NumRotationsToTrack), NumRotationsToTrack);
			}
		}

		currFrameAngle = curr0360 + 360.0f * clockwiseRotationCounter;

		LastFrameAngleChange = currFrameAngle - lastFrameAngle;

		/*
		//for debugging
		UE_LOG(LogTemp, Warning, TEXT("	clockwiseRotationCounter is %f"), (float)clockwiseRotationCounter);
		UE_LOG(LogTemp, Warning, TEXT("	new lastFrameAngle is %f"), lastFrameAngle);
		*/
	}

	if (ShouldNotifyOnDegreeInterval == true)
	{
		//if our angle is near a DegreeNotificationInterval, broadcast an event
		check(DegreeNotificationInterval > 0);
		if (FMath::FloorToInt(currFrameAngle) % DegreeNotificationInterval <= CloseEnoughToTargetThreshold)
		{
			float onIntervalAngle = FMath::FloorToInt(currFrameAngle) - (FMath::FloorToInt(currFrameAngle) % DegreeNotificationInterval);

			//if we don't want to notify the same angle consecutively, then only notify if the current target angle isn't the same as the previous one
			if (ShouldNotifySameAngleConsecutively == true || lastTargetHitIsValid == false || lastTargetHit != onIntervalAngle)
			{
				lastTargetHit = onIntervalAngle;
				lastTargetHitIsValid = true;
				OnReachedTargetAngle.Broadcast(onIntervalAngle);
				OnRotationMonitorReachedTarget.Broadcast(this);
			}
		}
	}

	//if our angle is near a threshold, broadcast an event
	for (float target : TargetAngles)
	{
		if (FMath::Abs(currFrameAngle - target) <= CloseEnoughToTargetThreshold)
		{
			//if we don't want to notify the same angle consecutively, then only notify if the current target angle isn't the same as the previous one
			if (ShouldNotifySameAngleConsecutively == true || lastTargetHitIsValid == false || lastTargetHit != target)
			{
				lastTargetHit = target;
				lastTargetHitIsValid = true;
				OnReachedTargetAngle.Broadcast(target);
				OnRotationMonitorReachedTarget.Broadcast(this);
			}

			break; //don't keep looking for target angles
		}
	}

	lastFrameAngle = currFrameAngle;
	lastFrameAngleIsValid = true;
}

void URotationMonitor::PostOnReachedTargetAngle(float targetAngle)
{
	OnAddHapticFeedback.Broadcast(HapticBaseStrength);
}

float URotationMonitor::GetCurrentRotation()
{
	if (NumRotationsToTrack <= 1)
	{
		float currAngle = WRTK_Math::GetSignedAngleBetweenTwoVectors(FixedComponent->GetForwardVector(), RotatingComponent->GetForwardVector(),
			RotatingComponent->GetUpVector());

		//convert to 360 degree format
		float curr0360 = currAngle;
		if (curr0360 < 0.0f)
		{
			curr0360 = 360.0f - FMath::Abs(curr0360);
		}

		//add previous rotations (if we're counting any)
		return curr0360 + 360.0f * clockwiseRotationCounter;
	}
	else
	{
		return lastFrameAngle;
	}
}