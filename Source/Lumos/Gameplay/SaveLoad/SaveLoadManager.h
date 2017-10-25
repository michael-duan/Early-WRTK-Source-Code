// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataArchive.h"

/**
 * Manager class for game saving and loading.
 */
class LUMOS_API SaveLoadManager
{
public:
	SaveLoadManager();
	~SaveLoadManager();

	void RegisterSavableObject(FString, class ISavableObject*);
	void SaveAll(FString);
	void LoadAll(FString);
	void CleanRegistry();

private:
	TMap<FString, class ISavableObject*> SavableObjectRegistry;
	class ULumosSaveGame* SaveGame;
};
