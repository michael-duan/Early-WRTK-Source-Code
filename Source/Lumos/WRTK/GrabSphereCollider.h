// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabShapeInterface.h"
#include "Components/SphereComponent.h"
#include "GrabSphereCollider.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (WRTK), meta = (BlueprintSpawnableComponent))
class LUMOS_API UGrabSphereCollider : public USphereComponent, public IGrabShapeInterface
{
	GENERATED_BODY()

public:
	UGrabSphereCollider();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
	bool ShouldDebugDraw = false;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		bool noticesFistGrab = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		bool noticesPinch = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class UGrabbableComponent* GrabbableComponent;

	class UGrabbableComponent* FindGrabbableComponentByName();

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
		FName l_GrabSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		FName l_PinchSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		FName r_GrabSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK, meta = (AllowPrivateAccess = "true"))
		FName r_PinchSocket;

	const bool NoticesFistGrab() const { return noticesFistGrab; }
	const bool NoticesPinch() const { return noticesPinch; }

	//Implementing the interface functions below

	//Getters and Setters for the variables
	const FString GetNameOfGrabbableComponent() override { return NameOfGrabbableComponent; }
	void SetNameOfGrabbableComponent(const FString string) override { NameOfGrabbableComponent = string; }

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

	const FName GetLGrabSocket() override { return l_GrabSocket; }
	void SetLGrabSocket(FName name) override { l_GrabSocket = name; }

	const FName GetLPinchSocket() override { return l_PinchSocket; }
	void SetLPinchSocket(FName name) override { l_PinchSocket = name; }

	const FName GetRGrabSocket() override { return l_GrabSocket; }
	void SetRGrabSocket(FName name) override { r_GrabSocket = name; }

	const FName GetRPinchSocket() override { return r_PinchSocket; }
	void SetRPinchSocket(FName name) override { r_PinchSocket = name; }

	const bool GetNoticesFistGrab() override { return noticesFistGrab; }
	void SetNoticesFistGrab(const bool value) override { noticesFistGrab = value; }

	const bool GetNoticesPinch() override { return noticesPinch; }
	void SetNoticesPinch(const bool value) override { noticesPinch = value; }
};
