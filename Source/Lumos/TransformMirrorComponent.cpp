// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "TransformMirrorComponent.h"


// Sets default values for this component's properties
UTransformMirrorComponent::UTransformMirrorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UTransformMirrorComponent::SearchForKnownComponents()
{
	TArray<UActorComponent*> comps;

	//find handle and base locations
	if (ThisActor)
	{
		comps = ThisActor->GetComponentsByClass(UActorComponent::StaticClass());
		for (auto& c : comps)
		{
			auto sc = Cast<USceneComponent>(c);
			if (sc == nullptr) //if component is not a scenecomponent
			{
				continue;
			}

			if (sc->GetName() == ThisComponentName)
			{
				ThisComponent = sc;
			}
		}
	}
	
	if (OtherActor)
	{
		comps = OtherActor->GetComponentsByClass(UActorComponent::StaticClass());
		for (auto& c : comps)
		{
			auto sc = Cast<USceneComponent>(c);
			if (sc == nullptr) //if component is not a scenecomponent
			{
				continue;
			}

			if (sc->GetName() == OtherComponentName)
			{
				OtherComponent = sc;
			}
		}
	}

}


// Called when the game starts
void UTransformMirrorComponent::BeginPlay()
{
	Super::BeginPlay();

	SearchForKnownComponents();

	if (ThisComponent)
	{
		OldPosition = ThisComponent->GetRelativeTransform().GetLocation();
		OldRotation = ThisComponent->GetRelativeTransform().GetRotation();

		bReady = true;
	}

	// ...
	
}


// Called every frame
void UTransformMirrorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bReady)
	{
		if (ThisComponent)
		{
			OldPosition = ThisComponent->GetRelativeTransform().GetLocation();
			OldRotation = ThisComponent->GetRelativeTransform().GetRotation();

			bReady = true;
		}
	}
	
	// if both components exist
	if (ThisComponent && OtherComponent && bReady)
	{
		FVector NewPosition = ThisComponent->GetRelativeTransform().GetLocation();

		FQuat NewRotation = ThisComponent->GetRelativeTransform().GetRotation();

		FVector PositionDiff = NewPosition - OldPosition;



		FQuat RotationDiff = NewRotation - OldRotation;

		if (MirrorX)
			OtherComponent->AddRelativeLocation(FVector(PositionDiff.X * XScale, 0, 0), false);
		if (MirrorY)
		{
			OtherComponent->AddRelativeLocation(FVector(0, PositionDiff.Y * YScale, 0), false);

		}
		if (MirrorZ)
			OtherComponent->AddRelativeLocation(FVector(0, 0, PositionDiff.Z * ZScale), false);

		float changeInDegrees = 0.0f;
		FRotator oldRot = FRotator(OldRotation);
		FRotator newRot = FRotator(NewRotation);


		// turned clockwise
		if (oldRot.Yaw > 140.0f && newRot.Yaw < -140.0f)
		{
			changeInDegrees = (180.0f - oldRot.Yaw) + (newRot.Yaw + 180.0f);
		}
		// turned counter clockwise
		else if (oldRot.Yaw < -140.0f && newRot.Yaw > 140.0f)
		{
			changeInDegrees = (oldRot.Yaw + 180.0f) + (180.0f - newRot.Yaw);
		}
		else
			changeInDegrees = newRot.Yaw - oldRot.Yaw;

		if (MirrorYaw)
		{
			// -180 wraps to 180, so we check for these
			// changeInDegrees goes by clockwise = positive, counterclockwise = negative


			OtherComponent->AddRelativeRotation(FRotator(0, changeInDegrees * YawMirrorScale, 0));
		}

		if (YawToZ)
		{
			OtherComponent->AddRelativeLocation(FVector(0, 0, changeInDegrees * YawToZScale), false);
		}


		OldPosition = ThisComponent->GetRelativeTransform().GetLocation();
		OldRotation = ThisComponent->GetRelativeTransform().GetRotation();

	}
}

