// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "RetrievableBehaviorComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (WRTK), meta = (BlueprintSpawnableComponent))
class LUMOS_API URetrievableBehaviorComponent : public UBehaviorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostOnGrabRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinchRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnGrab(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinch(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	
public:
	void Retrieve();
	void RetrieveAfterDelay();

	UFUNCTION(BlueprintCallable, Category = WRTK)
	void SetEnabled(bool b);

	UPROPERTY(EditAnywhere, Category = WRTK)
	TArray<AActor*> RespawnLocations;

	UPROPERTY(EditAnywhere, Category = WRTK)
	FString AffectedMesh; // Provide the name of the mesh that needs to be teleported

	UPROPERTY(EditAnywhere, Category = WRTK)
	float RespawnDelay = 7.0f;

private:
	FVector StartPosition;
	FRotator StartRotation;
	FTimerHandle handle;
	bool held = false;

	USceneComponent* affectedComponent;

	void CheckForReturnOnRelease();
};
