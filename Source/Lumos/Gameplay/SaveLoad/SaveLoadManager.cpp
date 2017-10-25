// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "LumosSaveGame.h"
#include "DataArchive.h"
#include "SavableObject.h"
#include "SaveLoadManager.h"
#include "Kismet/GameplayStatics.h"

SaveLoadManager::SaveLoadManager()
{
}

SaveLoadManager::~SaveLoadManager()
{
}

void SaveLoadManager::RegisterSavableObject(FString objectId, class ISavableObject* object)
{
	SavableObjectRegistry.Add(objectId, object);
}

void SaveLoadManager::SaveAll(FString saveFilePath)
{
	SaveGame = Cast<ULumosSaveGame>(UGameplayStatics::CreateSaveGameObject(ULumosSaveGame::StaticClass()));
	for (auto it = SavableObjectRegistry.CreateConstIterator(); it; ++it)
	{
		TArray<uint8> ToBinary;
		it.Value()->SaveObject(ToBinary);
		SaveGame->AllDataToSave.Emplace(it.Key(), ToBinary);
	}
	UGameplayStatics::SaveGameToSlot(SaveGame, saveFilePath, 0);
}

void SaveLoadManager::LoadAll(FString saveFilePath)
{
	SaveGame = Cast<ULumosSaveGame>(UGameplayStatics::LoadGameFromSlot(saveFilePath, 0));
	for (FDataArchive& data : SaveGame->AllDataToSave)
	{
		if (SavableObjectRegistry.Contains(data.ObjectId)) {
			SavableObjectRegistry[data.ObjectId]->LoadObject(data.DataToSave);
		}
	}
}

void SaveLoadManager::CleanRegistry()
{
	SavableObjectRegistry.Reset();
}
