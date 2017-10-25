// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "WRTK_CommonInfo.h"
#include "ThumbTurnBehavior.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUMOS_API UThumbTurnBehavior : public UBehaviorComponent
{
	GENERATED_BODY()
	
protected:
	virtual void BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void SearchForKnownComponents() override;

protected:
	virtual void PostOnGrab(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinch(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnGrabRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
	virtual void PostOnPinchRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) override;
public:
	//name of the scene component on the edge of the thumb turn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThumbTurn)
	FString ThumbEdgeMarkerName;

	//name of the scene component that marks the center of the thumb turn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThumbTurn)
	FString BaseName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThumbTurn)
	FString CircleNormalMarkerName;

private:
	class USceneComponent* ThumbEdgeMarker;
	class USceneComponent* Base;
	class USceneComponent* CircleNormalMarker;

public:
	//if true, the thumb turn's thumbedgemarker will snap to the hand (i.e. behave like a thumb turn); otherwise the thumb turn will move with the hand as if the initial grab position snaps to the hand (i.e. behave like a dial)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThumbTurn)
	bool ShouldSnapToHand = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ThumbTurn)
	bool bShouldAttachHand = true;

private:
	//on grab/pinch, this is the right vector of the hand projected onto the circle; afterward it rotates relative to the controller unless ShouldSnapToHand == true
	FVector OnGrabRightVector;

	void OnAttach(class UWRTK_InteractManager*, class IGrabShapeInterface*);
	void OnDetach(class UWRTK_InteractManager*, class IGrabShapeInterface*);
};
