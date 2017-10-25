// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SwitchListener.h"
#include "OverlapSwitch.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUMOS_API UOverlapSwitch : public USwitchListener
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

private:
	bool hasOverlappers = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OverlapSwitch)
	FString CollisionShapeName;

	UPrimitiveComponent* CollisionShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OverlapSwitch)
	TSubclassOf<AActor> ClassToTriggerOverlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OverlapSwitch)
	bool bShouldUseTagInstead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OverlapSwitch)
	FName TagToTriggerOverlap;

	UFUNCTION()
	void TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	virtual bool IsOn() override { return hasOverlappers; }
};
