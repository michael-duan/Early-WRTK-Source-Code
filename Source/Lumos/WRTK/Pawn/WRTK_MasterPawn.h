// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "WRTK_MasterPawn.generated.h"

UCLASS()
class LUMOS_API AWRTK_MasterPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWRTK_MasterPawn();

	UPROPERTY(EditAnywhere, Category = WRTK)
	TSubclassOf<AWRTK_HandActor> HandClass;

	UPROPERTY(EditAnywhere, Category = WRTK)
	class USceneComponent* LeftSocket;
	UPROPERTY(EditAnywhere, Category = WRTK)
	class USceneComponent* RightSocket;

	UPROPERTY(EditAnywhere, Category = WRTK)
	class USceneComponent* CameraNode;

	UPROPERTY(EditAnywhere, Category = WRTK)
	class UMotionControllerComponent* LeftMotionController;
	UPROPERTY(EditAnywhere, Category = WRTK)
	class UMotionControllerComponent* RightMotionController;
	UPROPERTY(EditAnywhere, Category = WRTK)
	class USceneComponent* MotionControllerNode;
	UPROPERTY(EditAnywhere, Category = WRTK)
	class UCameraComponent* MainCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class AWRTK_HandActor* LeftHandActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class USkeletalMeshComponent* LeftHandDummy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class USkeletalMeshComponent* LeftHandRenderMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class AWRTK_HandActor* RightHandActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class USkeletalMeshComponent* RightHandDummy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class USkeletalMeshComponent* RightHandRenderMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	float MoveSpeed=0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWRite, Category = WRTK)
	bool MovementEnabled = false;

	void RGripPressed();
	void RGripReleased();
	void RGripAxis(float amount);
	void RTriggerCapTouched();
	void RTriggerCapUntouched();
	void RTriggerPressed();
	void RTriggerReleased();
	void RTriggerAxis(float amount);
	void RThumbCapTouched();
	void RThumbCapUntouched();

	void LGripPressed();
	void LGripReleased();
	void LGripAxis(float amount);
	void LTriggerCapTouched();
	void LTriggerCapUntouched();
	void LTriggerPressed();
	void LTriggerReleased();
	void LTriggerAxis(float amount);
	void LThumbCapTouched();
	void LThumbCapUntouched();

	void ACapTouched();
	void ACapUntouched();
	void BCapTouched();
	void BCapUntouched();
	void XCapTouched();
	void XCapUntouched();
	void YCapTouched();
	void YCapUntouched();

	void AButtonPressed();
	void AButtonReleased();
	void BButtonPressed();
	void BButtonReleased();

	void XButtonPressed();
	void XButtonReleased();
	void YButtonPressed();
	void YButtonReleased();

	void LStickTiltUp(float amount);
	void LStickTiltRight(float amount);
	void RStickTiltUp(float amount);
	void RStickTiltRight(float amount);

	// Specific to mouse and keyboard controls
	void AddMouseX(float amount);
	void AddMouseY(float amount);
	void ScrollWheel(float amount);
	void FKeyPressed();
	void LeftShiftPressed() { LeftShiftHeld = true; };
	void LeftShiftReleased() { LeftShiftHeld = false; };
	void LeftCtrlPressed() { LeftCtrlHeld = true; };
	void LeftCtrlReleased() { LeftCtrlHeld = false; };
	void LeftAltPressed() { LeftAltHeld = true; };
	void LeftAltReleased() { LeftAltHeld = false; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Input variables
	FVector2D MovementInput;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FVector GetGroundNormal();
	void AttemptTeleport();
	float LastRStickTiltRightAmount = 0.0f; // Store this to check if we should rotate
	AActor* pCurrentTeleportLocation;

	// Private variables for tracking modifiers for axis input (KBM Only)
	bool LeftShiftHeld;
	bool LeftCtrlHeld;
	bool LeftAltHeld;
};
