// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VolumeConstraint.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUMOS_API UVolumeConstraint : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVolumeConstraint();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	//the name of the scene component to constrain
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VolumeConstraint)
	FString ConstrainedComponentName;

	//the actor that holds the constraint volume; if this is null, then we will assume that the owner of this component is the actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VolumeConstraint)
	AActor* VolumeActor;

	//the name of the constraint volume component (a primitive component)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VolumeConstraint)
	FString ConstraintVolumeName;

protected:
	USceneComponent* constrainedComponent;
	UPrimitiveComponent* constraintVolume;

	UFUNCTION()
	void onExitConstraintVolume(class UPrimitiveComponent* overlappedComponent, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex);
};
