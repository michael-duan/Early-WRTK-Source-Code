// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "DoorBehavior.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))

class LUMOS_API UDoorBehavior : public UBehaviorComponent
{
	GENERATED_BODY()

	public:
		virtual void BeginPlay() override;
		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
	public:

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
			FString DoorRootName;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
			int direction;


		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
			FRotator targetRotation;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
			float turnRate;

	private:
		class USceneComponent* DoorRoot;
		bool isOpening = false;
		
};
