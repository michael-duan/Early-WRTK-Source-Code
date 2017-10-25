// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "TeleportMarker.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class LUMOS_API UTeleportMarker : public UBehaviorComponent
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FVector GetOwnedPosition();
	FRotator GetOwnedRotation();
	bool GetTPEnabled() { return TPEnabled; }

	UPROPERTY(EditAnywhere)
	AActor* OwnedPostion; // Use an empty actor to signify the teleport pos

	UPROPERTY(EditAnywhere)
	bool RotateOnTeleport = false;

	UFUNCTION(BlueprintCallable)
	void SetTPEnabled(bool b) { TPEnabled = b; }

	UPROPERTY(EditAnywhere)
	bool TPEnabled;
};
