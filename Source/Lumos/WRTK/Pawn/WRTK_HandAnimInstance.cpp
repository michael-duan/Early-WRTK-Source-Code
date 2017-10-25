// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "WRTK_HandAnimInstance.h"

// 0 = thumb
// 1 = topbutton
// 2 = bottombutton

void UWRTK_HandAnimInstance::SetThumbCap(int key, bool bIsThumbDown)
{
	switch (key) 
	{
		case 0: bIs0Down = bIsThumbDown;
			break;
		case 1: bIs1Down = bIsThumbDown;
			break;
		case 2: bIs2Down = bIsThumbDown;
			break;
	}

	// this is true when any capacitors are being touched
	if (bIs0Down == true || bIs1Down == true || bIs2Down == true)
	{
		fTargetThumb = 0.0f;
		isThumbDown = true;
	}
	else
	{
		fTargetThumb = 1.0f;
		isThumbDown = false;
	}
}

void UWRTK_HandAnimInstance::SetTriggerCap(bool bIsTriggerDown)
{
	//if (!bIsTriggerDown)
		//fTargetTrigger = 1.0f;
	isIndexDown = bIsTriggerDown;
	isTriggerDown = bIsTriggerDown;

}

void UWRTK_HandAnimInstance::SetGripAxis(float amount)
{
	fCurrentGripAxis = amount;
	ReportGripAxis(amount);
	if (amount > 0.01f)
		isGripDown = true;
	else
		isGripDown = false;
}

void UWRTK_HandAnimInstance::SetTriggerAxis(float amount)
{
	fCurrentTriggerAxis = -amount;
	if (amount > 0.01)
		isTriggerDown = true;
	else
		isTriggerDown = false;
}

void UWRTK_HandAnimInstance::SetTriggerDown(bool bIsTriggerDown)
{
	isTriggerDown = bIsTriggerDown;
}

void UWRTK_HandAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	//Very Important Linevegeta's pride
	Super::NativeUpdateAnimation(DeltaTimeX);

	// update thumb cap
	fCurrentThumb = FMath::FInterpTo(fCurrentThumb, fTargetThumb, DeltaTimeX, fInterpSpeed);
	ReportThumbAxis(fCurrentThumb);
	// update trigger cap


	// TODO: optimize this

	if (fCurrentGripAxis > 0.1f && bTriggerDown)
		fTargetTriggerBlend = 0.0f;
	else
		fTargetTriggerBlend = 1.0f;

	if (!bTriggerDown)
		fTargetTrigger = 1.0f;
	else
		fTargetTrigger = fCurrentTriggerAxis;

	fCurrentTriggerBlend = FMath::FInterpTo(fCurrentTriggerBlend, fTargetTriggerBlend, DeltaTimeX, fInterpSpeed);
	fCurrentTrigger = FMath::FInterpTo(fCurrentTrigger, fTargetTrigger, DeltaTimeX, fInterpSpeed);
	ReportTriggerAxis(fCurrentTrigger);

	fHoldingObjectBlend = FMath::FInterpTo(fHoldingObjectBlend, fHoldingObjectBlendTarget, DeltaTimeX, fInterpSpeed);

	if (ShouldDrawHandAnimDebugInfo == true)
	{
		for (int i = 0; i < FingertipSocketNames.Num(); ++i)
		{
			//ensure that this fingertip name has a matching end name
			if (FingerRaycastEndSocketNames.Num() <= i)
			{
				break;
			}

			//draw line segments coming out of the fingers to show that the sockets work
			FVector lineStart = GetSkelMeshComponent()->GetSocketLocation(FingertipSocketNames[i]);
			FVector lineEnd = GetSkelMeshComponent()->GetSocketLocation(FingerRaycastEndSocketNames[i]);

			//for debugging
			DrawDebugLine(GetWorld(), lineStart, lineEnd, FColor::Red);
		}
	}
}

void UWRTK_HandAnimInstance::SetHoldingObject(bool bIsHolding, class UAnimSequence* Pose)
{
	isHolding = bIsHolding;

	if (bIsHolding)
	{
		holdPose = Pose;
		// blend into pose
		fHoldingObjectBlendTarget = 5.0f;
	}
	else
	{
		// blend out
		fHoldingObjectBlendTarget = 0.0f;
	}
}
