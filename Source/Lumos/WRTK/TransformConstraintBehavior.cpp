// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "TransformConstraintBehavior.h"

void UTransformConstraintBehavior::BeginPlay()
{
	Super::BeginPlay();

	SetTickGroup(TG_LastDemotable);

	SetCurrentTransformAsReference();
}

void UTransformConstraintBehavior::BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::BehaviorTickComponent(DeltaTime, TickType, ThisTickFunction);

	for (UActorComponent* c : AffectedComponents)
	{
		auto sc = Cast<USceneComponent>(c);
		if (sc != nullptr)
		{
			constrain(sc);
		}
	}
}

void UTransformConstraintBehavior::constrain(USceneComponent* sc)
{
	//for debugging
	//UE_LOG(LogTemp, Warning, TEXT("constrain function of actor %s"), *GetOwner()->GetName());

	if (ConstrainXTranslation)
	{
		constrainAxis(sc, 0, 0);
	}
	if (ConstrainYTranslation)
	{
		constrainAxis(sc, 0, 1);
	}
	if (ConstrainZTranslation)
	{
		constrainAxis(sc, 0, 2);
	}
	if (ConstrainPitch)
	{
		constrainAxis(sc, 1, 0);
	}
	if (ConstrainYaw)
	{
		constrainAxis(sc, 1, 1);
	}
	if (ConstrainRoll)
	{
		constrainAxis(sc, 1, 2);
	}
}

void UTransformConstraintBehavior::constrainAxis(USceneComponent* sc, bool isTranslationOrRotation, uint32 xyzToConstrain)
{
	if (isTranslationOrRotation == 0)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (i == xyzToConstrain) //this if-block will only execute 1 time in the for loop
			{
				FVector currLoc = sc->GetRelativeTransform().GetLocation();
				FVector newLoc = currLoc;

				//for debugging
				/*UE_LOG(LogTemp, Warning, TEXT("	startPos is %s"), *startPos.ToString());
				UE_LOG(LogTemp, Warning, TEXT("	currLoc is %s"), *currLoc.ToString());

				UE_LOG(LogTemp, Warning, TEXT("	newLoc[i] before clamp is %f"), newLoc[i]);*/

				newLoc[i] = FMath::Clamp(newLoc[i], startPos[i] + MaxTranslationNegative[i], startPos[i] + MaxTranslationPositive[i]);

			/*	UE_LOG(LogTemp, Warning, TEXT("	startPos[i] + MaxTranslationNegative[i] is %f"), startPos[i] + MaxTranslationNegative[i]);
				UE_LOG(LogTemp, Warning, TEXT("	startPos[i] + MaxTranslationPositive[i] is %f"), startPos[i] + MaxTranslationPositive[i]);
				UE_LOG(LogTemp, Warning, TEXT("	newLoc[i] after clamp is %f"), newLoc[i]);

				UE_LOG(LogTemp, Warning, TEXT("	newLoc is %s"), *newLoc.ToString());*/

				//sc->AddRelativeLocation(newLoc - currLoc); //this only seems to work after you let go
				sc->SetRelativeLocation(newLoc); //what the FUCK do i need to do to get this to work properly
			}
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			if (i == xyzToConstrain) //this if-block will only execute 1 time in the for loop
			{
				FRotator currRot = sc->GetRelativeTransform().GetRotation().Rotator();
				FVector currRotVec = FVector(currRot.Pitch, currRot.Yaw, currRot.Roll);
				FVector newRotVec = currRotVec;

				FVector startRotVec = FVector(startRot.Pitch, startRot.Yaw, startRot.Roll);
				FVector MaxRotationPositiveVec = FVector(MaxRotationPositive.Pitch, MaxRotationPositive.Yaw, MaxRotationPositive.Roll);
				FVector MaxRotationNegativeVec = FVector(MaxRotationNegative.Pitch, MaxRotationNegative.Yaw, MaxRotationNegative.Roll);

				newRotVec[i] = FMath::Clamp(newRotVec[i], startRotVec[i] + MaxRotationNegativeVec[i], startRotVec[i] + MaxRotationPositiveVec[i]);

				sc->SetRelativeRotation(FRotator(newRotVec.X, newRotVec.Y, newRotVec.Z).Quaternion());
			}
		}
	}
}

void UTransformConstraintBehavior::SetCurrentPosAsReference()
{
	if (AffectedComponents.Num() > 0)
	{
		auto comp = Cast<USceneComponent>(AffectedComponents[0]);
		if (comp == nullptr)
		{
			return;
		}

		startPos = comp->GetRelativeTransform().GetLocation();
	}
}

void UTransformConstraintBehavior::SetCurrentRotAsReference()
{
	if (AffectedComponents.Num() > 0)
	{
		auto comp = Cast<USceneComponent>(AffectedComponents[0]);
		if (comp == nullptr)
		{
			return;
		}

		startRot = comp->GetRelativeTransform().GetRotation().Rotator();
	}
}

