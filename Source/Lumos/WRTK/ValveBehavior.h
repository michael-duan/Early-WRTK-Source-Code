// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "WRTK_CommonInfo.h"
#include "ValveBehavior.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUMOS_API UValveBehavior : public UBehaviorComponent
{
	GENERATED_BODY()
	
protected:
	virtual void BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void SearchForKnownComponents() override;

protected:
	virtual void PostOnGrab(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinch(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;

	//name of the scene component on the edge of the valve (you should probably add a scene component for this)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Valve)
	FString ValveCircumferenceMarkerName;

	//name of the scene component that marks the center of the valve (the valve's mesh should work fine for this)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Valve)
	FString ValveBaseName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Valve)
	FString CircleNormalMarkerName;

private:
	class USceneComponent* CircumferenceMarker;
	class USceneComponent* Base;
	class USceneComponent* CircleNormalMarker;

public:
	//if true, the lever's handle will snap to the hand; otherwise the valve will move with the hand as if the initial grab position snaps to the hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Valve)
	bool ShouldSnapToHand = false;

private:
	//on grab/pinch, this is the position at which the valve was grabbed/pinched relative to the handle; afterward it moves relative to the controller unless ShouldSnapToHand == true
	USceneComponent* grabLocMarker;
};
