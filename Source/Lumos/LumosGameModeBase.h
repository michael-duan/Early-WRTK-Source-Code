// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "Gameplay/SaveLoad/SaveLoadManager.h"
#include "LumosGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class LUMOS_API ALumosGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ALumosGameModeBase();
	virtual void Tick(float DeltaSeconds) override;
	SaveLoadManager* GetSaveLoadManager() { return &MSaveLoadManager; }

protected:

private:
	SaveLoadManager MSaveLoadManager;
};
