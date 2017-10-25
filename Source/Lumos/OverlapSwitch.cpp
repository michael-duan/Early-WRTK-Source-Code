// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "OverlapSwitch.h"

void UOverlapSwitch::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (auto& c : comps)
	{
		if (c->GetName() == CollisionShapeName)
		{
			CollisionShape = Cast<UPrimitiveComponent>(c);
			if (CollisionShape == nullptr) //if component is not a scenecomponent
			{
				break;
			}
			CollisionShape->OnComponentBeginOverlap.AddDynamic(this, &UOverlapSwitch::TriggerEnter);
			CollisionShape->OnComponentEndOverlap.AddDynamic(this, &UOverlapSwitch::TriggerExit);
		}
	}
}

void UOverlapSwitch::TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (bShouldUseTagInstead)
	{
		if (!OtherComp->ComponentHasTag(TagToTriggerOverlap))
			return;
	}
	else if (!OtherActor->IsA(ClassToTriggerOverlap))
	{
		//for debugging
		//UE_LOG(LogTemp, Warning, TEXT("OverlapSwitch's TriggerEnter OtherActor is %s"), *OtherActor->GetName());
		return;
	}

	Triggerer = OtherComp;

	hasOverlappers = true;
	OnSwitch(this);
}

void UOverlapSwitch::TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bShouldUseTagInstead)
		if (!OtherComp->ComponentHasTag(TagToTriggerOverlap))
			return;

	else if (!OtherActor->IsA(ClassToTriggerOverlap))
		return;

	//for debugging
	//UE_LOG(LogTemp, Warning, TEXT("OverlapSwitch's TriggerExit OtherActor is %s"), *OtherActor->GetName());

	//check if we still have overlappers
	TSet<AActor*> overlappers;
	if(bShouldUseTagInstead)
		OtherComp->GetOverlappingActors(overlappers);
	else
		OtherComp->GetOverlappingActors(overlappers, ClassToTriggerOverlap);

	if (bShouldUseTagInstead == false || OtherComp->ComponentHasTag(TagToTriggerOverlap))
	{
		hasOverlappers = false;
	}

	OnSwitch(this);
}