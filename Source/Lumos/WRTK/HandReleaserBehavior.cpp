// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "HandReleaserBehavior.h"

#include "WRTK/Pawn/WRTK_InteractManager.h"
#include "WRTK/GrabShapeInterface.h"
#include "WRTK/GrabbableComponent.h"
#include "WRTK/Pawn/WRTK_HandActor.h"
#include "WRTK/HapticFeedbackBehavior.h"

// Sets default values for this component's properties
UHandReleaserBehavior::UHandReleaserBehavior()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHandReleaserBehavior::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UHapticFeedbackBehavior::StaticClass());
	for (auto& c : comps)
	{
		auto hf = Cast<UHapticFeedbackBehavior>(c);
		OnAddHapticFeedback.AddDynamic(hf, &UHapticFeedbackBehavior::AddHapticCurrentFrameAmplitude);
	}
}


// Called every frame
void UHandReleaserBehavior::BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::BehaviorTickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	UWRTK_InteractManager* IM = GetStrongestInteractor();
	if (IM)
	{
		AWRTK_HandActor* GrabbingHand = IM->HandActor;

		float ghostHandOpacity = 0.0f;
		float hapticAmount = 0.0f;
		if (ReleaseShape != nullptr)
		{
			FVector pointOnSurfaceNearestToHand;
			float maxDistance = MaxExpectedHandDistance;
			float handDistance = FVector::Distance(GrabbingHand->HandMesh->GetComponentLocation(), GrabbingHand->DummyHand->GetComponentLocation());
			float percentage = FMath::Min(handDistance / maxDistance, 1.0f);

			//for debugging
			UE_LOG(LogTemp, Warning, TEXT("maxDistance is %f and handDistance is %f"), maxDistance, handDistance);

			if (percentage > MinHandDistanceToShowGhostHands)
			{
				ghostHandOpacity = percentage - GhostHandOpacitySubtractAmount;
				if (GhostHandOpacitySubtractAmount < 1.0f)
				{
					//scale opacity baesd on subtract amount
					ghostHandOpacity *= 1.0f / (1.0f - GhostHandOpacitySubtractAmount);
				}
				ghostHandOpacity = FMath::Min(1.0f, FMath::Max(0.0f, ghostHandOpacity));
			}

			//add haptic effect based on the distance between the ghost hand and the shown hand
			if (percentage > MinHandDistanceToPlayEffect)
			{
				hapticAmount = percentage - HapticPercentageSubtractAmount;
				if (HapticPercentageSubtractAmount < 1.0f)
				{
					//scale opacity baesd on subtract amount
					hapticAmount *= 1.0f / (1.0f - HapticPercentageSubtractAmount);
				}
				hapticAmount = FMath::Min(1.0f, FMath::Max(0.0f, hapticAmount));
			}
		}

		GrabbingHand->HandMeshMaterialInstance->SetScalarParameterValue(TEXT("isSeparated"), ghostHandOpacity);
		OnAddHapticFeedback.Broadcast(hapticAmount);
	}
}

void UHandReleaserBehavior::SearchForKnownComponents()
{
	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (auto c : comps)
	{
		if (c->GetName() == ReleaseShapeName)
		{
			ReleaseShape = Cast<UPrimitiveComponent>(c);
			if (ReleaseShape != nullptr) //if component is not a scenecomponent
			{
				ReleaseShape->OnComponentEndOverlap.AddDynamic(this, &UHandReleaserBehavior::TriggerExit);
				return;
			}
		}
	}
}

void UHandReleaserBehavior::TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//if the hand is holding this behavior
	if (IsHeld() == true)
	{

		float distance = FVector::Dist(HitComp->GetComponentLocation(), OtherComp->GetComponentLocation());
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetHapticsByValue(Frequency, distance * AmplitudeDampingFactor, GetStrongestInteractor()->HandActor->HandEnum);

		//only release if the holding hand actor is the actor that left the release shape
		UWRTK_InteractManager* im = GetStrongestInteractor();
		if (im != nullptr && im->HandActor == Cast<AWRTK_HandActor>(OtherActor))
		{
			GetStrongestInteractionShape()->GetGrabbableComponent()->Release();
		}
	}
}

void UHandReleaserBehavior::PostOnGrab(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnGrab(im, shape);
	OnAttach(im, shape);
}
void UHandReleaserBehavior::PostOnPinch(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnPinch(im, shape);
	OnAttach(im, shape);
}
void UHandReleaserBehavior::PostOnGrabRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnGrabRelease(im, shape);
	OnDetach(im, shape);
}
void UHandReleaserBehavior::PostOnPinchRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnPinchRelease(im, shape);
	OnDetach(im, shape);
}
void UHandReleaserBehavior::OnAttach(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	if(im)
		im->HandActor->HandMeshMaterialInstance->SetScalarParameterValue(TEXT("isHolding"), 1);
}
void UHandReleaserBehavior::OnDetach(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	if(im)
		im->HandActor->HandMeshMaterialInstance->SetScalarParameterValue(TEXT("isHolding"), 0);
}