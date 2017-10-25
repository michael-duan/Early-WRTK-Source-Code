// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "WheelBehavior.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
//UCLASS()


class LUMOS_API UWheelBehavior : public UBehaviorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lever)
		//TEnumAsByte<DirectionType> CircleNormalBasedOnActorDirection = DirectionType::ACTOR_RIGHT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Wheel)
		FString WheelRootName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Wheel)
		FString leftColliderName;


	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Wheel)
		//FString LeverBaseName;

private:
	
	//FVector CircleNormal;
	//class USceneComponent* Handle;
	//class USceneComponent* Base;
	class USceneComponent* WheelRoot;

	class UGrabSphereCollider* LeftCollider;

	
	
};
