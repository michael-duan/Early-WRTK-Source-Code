// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK_CommonInfo.h"
#include "Components/ActorComponent.h"
#include "BehaviorComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUMOS_API UBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBehaviorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void SearchForKnownComponents();

public:
	// Called every frame; don't override this in child classes
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	//override this in child classes
	virtual void BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) { /* do nothing by default */ }

public:
	//determines whether this behavior should do anything (e.g. tick, respond to grabs/releases)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WRTK)
	bool IsAsleep = false;

	/* The name of the primaray grabbable component. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BehaviorComponent)
	FString MainGrabCompName;

	/* The name of the secondary grabbable component that the attachable component switchs to after been attached. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BehaviorComponent)
	FString SecondaryGrabCompName;

	/*
	* A list of grab shape collider names.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BehaviorComponent)
	TArray<FName> GrabColliderNames;

public:
	//do not override these in child classes
	void OnRelease(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*);
	void OnTouch(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*);
	void OnAxis(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*);
	void OnPress(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*);
	void OnAnalogMove(const float x, const float y, class UWRTK_InteractManager*);

	void OnGrab(class UWRTK_InteractManager*, class IGrabShapeInterface*);
	void OnGrabRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*);
	void OnPinch(class UWRTK_InteractManager*, class IGrabShapeInterface*);
	void OnPinchRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*);

	//override these in child classes
protected:
	virtual void PostOnRelease(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*) { /* do nothing*/ }
	virtual void PostOnTouch(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*) { /* do nothing*/ }
	virtual void PostOnAxis(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*) { /* do nothing*/ }
	virtual void PostOnPress(const WRTK_CommonInfo::ButtonName button, class UWRTK_InteractManager*) { /* do nothing*/ }
	virtual void PostOnAnalogMove(const float x, const float y, class UWRTK_InteractManager*) { /* do nothing*/ }

	virtual void PostOnGrab(class UWRTK_InteractManager*, class IGrabShapeInterface*) { /* do nothing*/ }
	virtual void PostOnGrabRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) { /* do nothing*/ }
	virtual void PostOnPinch(class UWRTK_InteractManager*, class IGrabShapeInterface*) { /* do nothing*/ }
	virtual void PostOnPinchRelease(class UWRTK_InteractManager*, class IGrabShapeInterface*) { /* do nothing*/ }

	virtual void SwitchToUseSecondaryGrabbable();
	virtual void SwitchToUseMainGrabbable();

public:
	class UWRTK_InteractManager* GrabbingInteractor;
	class UWRTK_InteractManager* PinchingInteractor;
	class IGrabShapeInterface* GrabShape;
	class IGrabShapeInterface* PinchShape;

public:
	class UWRTK_InteractManager* GetStrongestInteractor() const;
	class IGrabShapeInterface* GetStrongestInteractionShape() const;
	UFUNCTION(BlueprintCallable)
	const bool IsHeld() const { return (GrabbingInteractor != nullptr) || (PinchingInteractor != nullptr); }

public:
	//deprecated; just access this through GrabbableComponent if possible
	TArray<class UActorComponent*> AffectedComponents;
	//deprecated; just access this through GrabbableComponent if possible
	TArray<class UActorComponent*> AffectedParents;

	class UGrabbableComponent* GrabbableComp;

	TArray<class IGrabShapeInterface*> GrabBoxColliders;
	class UGrabbableComponent* MainGrabComp;
	class UGrabbableComponent* SecondaryGrabComp;
};
