// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataArchive.generated.h"

/**
 * A data container for game saving.
 */
USTRUCT()
struct LUMOS_API FDataArchive
{
	GENERATED_BODY()

	FDataArchive() {}

	FDataArchive(FString id, TArray<uint8>& toSave): 
		ObjectId(id),
		DataToSave(toSave)
	{}

	UPROPERTY()
	FString ObjectId;

	UPROPERTY()
	TArray<uint8> DataToSave;
};
