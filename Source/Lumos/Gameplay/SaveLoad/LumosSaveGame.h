// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DataArchive.h"
#include "LumosSaveGame.generated.h"

/**
 * Customized SaveGame class for Lantern.
 */
UCLASS()
class LUMOS_API ULumosSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FDataArchive> AllDataToSave;
};
