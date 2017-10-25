// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "RetrieveTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class LUMOS_API ARetrieveTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
	
public:
	ARetrieveTriggerBox();
	void BeginPlay() override;

private:
	UFUNCTION()
	void AttemptRetrieval(AActor* MyOverlappedActor, AActor* OtherActor);
	
};
