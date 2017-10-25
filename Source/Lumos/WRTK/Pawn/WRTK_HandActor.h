// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WRTK_HandActor.generated.h"

UCLASS()
class LUMOS_API AWRTK_HandActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWRTK_HandActor();

	UPROPERTY(BlueprintReadOnly, Category = WRTK)
	EControllerHand HandEnum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	FString HandName;

	UFUNCTION(BlueprintCallable, Category = WRTK)
	bool IsRightHand();

	UPROPERTY(EditDefaultsOnly, Category = WRTK)
	class USkeletalMeshComponent* HandMesh;

	// the motion controller component that this hand should follow
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	class UMotionControllerComponent* OwningMotionController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	class AWRTK_MasterPawn* OwningMasterPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	class USceneComponent* TempGrabScene;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	class USphereComponent* GrabSphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	class USphereComponent* PinchSphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	class USphereComponent* FistSphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	class UPhysicsHandleComponent* PhysicsHandleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	class UWRTK_InteractManager* InteractManagerComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// If using KB+M controls, represent the position relative to the owner in spherical coordinates
	// This allows intuitive controls for fixed radius, and radius adjustments are natural
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WRTK)
	bool UsingKBM = false;

	float Theta; // The "yaw" for the position (Not for orientation)
	float Phi;   // The "pitch" for the postion (Not for orientation)
	float Radius;
	// Used for tracking orientation for KBM controls
	float KBMYaw;
	float KBMPitch;
	float KBMRoll;
	bool MovedSinceTick = true;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnGrabOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnGrabOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void OnPinchOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnPinchOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Functions specifically for KBM controls
	UFUNCTION()
		void UpdateTheta(float amount);

	UFUNCTION()
		void UpdatePhi(float amount);

	UFUNCTION()
		void UpdateRadius(float amount);

	UFUNCTION()
		void UpdateKBMYaw(float amount);

	UFUNCTION()
		void UpdateKBMPitch(float amount);

	UFUNCTION()
		void UpdateKBMRoll(float amount);

	UFUNCTION()
		void EnableKBM();

	USceneComponent* HandSocket;
	USkeletalMeshComponent* DummyHand;

	UPROPERTY(BlueprintReadOnly, Category = WRTK)
	UMaterialInstanceDynamic* HandMeshMaterialInstance;

	void AttachDummyHandToComponent(bool bShouldAttach, USceneComponent* OtherComp, FAttachmentTransformRules Rules);

public:
	//interpolation speed of the physics handle
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	float PhysicsHandleBaseInterpSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	float DummyHandInterpSpeed = 50.0f;

	bool bShouldInterpToGhostHand = false;
	bool bShouldKeepRotation = false;
	bool bTempReplaceThis = false;
};
