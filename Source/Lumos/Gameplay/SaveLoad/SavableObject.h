// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SavableObject.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USavableObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * Savable Interface that every savable object need to inherit.
 */
class LUMOS_API ISavableObject
{
	GENERATED_BODY()

public:
	/* All child classes must implement their customized saving logics. */
	virtual void SaveObject(TArray<uint8>& saveData) = 0;
	/* All child classes must implemnet their corresponding loading logics. */
	virtual void LoadObject(TArray<uint8>& loadedData) = 0;
};
