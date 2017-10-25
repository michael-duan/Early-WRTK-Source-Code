// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "RetrieveTriggerBox.h"
#include "RetrievableBehaviorComponent.h"

ARetrieveTriggerBox::ARetrieveTriggerBox()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ARetrieveTriggerBox::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &ARetrieveTriggerBox::AttemptRetrieval);
}

void ARetrieveTriggerBox::AttemptRetrieval(AActor* MyOverlappedActor, AActor* OtherActor)
{
	if (OtherActor)
	{
		TArray<URetrievableBehaviorComponent*> RetrievableComponents;
		OtherActor->GetComponents(RetrievableComponents);
		for (auto retrievable : RetrievableComponents)
		{
			retrievable->RetrieveAfterDelay();
		}
	}
}