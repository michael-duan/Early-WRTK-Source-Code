// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SwitchListener.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPostSwitchSignature, USwitchListener*, SwitchListener);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUMOS_API USwitchListener : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USwitchListener();
	virtual void BeginPlay() override;

public:
	//a generic state is useful for most switchlisteners; override this for different SwitchListeners if necessary
	UFUNCTION(BlueprintCallable, Category = "Switch")
	virtual bool IsOn() { return IsActive; }

	//whether this switch should do anything when it is switched
	bool IsActive = true;

	//Actors in this list will be notified when this SwitchListener is switched if their SwitchListener component name is in the list below
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Switch)
	TArray<AActor*> ListenerActors;

	//Specify the SwitchListener component names to notify when notifying from ListenerActorsToNotify
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Switch)
	TArray<FString> ListenerComponents;

	UPROPERTY(BlueprintReadOnly, Category = Switch)
	USceneComponent* Triggerer;

	//default behavior is just to notify listeners that this switch has been switched; the effect should be specified in DoPostSwitchEffect (C++) or with OnPostSwitch (Blueprint)
	void OnSwitch(USwitchListener* sl);

	UFUNCTION(BlueprintCallable, Category = "Switch")
	void OnSwitch(AActor* switchActor);

	UPROPERTY(BlueprintAssignable, Category = "Switch")
	FOnPostSwitchSignature OnPostSwitch;

protected:
	UFUNCTION()
	virtual void DoPostSwitchEffect(USwitchListener* switchListener);

public:
	//if enabled, this switch will disable itself after it is switched so that its state changes (i.e. on->off, off->on)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Switch)
	bool IsOneShotSwitch = false;
};