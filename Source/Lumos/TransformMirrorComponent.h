// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TransformMirrorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUMOS_API UTransformMirrorComponent : public UActorComponent
{

public:	
	GENERATED_BODY()

	// Sets default values for this component's properties
	UTransformMirrorComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformMirror)
	class AActor* ThisActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformMirror)
	FString ThisComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformMirror)
	class AActor* OtherActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TransformMirror)
	FString OtherComponentName;

	UPROPERTY(BlueprintReadWrite, Category = TransformMirror)
	class USceneComponent* ThisComponent;

	UPROPERTY(BlueprintReadWrite, Category = TransformMirror)
	class USceneComponent* OtherComponent;

	bool bReady = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
	bool MirrorX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
	bool MirrorY;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
	bool MirrorZ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
		float XScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
		float YScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
	float ZScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
	bool MirrorPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
	bool MirrorRoll;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
	bool MirrorYaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
	bool YawToZ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
	float YawToZScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TransformMirror)
	float YawMirrorScale;

	FVector OldPosition;
	FQuat OldRotation;

	virtual void SearchForKnownComponents();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
