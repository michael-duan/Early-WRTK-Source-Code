// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WRTK_PlayerController.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class LUMOS_API AWRTK_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	//Constructor and initializers
	AWRTK_PlayerController(const FObjectInitializer& ObjectInitializer);

	//Tick
	virtual void Tick(float DeltaSeconds) override;
	
protected:
};
