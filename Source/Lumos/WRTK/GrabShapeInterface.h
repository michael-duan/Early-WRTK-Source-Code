// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "GrabShapeInterface.generated.h"

#define GRABSHAPE_TAG TEXT("GrabShapeTag")

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class LUMOS_API UGrabShapeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LUMOS_API IGrabShapeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//Class functions: These must be implemented
	virtual void BeginPlay() = 0;
	virtual class UGrabbableComponent* FindGrabbableComponentByName() = 0;

	//We have to implement all the data variables for these pure virtual getter functions
	//virtual FYourDataStruct& GetDataStruct() = 0;

	virtual class UGrabbableComponent* GetGrabbableComponent() = 0;
	virtual void SetGrabbableComponent(class UGrabbableComponent* grabc) = 0;

	virtual const FString GetNameOfGrabbableComponent() = 0;
	virtual void SetNameOfGrabbableComponent(const FString string) = 0;

	virtual const bool GetShouldRotateInLeftHand() = 0;
	virtual void SetShouldRotateInLeftHand(const bool value) = 0;

	virtual const bool GetShouldHaveGravityWhenReleased() = 0;
	virtual void SetShouldHaveGravityWhenReleased(const bool value) = 0;

	virtual const bool GetShouldSnapToSocket() = 0;
	virtual void SetShouldSnapToSocket(const bool value) = 0;

	virtual class UAnimSequence* GetGrabAnim() = 0;
	virtual void SetGrabAnim(class UAnimSequence* anime) = 0;

	virtual class UAnimSequence* GetPinchAnim() = 0;
	virtual void SetPinchAnim(class UAnimSequence* anime) = 0;

	virtual const FName GetLGrabSocket() = 0;
	virtual void SetLGrabSocket(FName name) = 0;

	virtual const FName GetLPinchSocket() = 0;
	virtual void SetLPinchSocket(FName name) = 0;

	virtual const FName GetRGrabSocket() = 0;
	virtual void SetRGrabSocket(FName name) = 0;

	virtual const FName GetRPinchSocket() = 0;
	virtual void SetRPinchSocket(FName name) = 0;

	virtual const bool GetNoticesFistGrab() = 0;
	virtual void SetNoticesFistGrab(const bool value) = 0;

	virtual const bool GetNoticesPinch() = 0;
	virtual void SetNoticesPinch(const bool value) = 0;
};
