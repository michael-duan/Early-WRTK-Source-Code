// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "LeverBehavior.h"

#include "Pawn/WRTK_InteractManager.h"
#include "Pawn/WRTK_HandActor.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "GrabSphereCollider.h"
#include "WRTK_Math.h"

#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

void ULeverBehavior::SearchForKnownComponents()
{
	Super::SearchForKnownComponents();

	//find handle and base locations
	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UActorComponent::StaticClass());
	for (auto& c : comps)
	{
		auto sc = Cast<USceneComponent>(c);
		if (sc == nullptr) //if component is not a scenecomponent
		{
			continue;
		}

		if (sc->GetName() == LeverBaseName)
		{
			Base = sc;
		}
		else if (sc->GetName() == LeverHandleName)
		{
			Handle = sc;
		}
		else if (sc->GetName() == CircleNormalMarkerName)
		{
			CircleNormalMarker = sc;
		}
	}
}


void ULeverBehavior::PostOnGrab(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnGrab(im, shape);

	im->HandAnimInstance->SetHoldingObject(true, shape->GetGrabAnim());

	OnAttach(im, shape);
}

void ULeverBehavior::PostOnPinch(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnPinch(im, shape);

	im->HandAnimInstance->SetHoldingObject(true, shape->GetPinchAnim());

	OnAttach(im, shape);
}

void ULeverBehavior::OnAttach(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	if (Handle == nullptr)
	{
		Handle = Cast<USceneComponent>(shape);
	}

	UPrimitiveComponent* c = Cast<UPrimitiveComponent>(Base);
	if (c)
		c->SetSimulatePhysics(false);

	FVector handLoc = GetStrongestInteractor()->HandActor->GrabSphereComponent->GetComponentLocation();
	FVector markerLoc = WRTK_Math::ProjectPointOntoCircle(Base->GetComponentLocation(), Handle->GetComponentLocation(), CircleNormalMarker->GetForwardVector(), handLoc);

	if (grabLocMarker != nullptr)
	{
		grabLocMarker->DestroyComponent();
		grabLocMarker = nullptr;
	}

	grabLocMarker = NewObject<USceneComponent>(Base);
	grabLocMarker->AttachToComponent(Base, FAttachmentTransformRules::KeepRelativeTransform);
	grabLocMarker->RegisterComponent();
	grabLocMarker->SetWorldLocation(markerLoc);

	if (bShouldAttachHand)
		im->HandActor->AttachDummyHandToComponent(true, Cast<USceneComponent>(AffectedComponents[0]), FAttachmentTransformRules::KeepWorldTransform);

}

void ULeverBehavior::OnRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	if (!shape)
		return;

	if (shape->GetShouldHaveGravityWhenReleased())
	{
		UPrimitiveComponent* c = Cast<UPrimitiveComponent>(Base);
		if (c)
			c->SetSimulatePhysics(true);
	}

	im->HandAnimInstance->SetHoldingObject(false, shape->GetPinchAnim());

	if(bShouldAttachHand)
		im->HandActor->AttachDummyHandToComponent(false, Cast<USceneComponent>(AffectedComponents[0]), FAttachmentTransformRules::KeepWorldTransform);

}

void ULeverBehavior::PostOnGrabRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnGrabRelease(im, shape);

	OnRelease(im, shape);
}

void ULeverBehavior::PostOnPinchRelease(class UWRTK_InteractManager* im, class IGrabShapeInterface* shape)
{
	Super::PostOnPinchRelease(im, shape);

	OnRelease(im, shape);
}

// Called every frame
void ULeverBehavior::BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::BehaviorTickComponent(DeltaTime, TickType, ThisTickFunction);

	//if not held, do nothing
	if (IsHeld() == false)
	{
		return;
	}

	if (CircleNormalMarker == nullptr || Handle == nullptr || Base == nullptr)
	{
		return;
	}
	
	//if held, move with controller (currently not with respect to deltatime)

	FVector baseLoc = Base->GetComponentLocation();
	FVector controllerLoc = GetStrongestInteractor()->HandActor->GrabSphereComponent->GetComponentLocation();

	AWRTK_HandActor* GrabbingHand = GetStrongestInteractor()->HandActor;
	FVector pointOnCircle = WRTK_Math::ProjectPointOntoCircle(baseLoc, Handle->GetComponentLocation(), CircleNormalMarker->GetForwardVector(), controllerLoc);
	
	USceneComponent* tempComp = Cast<USceneComponent>(AffectedComponents[0]);
	if (tempComp)
	{
		FVector forwardX = pointOnCircle - baseLoc;
		forwardX.Normalize();

		FVector upZ = CircleNormalMarker->GetForwardVector();

		//FRotator Rot = UKismetMathLibrary::MakeRotFromXZ(forwardX, upZ);

		FRotator Rot = UKismetMathLibrary::MakeRotationFromAxes(forwardX, CircleRight, upZ);

		if (ShouldSnapToHand == true)
		{
			Rot = FMath::RInterpTo(tempComp->GetComponentRotation(), Rot, DeltaTime, InterpSpeed);
			tempComp->SetWorldRotation(Rot, false, nullptr, ETeleportType::None);
		}
		else
		{
			float angleFromGrabLocToProjPoint = WRTK_Math::GetAngleBetweenTwoPointsOnACircle(tempComp->GetComponentLocation(), CircleNormalMarker->GetForwardVector(), pointOnCircle, grabLocMarker->GetComponentLocation());
			FVector projectedForwardVec = tempComp->GetForwardVector().RotateAngleAxis(angleFromGrabLocToProjPoint, CircleNormalMarker->GetForwardVector());
			FRotator rot = UKismetMathLibrary::MakeRotFromXZ(projectedForwardVec, CircleNormalMarker->GetForwardVector());
			
			//interpolate rotation
			rot = FMath::RInterpTo(tempComp->GetComponentRotation(), rot, DeltaTime, InterpSpeed);

			tempComp->SetWorldRotation(rot);

			//for debugging
			//float dist = FVector::Dist(Handle->GetComponentLocation(), tempComp->GetComponentLocation());
			//DrawDebugSphere(GetWorld(), tempComp->GetComponentLocation() + projectedForwardVec * dist, 8.0f, 5, FColor::Red);
		}

		//for debugging
		//UE_LOG(LogTemp, Warning, TEXT("rotation is %s"), *tempComp->GetComponentRotation().ToString());

	}

	//for debugging
	/*
	DrawDebugSphere(GetWorld(), pointOnCircle, 8.0f, 5, FColor::Red);
	DrawDebugSphere(GetWorld(), grabLocMarker->GetComponentLocation(), 8.0f, 5, FColor::Green);
	*/
}