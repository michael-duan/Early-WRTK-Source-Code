// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "Elevator.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"


// Sets default values for this component's properties
UElevator::UElevator()
{
	PrimaryComponentTick.bCanEverTick = true;
	mElevatorAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ElevatorAudioComp"));
	mElevatorAudioComponent->bAutoActivate = false;
}

void UElevator::BeginPlay()
{
	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto& c : comps)
	{
		if (c == nullptr)
		{
			continue;
		}
		else if (c->GetName() == elevatorName)
		{
			mElevator = Cast<USceneComponent>(c);
			break;
		}
	}

	FString currWay;
	for (int i = 0; i < numFloors; i++)
	{
		currWay = floorPrefix;
		currWay += FString::FromInt(i);
		for (auto& c : comps)
		{
			if (c == nullptr)
			{
				continue;
			}
			else if (c->GetName() == currWay)
			{
				mWaypoints.Add(Cast<USceneComponent>(c));
				break;
			}
		}
	}

	mElevatorAudioComponent->OnAudioFinished.AddDynamic(this, &UElevator::onSoundFinished);
}

void UElevator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (isMovingUp)
	{
		FVector deltaLocation(0.0f, 0.0f, DeltaTime * speed);
		mElevator->AddWorldOffset(deltaLocation);
		if (mElevator->GetComponentLocation().Z > targetPosition.Z) {
			reachDestFloor();
		}
	}
}

bool UElevator::moveUp()
{
	if ((targetFloor >= 0) && (targetFloor < mWaypoints.Num()) )
	{
		targetPosition = mWaypoints[targetFloor]->GetComponentLocation();
		isMovingUp = true;
		mElevatorAudioComponent->SetSound(mStartAudioCue);
		mElevatorAudioComponent->Play();
		return true;
	}
	return false;
}

void UElevator::reachDestFloor() 
{
	isMovingUp = false;
	targetFloor++;
	mElevatorAudioComponent->SetSound(mEndAudioCue);
	mElevatorAudioComponent->Play();

	OnReachedFloor.Broadcast(targetFloor);
}

void UElevator::onSoundFinished()
{
	if (isMovingUp == true)
	{
		mElevatorAudioComponent->SetSound(mMoveAudioCue);
		mElevatorAudioComponent->Play();
	}
}

