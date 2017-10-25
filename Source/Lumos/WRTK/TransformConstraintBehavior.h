// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRTK/BehaviorComponent.h"
#include "TransformConstraintBehavior.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (WRTK), meta = (BlueprintSpawnableComponent))
class LUMOS_API UTransformConstraintBehavior : public UBehaviorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	virtual void BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FVector startPos;
	FRotator startRot;

public:
	/*
	translations and rotations are currently relative to origin
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformConstraint)
	bool ConstrainXTranslation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformConstraint)
	bool ConstrainYTranslation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformConstraint)
	bool ConstrainZTranslation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformConstraint)
	bool ConstrainRoll = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformConstraint)
	bool ConstrainPitch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformConstraint)
	bool ConstrainYaw = false;

	//max relative translation in the positive direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformConstraint)
	FVector MaxTranslationPositive;

	//max relative translation in the negative direction (note: these must be 0 or lower)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformConstraint)
	FVector MaxTranslationNegative;

	//max relative rotation in the positive direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformConstraint)
	FRotator MaxRotationPositive; //note: order is pitch, yaw, roll

	//max relative rotation in the negative direction (note: these must be 0 or lower)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformConstraint)
	FRotator MaxRotationNegative; //note: order is pitch, yaw, roll

	void SetCurrentPosAsReference();
	void SetCurrentRotAsReference();
	void SetCurrentTransformAsReference() { SetCurrentPosAsReference(); SetCurrentRotAsReference(); }

private:
	//helper function
	void constrain(USceneComponent* sc);
	
	//helper function for constrain; false is translation, true is rotation
	void constrainAxis(USceneComponent* sc, bool isTranslationOrRotation, uint32 xyzToConstrain);
};
