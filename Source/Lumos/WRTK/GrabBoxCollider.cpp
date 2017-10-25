// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "GrabbableComponent.h"
#include "GrabBoxCollider.h"




UGrabBoxCollider::UGrabBoxCollider() 
{
	// add grab shape tag
	ComponentTags.Add(GRABSHAPE_TAG);
}

void UGrabBoxCollider::BeginPlay()
{
	Super::BeginPlay();

	GrabbableComponent = FindGrabbableComponentByName();
}


UGrabbableComponent* UGrabBoxCollider::FindGrabbableComponentByName()
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