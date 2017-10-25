// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WRTK_HandAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class LUMOS_API UWRTK_HandAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GripAxisAmount"))
	void ReportGripAxis(float amount);
	
	void SetThumbCap(int key, bool bIsThumbDown);
	void SetTriggerCap(bool bIsTriggerDown);
	void SetGripAxis(float amount);
	void SetTriggerAxis(float amount);

	void SetTriggerDown(bool bIsTriggerDown);

	void SetHoldingObject(bool bIsHolding, class UAnimSequence* Pose);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "PointAxisAmount"))
	void ReportPointAxis(float amount);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "TriggerAxisAmount"))
	void ReportTriggerAxis(float amount);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ThumbAxisAmount"))
	void ReportThumbAxis(float amount);

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class UAnimSequence* holdPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	float fHoldingObjectBlend;

	float fHoldingObjectBlendTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	float fInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
		float fCurrentTriggerBlend = 1.0f;

	float fTargetThumb;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
		bool isThumbDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
		bool isIndexDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
		bool isGripDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
		bool isTriggerDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
		bool isHolding;

private:
	bool bIs0Down;
	bool bIs1Down;
	bool bIs2Down;

	bool bTriggerDown;

	float fThumbAlpha;

	float fCurrentGripAxis;
	float fCurrentTriggerAxis;
	float fCurrentThumb;
	float fCurrentTrigger;
	float fTargetTrigger;
	float fTargetTriggerBlend = 1.0f;
	
public:
	//the names of the sockets that mark the fingertips
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK_HandAnim)
	TArray<FName> FingertipSocketNames;

	//the names of the sockets that mark the ends of the finger raycasts; match the order of these to the order of FingertipSocketNames
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK_HandAnim)
	TArray<FName> FingerRaycastEndSocketNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK_HandAnim_Debug)
	bool ShouldDrawHandAnimDebugInfo = false;

	//length of the debug lines that will come out of the fingers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK_HandAnim_Debug)
	float FingerDebugLineLength = 5.0f;
};
