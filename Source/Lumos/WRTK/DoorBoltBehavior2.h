// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "DoorBoltBehavior2.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUMOS_API UDoorBoltBehavior2 : public UBehaviorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		FString ControlBoltName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		FString OtherBoltName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		FString ControlBoltBehaviorName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		FString OtherBoltBehaviorName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		FString GrabbableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		FString DoorBehaviorName;


	bool isResettingPosition = false;

	float ResetRate = 5.0f;


private:

	class UStaticMeshComponent* ControlBolt;
	class UStaticMeshComponent* OtherBolt;
	FVector InitialPosition;
	class USegmentConstraintBehavior* ControlBoltBehavior;
	class USegmentConstraintBehavior* OtherBoltBehavior;
	class UGrabbableComponent* Grabbable;
	class ULeverBehavior* DoorBehavior;
	

protected:
	virtual void PostOnGrab(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnGrabRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	
};


