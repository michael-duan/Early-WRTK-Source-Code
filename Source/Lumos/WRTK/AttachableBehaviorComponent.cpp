// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "WRTK/GrabShapeInterface.h"
#include "WRTK/GrabbableComponent.h"
#include "WRTK/PickUpBehaviorComponent.h"
#include "WRTK/PickUpBehaviorComponent.h"
#include "WRTK/Pawn/WRTK_InteractManager.h"
#include "WRTK/Pawn/WRTK_HandActor.h"
#include "Haptics/HapticFeedbackEffect_Base.h"
#include "AttachableBehaviorComponent.h"

#define DEBUG

#define ATTACHABLE_TAG TEXT("AttachableTag")

// Behavior Switch Identifiers
#define MAIN_GRABCOMP_TAG TEXT("MainGrabForAttachable")
#define SECOND_GRABCOMP_TAG TEXT("SecondGrabForAttachable")
#define THIRD_GRABCOMP_TAG TEXT("ThirdGrabForAttachable")

#define SAME_DIRECTION_TOLERANCE 0.9f
#define SAME_DISTANCE_TOLERANCE 2.0f


// Sets default values for this component's properties
UAttachableBehaviorComponent::UAttachableBehaviorComponent() :
	alpha(0.0f),
	bIsAttached(false),
	bNeedToMove(false),
	HitActor(nullptr),
	ToAttach(nullptr),
	Hand(nullptr),
	PrevIM(nullptr),
	PrevGrabInterface(nullptr)
{

	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAttachableBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();


	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (auto& c : comps)
	{
		if (c->GetName() == CollisionShapeName)
		{
			CollisionShape = Cast<UPrimitiveComponent>(c);
			if (CollisionShape == nullptr) //if component is not a scenecomponent
			{
				break;
			}

			// By default, add AttachableTag
			CollisionShape->ComponentTags.Empty();
			CollisionShape->ComponentTags.Add(ATTACHABLE_TAG);
			// Add any other customized tags
			for (auto& tag : AttachProlie)
			{
				CollisionShape->ComponentTags.Add(tag);
			}

			if (AnchorPointTags.Num() == 0)
			{
				CollisionShape->OnComponentBeginOverlap.AddDynamic(this, &UAttachableBehaviorComponent::TriggerEnter);
				CollisionShape->OnComponentEndOverlap.AddDynamic(this, &UAttachableBehaviorComponent::TriggerExit);
			}
		}
	}

#ifdef DEBUG
	if (CollisionShape == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT(" %s OnBeging: CollisionShape is null!"), *this->GetName());
		IsAsleep = true;
		PrimaryComponentTick.bCanEverTick = false;
		return;
	}
#endif

	if (AnchorPointTags.Num() != 0)
	{
		for (auto& tag : AnchorPointTags)
		{
			comps.Empty();
			comps = GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), tag);
			for (auto& c : comps)
			{
				USceneComponent* anchor = Cast<USceneComponent>(c);
				if (anchor != nullptr)
				{
					AnchorPoints.Add(anchor);
					break;
				}
			}
		}
	}
	else
	{
#ifdef DEBUG
		if (MainGrabComp == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnBeging: MainGrabComp is null! %s"), *MainGrabCompName);
			IsAsleep = true;
			PrimaryComponentTick.bCanEverTick = false;
			CollisionShape->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
			return;
		}

		if (SecondaryGrabComp == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnBeging: SecondaryGrabComp is null! %s"), *SecondaryGrabCompName);
			IsAsleep = true;
			PrimaryComponentTick.bCanEverTick = false;
			CollisionShape->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
			return;
		}
#endif
	}
}

void UAttachableBehaviorComponent::PostOnGrab(UWRTK_InteractManager * IM, IGrabShapeInterface * GrabInterface)
{
	Super::PostOnGrab(IM, GrabInterface);
	if (bIsAttached)
	{
		Hand = IM->HandActor;
		PrevIM = IM;
		PrevGrabInterface = GrabInterface;
	}
}

// Called every frame
void UAttachableBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Hand != nullptr && !bNeedToMove && IsHeld() && bIsAttached)
	{
		if (FVector::Distance(Hand->GrabSphereComponent->GetComponentLocation(), GetOwner()->GetActorLocation()) > PullOffDistance)
		{
			bNeedToMove = true;
			//ToPos = Hand->GrabSphereComponent->GetComponentLocation();
		}
	}

	if (bNeedToMove)
	{
		// already here
		if (FVector::Distance(Hand->GrabSphereComponent->GetComponentLocation(), GetOwner()->GetActorLocation()) < SAME_DISTANCE_TOLERANCE)
		{
			alpha = 0.0f;
			bNeedToMove = false;
			Hand = nullptr;
			Detach();
			if (MainGrabComp != nullptr)
			{
				MainGrabComp->OnGrab(PrevIM, PrevGrabInterface);
			}
			return;
		}

		// move actor linearly
		alpha += DeltaTime;
		FVector pos = FMath::Lerp(GetOwner()->GetActorLocation(), Hand->GrabSphereComponent->GetComponentLocation(), alpha);
		GetOwner()->SetActorLocation(pos);
		
		//TODO: lerp rotation
		//FRotator rot = FMath::Lerp(GetOwner()->GetActorRotation(), Hand->GrabSphereComponent->GetComponentRotation(), alpha);
		//GetOwner()->SetActorRotation(rot);
	}
}

void UAttachableBehaviorComponent::TriggerEnter(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
 {
	if (HitActor == nullptr && OtherComp->ComponentHasTag(ATTACHABLE_TAG) && !bIsAttached)
	{
		if (AttachProlie.Num() != 0)
		{
			for (auto& tag : AttachProlie)
			{
				if (OtherComp->ComponentHasTag(tag))
				{
					HitActor = OtherActor;
					break;
				}
			}
		}
		else 
		{
			HitActor = OtherActor;
		}

		// two attachable components are not compatable
		if (HitActor == nullptr) return;

		// retrive UAttachableBehaviorComponent from other actor
		TArray<UActorComponent*> comps = HitActor->GetComponentsByClass(UAttachableBehaviorComponent::StaticClass());
		for (auto& c : comps)
		{
			ToAttach = Cast<UAttachableBehaviorComponent>(c);
			if (ToAttach != nullptr)
			{
				// SUCCESS
				if (bAttachWhenTouch)
				{
					Attach();
				}
				return;
			}
		}

		// UAttachableBehaviorComponent not found in other actor, FAIL
		ToAttach = nullptr;
		HitActor = nullptr;
	}
}

void UAttachableBehaviorComponent::TriggerExit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (HitActor == OtherActor)
	{
		HitActor = nullptr;
		ToAttach = nullptr;
	}
}

void UAttachableBehaviorComponent::Attach()
{
	if (HitActor == nullptr || ToAttach == nullptr || bIsAttached) return;

	// double Check
	if (ToAttach->IsA(UAttachableBehaviorComponent::StaticClass()) && ToAttach->AnchorPoints.Num() != 0 && !ToAttach->IsAttached())
	{
		//TODO: fix nullptr issue (46825)
		const void* ToAttachAdr = ToAttach;
		const void* HitActorAdr = HitActor;
		USceneComponent* anchorPoint = nullptr;

		// check if rotation match
		for (auto& anchor : ToAttach->AnchorPoints) 
		{
			FVector anchorForward = anchor->GetComponentTransform().GetRotation().GetForwardVector().GetSafeNormal();
			FVector mForward = CollisionShape->GetComponentTransform().GetRotation().GetForwardVector().GetSafeNormal();
			FVector anchorUp = anchor->GetComponentTransform().GetRotation().GetUpVector().GetSafeNormal();
			FVector mUp = CollisionShape->GetComponentTransform().GetRotation().GetUpVector().GetSafeNormal();
			if (FVector::DotProduct(anchorForward, mForward) < SAME_DIRECTION_TOLERANCE ||
				FVector::DotProduct(anchorUp, mUp) < SAME_DIRECTION_TOLERANCE) continue;
			else
			{
				anchorPoint = anchor;
				break;
			}
		}

		// rotation does not match
		if (anchorPoint == nullptr) return;

		//if need to be released by hands
		if (MainGrabComp != nullptr)
		{
			// play haptic effect
			AWRTK_HandActor* handActor = GetStrongestInteractor()->HandActor;
			EControllerHand handEnum = handActor->HandEnum;
			APlayerController* pc = UGameplayStatics::GetPlayerController(handActor->GetWorld(), 0);
			pc->PlayHapticEffect(DefaultHapticEffect, handEnum, InitialFrameAmplitude);

			// release from hands
			MainGrabComp->Release();
			MainGrabComp->Behaviors.Remove(this);
			if (SecondaryGrabComp != nullptr) SecondaryGrabComp->Behaviors.Add(this);
			SwitchToUseSecondaryGrabbable();

			for (int i = 0; i < AffectedComponents.Num(); ++i)
			{
				if (AffectedComponents[i]->IsA(UPrimitiveComponent::StaticClass())
					&& AffectedParents[i]->IsA(USceneComponent::StaticClass()))
				{
					UPrimitiveComponent* pComp = Cast<UPrimitiveComponent>(AffectedComponents[i]);
					USceneComponent* sComp = Cast<USceneComponent>(AffectedParents[i]);
					if (pComp->IsSimulatingPhysics())
					{
						pComp->SetSimulatePhysics(false);
						pComp->AttachToComponent(sComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
						physicsOffComps.Add(pComp);
					}
				}
			}
		}

		GetAttached();
		// TODO: fix nullptr issue (46825)
		ToAttach = (UAttachableBehaviorComponent*)ToAttachAdr;
		HitActor = (AActor*)HitActorAdr;
		GetOwner()->AttachToComponent(anchorPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale); // attaching
		ToAttach->GetAttached(); // notify the other attachable component that two objects are attached
		ToAttach->OnOtherAttach.Broadcast(GetOwner(), this);

	}

	OnAttach.Broadcast(HitActor, ToAttach);
}

void UAttachableBehaviorComponent::Detach()
{
	if (bIsAttached)
	{
		GetOwner()->DetachRootComponentFromParent(true);
		ToAttach->GetDetached();
		ToAttach->OnOtherDetach.Broadcast();
		GetDetached();
		
		// set physics back on
		for (auto toOff : physicsOffComps)
		{
			toOff->SetSimulatePhysics(true);
		}
		physicsOffComps.Reset();

		// grabbable switch
		SecondaryGrabComp->Release();
		SecondaryGrabComp->Behaviors.Remove(this);
		MainGrabComp->Behaviors.Add(this);
		SwitchToUseMainGrabbable();

		OnDetach.Broadcast();
	}
}

void UAttachableBehaviorComponent::GetAttached()
{
	if (!bIsAttached)
	{
		CollisionShape->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision); // turn off collision trigger
		bIsAttached = true;
	}
}

void UAttachableBehaviorComponent::GetDetached()
{
	if (bIsAttached)
	{
		CollisionShape->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		bIsAttached = false;
		HitActor = nullptr;
		ToAttach = nullptr;
	}
}

