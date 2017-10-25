// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "WRTK_CommonInfo.h"

#include "GrabbableComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUMOS_API UGrabbableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UGrabbableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	TArray<FString> AffectedComponentNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	TArray<FString> AffectedComponentParentNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	TArray<FString> BehaviorNames;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	TArray<class UBehaviorComponent*> Behaviors;

	UPROPERTY(BlueprintReadOnly, Category = WRTK)
	class UWRTK_InteractManager* CurrentIM;

	TArray<class UActorComponent*> AffectedComponents;
	TArray<class UActorComponent*> AffectedComponentParents;

public:
	virtual void OnRelease(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*);
	virtual void OnTouch(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*);
	virtual void OnAxis(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*);
	virtual void OnPress(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*);
	virtual void OnAnalogMove(const float x, const float y, class UWRTK_InteractManager*);

	virtual void OnGrab(class UWRTK_InteractManager*, class IGrabShapeInterface*);
	virtual void OnGrabRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*);
	virtual void OnPinch(class UWRTK_InteractManager*, class IGrabShapeInterface*);
	virtual void OnPinchRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*);

public:
	enum GrabbableState { UNHELD, GRABBED, PINCHED };
	//track whether the grabbable is being held, and how
	GrabbableState CurrentGrabState = UNHELD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	USoundBase* GrabPickUpSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	USoundBase* GrabReleaseSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	USoundBase* PinchPickUpSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WRTK)
	USoundBase* PinchReleaseSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	bool ShouldSwitchHandsOnRegrabOrPinch = true;

	//used to release anything we are currently holding
	UFUNCTION(BlueprintCallable)
	void Release();
};
