// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
# include "Runtime/CoreUObject/Public/UObject/Class.h"
#include "Elevator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReachedFloorSignature, float, floor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUMOS_API UElevator : public UActorComponent
{
	GENERATED_BODY()

public:	

	UElevator();

	//Elevator Motion
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString elevatorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int numFloors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString floorPrefix;

	//Sound Cues
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* mStartAudioCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* mEndAudioCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* mMoveAudioCue;

	UPROPERTY(BlueprintAssignable, Category = RotationMonitor)
	FOnReachedFloorSignature OnReachedFloor;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	//call to move elevator up
	UFUNCTION(BlueprintCallable)
	bool moveUp();


protected:

	virtual void BeginPlay() override;

	//called when destination floor is reached
	void reachDestFloor();

	USceneComponent * mElevator;

	//holds scene component references to waypoints for each floor 
	TArray<USceneComponent*> mWaypoints;

	//target position based on target floor
	FVector targetPosition;

	//target index of floor in mWaypoints array
	int targetFloor = 0;

	bool isMovingUp = false;

	UAudioComponent * mElevatorAudioComponent;

protected:
	UFUNCTION()
	void onSoundFinished();
};
