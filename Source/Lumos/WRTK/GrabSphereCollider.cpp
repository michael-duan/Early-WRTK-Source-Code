// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "GrabbableComponent.h"
#include "GrabSphereCollider.h"

UGrabSphereCollider::UGrabSphereCollider()
{
	PrimaryComponentTick.bCanEverTick = true;

	// add grab shape tag
	ComponentTags.Add(GRABSHAPE_TAG);
}

void UGrabSphereCollider::BeginPlay()
{
	Super::BeginPlay();

	GrabbableComponent = FindGrabbableComponentByName();
}

void UGrabSphereCollider::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShouldDebugDraw == true)
	{
		DrawDebugSphere(GetWorld(), this->GetComponentLocation(), this->GetScaledSphereRadius(), 32, FColor(0, 255, 0));
	}
}


UGrabbableComponent* UGrabSphereCollider::FindGrabbableComponentByName()
{
	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UGrabbableComponent::StaticClass());
	for (auto& c : comps)
	{
		if (c->GetName() == NameOfGrabbableComponent)
		{
			return Cast<UGrabbableComponent>(c);
		}
	}

	return nullptr;
}


