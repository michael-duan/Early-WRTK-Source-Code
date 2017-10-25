// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "WRTK_CommonInfo.h"
#include "LeverBehavior.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUMOS_API ULeverBehavior : public UBehaviorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void SearchForKnownComponents() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lever)
	FString LeverBaseName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lever)
	FString LeverHandleName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lever)
	FString CircleNormalMarkerName;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lever)
	float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lever)
	bool bShouldAttachHand = true;

public:
	FVector CircleRight;
	class USceneComponent* Handle;
	class USceneComponent* Base;
	class USceneComponent* CircleNormalMarker;

public:
	//if true, the lever's handle will snap to the hand; otherwise the lever will move with the hand as if the initial grab position snaps to the hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lever)
	bool ShouldSnapToHand = false;

private:
	//on grab/pinch, this is the position at which the valve was grabbed/pinched relative to the handle; afterward it moves relative to the controller unless ShouldSnapToHand == true
	USceneComponent* grabLocMarker;

	void OnAttach(class UWRTK_InteractManager*, class IGrabShapeInterface*);
	void OnRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*);

protected:
	virtual void PostOnGrab(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinch(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnGrabRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinchRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
};
