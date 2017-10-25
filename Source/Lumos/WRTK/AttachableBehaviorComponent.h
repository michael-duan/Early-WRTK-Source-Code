// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WRTK/BehaviorComponent.h"
#include "AttachableBehaviorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttachDelegate, AActor*, HitActor, UAttachableBehaviorComponent*, ToAttach);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOtherAttachDelegate, AActor*, HitActor, UAttachableBehaviorComponent*, ToAttach);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDetachDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOtherDetachDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUMOS_API UAttachableBehaviorComponent : public UBehaviorComponent
{
	GENERATED_BODY()

public:	
	UAttachableBehaviorComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Collision handlers
	UFUNCTION()
	void TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// For attaching to other attachable components
	void Attach();
	void Detach();

	// For being attached
	void GetAttached();
	void GetDetached();

	bool IsAttached() { return bIsAttached; }

	/* The name of the collider for the anchor point. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttachableComponent)
	FString CollisionShapeName;

	/* 
	 * The name of parent scene component for other attachable componenet to attach to. If this is not set,
	 * then this attachable componenet can not be attached by other attachable component.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttachableComponent)
	TArray<FName> AnchorPointTags;

	/* 
	 * A list of tags that are used to specify which other attachable components are able to attach to.
	 * If no tag is specified, then by default this is able to attach to any attachable component.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttachableComponent)
	TArray<FName> AttachProlie;

	/* The haptic effect played when attached. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HapticFeedback)
	class UHapticFeedbackEffect_Base* DefaultHapticEffect;

	/* Sale for playing haptic effect. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HapticFeedback)
	float InitialFrameAmplitude = 0.0f;

	/* Whether this attachable component attach to another attachable component when they collide. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttachableComponent)
	bool bAttachWhenTouch;

	/* The distance that object needs to be pull to detach. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttachableComponent)
	float PullOffDistance = 15.0f;

	/* The delegate get called when on attached. */
	UPROPERTY(BlueprintAssignable, Category = AttachableComponent)
	FOnAttachDelegate OnAttach;

	/* The delegate get called when on attached. */
	UPROPERTY(BlueprintAssignable, Category = AttachableComponent)
	FOnOtherAttachDelegate OnOtherAttach;

	/* The delegate get called when detached. */
	UPROPERTY(BlueprintAssignable, Category = AttachableComponent)
	FOnDetachDelegate OnDetach;

	/* The delegate get called when detached. */
	UPROPERTY(BlueprintAssignable, Category = AttachableComponent)
	FOnOtherDetachDelegate OnOtherDetach;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// override behavior functions.
	virtual void PostOnGrab(class UWRTK_InteractManager* IM, class IGrabShapeInterface* GrabInterface) override;

private:
	float alpha;
	bool bIsAttached;
	bool bNeedToMove;
	AActor* HitActor;
	TArray<USceneComponent*> AnchorPoints;
	TArray<class UPrimitiveComponent*> physicsOffComps;
	class AWRTK_HandActor* Hand;
	class UWRTK_InteractManager * PrevIM;
	class IGrabShapeInterface * PrevGrabInterface;
	UAttachableBehaviorComponent* ToAttach;
	UPrimitiveComponent* CollisionShape;
};