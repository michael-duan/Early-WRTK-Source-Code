// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabShapeInterface.h"
#include "Components/BoxComponent.h"
#include "GrabBoxCollider.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (WRTK), meta = (BlueprintSpawnableComponent))
class LUMOS_API UGrabBoxCollider : public UBoxComponent, public IGrabShapeInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class UGrabbableComponent* GrabbableComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		FString NameOfGrabbableComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		bool ShouldRotateInLeftHand = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		bool ShouldHaveGravityWhenReleased;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		bool ShouldSnapToSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		class UAnimSequence* grabAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		class UAnimSequence* pinchAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		FName LGrabSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		FName LPinchSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		FName RGrabSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		FName RPinchSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		bool NoticesFistGrab = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		bool NoticesPinch = false;

public:
	UGrabBoxCollider();

	virtual void BeginPlay();

	class UGrabbableComponent* FindGrabbableComponentByName();

	//Getters and Setters for all the member variables, these are a must to implement since interfaces cannot store data
	const FString GetNameOfGrabbableComponent() override { return NameOfGrabbableComponent; }
	void SetNameOfGrabbableComponent(const FString string) override { NameOfGrabbableComponent = string;  }

	class UGrabbableComponent* GetGrabbableComponent() override { return GrabbableComponent; }
	void SetGrabbableComponent(class UGrabbableComponent* grabc) override { GrabbableComponent = grabc; }

	const bool GetShouldRotateInLeftHand() override { return ShouldRotateInLeftHand; }
	void SetShouldRotateInLeftHand(const bool value) override { ShouldRotateInLeftHand = value; }

	const bool GetShouldHaveGravityWhenReleased() override { return ShouldHaveGravityWhenReleased; }
	void SetShouldHaveGravityWhenReleased(const bool value) override { ShouldHaveGravityWhenReleased = value; }

	const bool GetShouldSnapToSocket() override { return ShouldSnapToSocket; }
	void SetShouldSnapToSocket(const bool value) override { ShouldSnapToSocket = value; }

	class UAnimSequence* GetGrabAnim() override { return grabAnim; }
	void SetGrabAnim(class UAnimSequence* anime) override { grabAnim = anime; }

	class UAnimSequence* GetPinchAnim() override { return pinchAnim; }
	void SetPinchAnim(class UAnimSequence* anime) override { pinchAnim = anime; }

	const FName GetLGrabSocket() override { return LGrabSocket; }
	void SetLGrabSocket(FName name) override { LGrabSocket = name; }

	const FName GetLPinchSocket() override { return LPinchSocket; }
	void SetLPinchSocket(FName name) override { LPinchSocket = name; }

	const FName GetRGrabSocket() override { return RGrabSocket; }
	void SetRGrabSocket(FName name) override { RGrabSocket = name; }

	const FName GetRPinchSocket() override { return RPinchSocket; }
	void SetRPinchSocket(FName name) override { RPinchSocket = name; }
	
	UFUNCTION(BlueprintCallable)
	const bool GetNoticesFistGrab() override { return NoticesFistGrab; }
	UFUNCTION(BlueprintCallable)
	void SetNoticesFistGrab(const bool value) override { NoticesFistGrab = value; }

	UFUNCTION(BlueprintCallable)
	const bool GetNoticesPinch() override { return NoticesPinch; }
	UFUNCTION(BlueprintCallable)
	void SetNoticesPinch(const bool value) override { NoticesPinch = value; }
	
};
