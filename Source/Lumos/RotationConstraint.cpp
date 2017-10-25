// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "RotationConstraint.h"

#include "RotationMonitor.h"
#include "WRTK/WRTK_Math.h"

#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
URotationConstraint::URotationConstraint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void URotationConstraint::BeginPlay()
{
	Super::BeginPlay();

	SetTickGroup(TG_PostPhysics);

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(USceneComponent::StaticClass());
	for (auto& c : comps)
	{
		auto sc = Cast<USceneComponent>(c);
		if (sc->GetName() == BeginThresholdMarkerName)
		{
			beginThresholdMarker = sc;
		}
		else if (sc->GetName() == EndThresholdMarkerName)
		{
			endThresholdMarker = sc;
		}
		else if (sc->GetName() == ConstrainedCompName)
		{
			constrainedComp = sc;
		}
		else if (sc->GetName() == FixedForwardMarkerName)
		{
			fixedForwardMarker = sc;
		}
	}

	if (DegreeAmountsToActivateConstraints.Num() > 0 && fixedForwardMarker != nullptr)
	{
		//make a rotation monitor for each threshold marker
		rotMonitor = NewObject<URotationMonitor>(this);
		rotMonitor->RegisterComponent();
		rotMonitor->FixedComponent = fixedForwardMarker;
		rotMonitor->RotatingComponent = constrainedComp;

		float maxDegree = FMath::Abs(DegreeAmountsToActivateConstraints[0]);
		for (float angle : DegreeAmountsToActivateConstraints)
		{
			if (FMath::Abs(angle) > maxDegree)
			{
				maxDegree = FMath::Abs(angle);
			}
		}

		rotMonitor->NumRotationsToTrack = FMath::FloorToInt(maxDegree / 360) + 1;
	}
}

// Called every frame
void URotationConstraint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//for debugging
	/*if (rotMonitor != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("rotation is %f"), rotMonitor->GetCurrentRotation());
	}*/

	//if the constrained space is acute: if constrained vector is not between the two thresholds, find the closest threshold and put it there
	//if the constrained space is obtuse: if the constrained vector is between the two thresholds, find the closest threshold and put it there
	if (WRTK_Math::IsVectorBetweenTwoOthers(beginThresholdMarker->GetForwardVector(), constrainedComp->GetForwardVector(), endThresholdMarker->GetForwardVector()) != constrainedSpaceIsAcute)
	{
		FRotator rot;

		//find which vector is closer and snap the constrained component's forward to that vector
		float beginToConstrainedAngle = WRTK_Math::GetAngleBetweenTwoVectors(beginThresholdMarker->GetForwardVector(), constrainedComp->GetForwardVector());
		float endToConstrainedAngle = WRTK_Math::GetAngleBetweenTwoVectors(endThresholdMarker->GetForwardVector(), constrainedComp->GetForwardVector());
		if (beginToConstrainedAngle < endToConstrainedAngle)
		{
			if (DegreeAmountsToActivateConstraints.Num() == 0 || isBeyondDegreeAmountToActivateConstraint())
			{
				rot = UKismetMathLibrary::MakeRotFromXZ(beginThresholdMarker->GetForwardVector(), beginThresholdMarker->GetUpVector());
				constrainedComp->SetWorldRotation(rot);
			}
		}
		else
		{
			if (DegreeAmountsToActivateConstraints.Num() == 0 || isBeyondDegreeAmountToActivateConstraint())
			{
				rot = UKismetMathLibrary::MakeRotFromXZ(endThresholdMarker->GetForwardVector(), beginThresholdMarker->GetUpVector());
				constrainedComp->SetWorldRotation(rot);
			}
		}

		//for debugging
		/*FVector arrowEndLoc = constrainedComp->GetComponentLocation() + constrainedComp->GetForwardVector() * 50.0f;
		DrawDebugDirectionalArrow(GetWorld(), constrainedComp->GetComponentLocation(), arrowEndLoc, 3.0f, FColor::Purple);*/
	}
}

bool URotationConstraint::isBeyondDegreeAmountToActivateConstraint()
{
	for (float angle : DegreeAmountsToActivateConstraints)
	{
		if (angle > 0 && rotMonitor->GetCurrentRotation() > angle)
		{
			return true;
		}
		else if (angle < 0 && rotMonitor->GetCurrentRotation() < angle)
		{
			return true;
		}
	}

	return false;
}

