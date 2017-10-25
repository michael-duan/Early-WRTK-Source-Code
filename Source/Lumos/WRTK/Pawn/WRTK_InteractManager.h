// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WRTK_HandAnimInstance.h"
#include "WRTK_InteractManager.generated.h"


UCLASS( ClassGroup=(WRTK), meta=(BlueprintSpawnableComponent) )
class LUMOS_API UWRTK_InteractManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWRTK_InteractManager();


	// the sphere used to determine if things are grabbable, reference is passed in from WRTK_HandActor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class USphereComponent* GrabSphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class USphereComponent* PinchSphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class UWRTK_HandAnimInstance* HandAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class UChildActorComponent* HandChildActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class AWRTK_HandActor* HandActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class UGrabbableComponent* CurrentlyPinchedObj;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	class UGrabbableComponent* CurrentlyGrabbedObj;

	class IGrabShapeInterface* ClosestGrabbableCollider;
	class IGrabShapeInterface* ClosestPinchableCollider;

	//The Grab Shape Interface is used to call the interface functions for all shapes.
	//class IGrabShapeInterface* ClosestGrabbableCollider;
	//class IGrabShapeInterface* ClosestPinchableCollider;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	//Helper Functions
	IGrabShapeInterface* GetClosestGrabObject(bool IsGrab); //TOTEST

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GripPressed();
	void GripReleased();
	void GripAxis(float amount);
	void TriggerCapTouched();
	void TriggerCapUntouched();
	void TriggerPressed();
	void TriggerReleased();
	void TriggerAxis(float amount);
	void ThumbCapTouched();
	void ThumbCapUntouched();

	// 1 is B, 2 is A
	// 1 is Y, 2 is X
	void CapTouched1();
	void CapUntouched1();
	void CapTouched2();
	void CapUntouched2();

	void Pressed1();
	void Released1();
	void Pressed2();
	void Released2();

	bool bIsGripDown;

	bool IsFistClenched();

	UFUNCTION()
	void OnGrabOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGrabOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnPinchOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPinchOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
