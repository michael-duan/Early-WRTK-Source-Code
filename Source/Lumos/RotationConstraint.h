// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RotationConstraint.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUMOS_API URotationConstraint : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URotationConstraint();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/*
	the name of a scenecomponent (e.g. an arrowcomponent) that will mark one end of the rotation
	the constrained component should not be counter-clockwise of this marker
	we will also use the up vector of this component to determine which way to set the constrained marker's up vector
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationConstraint)
	FString BeginThresholdMarkerName;

	/*
	the name of a scenecomponent (e.g. an arrowcomponent) that will mark one end of the rotation
	the constrained component should not be clockwise of this marker
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationConstraint)
	FString EndThresholdMarkerName;

	//the name of a scenecomponent (e.g. an arrowcomponent) whose forward vector we will constrain between the begin and end threshold markers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationConstraint)
	FString ConstrainedCompName;

	/*
	the name of a scenecomponent (e.g. an arrowcomponent) whose forward vector will be used to monitor the constrained component's rotation relative to that vector
	we use this to track multiple rotations; so leave this field blank if you only want to constrain 0-360
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationConstraint)
	FString FixedForwardMarkerName;

	//the degree amounts after which the constraints will become active (e.g. -360 and 270 will activate the constraints after one full counter-clockwise or one 3/4 rotation)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationConstraint)
	TArray<float> DegreeAmountsToActivateConstraints;

	//if true, we will keep the constrained component inside the two threshold vectors; if false, we will keep it outside them
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationConstraint)
	bool constrainedSpaceIsAcute = true;

protected:
	USceneComponent* beginThresholdMarker;
	USceneComponent* endThresholdMarker;
	USceneComponent* fixedForwardMarker;
	USceneComponent* constrainedComp;

	class URotationMonitor* rotMonitor;

	bool isBeyondDegreeAmountToActivateConstraint();
};
