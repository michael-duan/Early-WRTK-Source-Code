// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "CollisionSoundComponent.h"


// Sets default values for this component's properties
UCollisionSoundComponent::UCollisionSoundComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCollisionSoundComponent::BeginPlay()
{
	Super::BeginPlay();

	// loop over all the components of our actor to find the one we want to monitor for collisions
	affectedComponent = nullptr;
	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto comp : comps)
	{
		if (comp->GetName() == MonitoredMesh && comp->IsA(USceneComponent::StaticClass()))
		{
			affectedComponent = Cast<USceneComponent>(comp);
			UPrimitiveComponent* pc = Cast<UPrimitiveComponent>(comp);
			pc->OnComponentHit.AddDynamic(this, &UCollisionSoundComponent::OnHit);
		}
	}
	
}


// Called every frame
void UCollisionSoundComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCollisionSoundComponent::OnHit(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit)
{
	// Play the sound or something
	PlayedSound->Activate(true);
	PlayedSound->Play(0.0f);
}