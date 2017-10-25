// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Gameplay/SaveLoad/SavableObject.h"
#include "SavableObjectTestPawn.generated.h"

UCLASS()
class LUMOS_API ASavableObjectTestPawn : public APawn, public ISavableObject
{
	GENERATED_BODY()
	
public:	
	ASavableObjectTestPawn();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InComp) override;
	virtual void SaveObject(TArray<uint8>&) override;
	virtual void LoadObject(TArray<uint8>&) override;
	void ChangeString();
	void OutputString();
	void SaveGame();
	void LoadGame();

protected:
	virtual void BeginPlay() override;

private:
	uint32 StringSize;
	FString TestString;
	FString SaveFile;
	class SaveLoadManager* SLManager;

	UPROPERTY(EditAnywhere)
	USceneComponent* OurVisibleComponent;
};
