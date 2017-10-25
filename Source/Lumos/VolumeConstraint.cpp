// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "VolumeConstraint.h"


// Sets default values for this component's properties
UVolumeConstraint::UVolumeConstraint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVolumeConstraint::BeginPlay()
{
	Super::BeginPlay();

	SetTickGroup(TG_PostPhysics);

	AActor* constraintVolumeOwner = VolumeActor;
	if (constraintVolumeOwner == nullptr)
	{
		constraintVolumeOwner = GetOwner();
	}
	TArray<UActorComponent*> comps = constraintVolumeOwner->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (auto& c : comps)
	{
		auto pc = Cast<UPrimitiveComponent>(c);
		if (pc != nullptr && ConstraintVolumeName.Len() == 0 || pc->GetName() == ConstraintVolumeName)
		{
			constraintVolume = pc;
			constraintVolume->OnComponentEndOverlap.AddDynamic(this, &UVolumeConstraint::onExitConstraintVolume);
		}
	}

	comps = GetOwner()->GetComponentsByClass(USceneComponent::StaticClass());
	for (auto& c : comps)
	{
		auto sc = Cast<USceneComponent>(c);
		if (sc->GetName() == ConstrainedComponentName)
		{
			constrainedComponent = sc;
		}
	}
}

void UVolumeConstraint::onExitConstraintVolume(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (overlappedComponent != constraintVolume || constrainedComponent == nullptr)
	{
		return;
	}

	FVector closestPointOnSurface;
	overlappedComponent->GetClosestPointOnCollision(constrainedComponent->GetComponentLocation(), closestPointOnSurface);
	constrainedComponent->SetWorldLocation(closestPointOnSurface);
}