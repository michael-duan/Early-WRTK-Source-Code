// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "PickUpBehaviorComponent.h"
#include "Pawn/WRTK_InteractManager.h"	
#include "GrabbableComponent.h"
#include "MotionControllerComponent.h"

#include "GrabSphereCollider.h"
#include "Pawn/WRTK_HandActor.h"
#include "WRTK/HapticFeedbackBehavior.h"

UPickUpBehaviorComponent::UPickUpBehaviorComponent()
{
}

void UPickUpBehaviorComponent::PostOnGrab(UWRTK_InteractManager* im, IGrabShapeInterface* coll)
{
	Super::PostOnGrab(im, coll);

	if (bIsBeingGrabbed || bIsBeingPinched || im == nullptr || coll == nullptr)
		return;

	im->HandAnimInstance->SetHoldingObject(true, coll->GetGrabAnim());
	bIsBeingGrabbed = true;
	AttachToHand(true, im->HandActor, coll);
}

void UPickUpBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> comps = GetOwner()->GetComponentsByClass(UHapticFeedbackBehavior::StaticClass());
	for (auto& c : comps)
	{
		auto hf = Cast<UHapticFeedbackBehavior>(c);
		OnAddHapticFeedback.AddDynamic(hf, &UHapticFeedbackBehavior::AddHapticCurrentFrameAmplitude);
	}

	for (auto& c : AffectedComponents)
	{
		UPrimitiveComponent* pc = Cast<UPrimitiveComponent>(c);
		if (pc != nullptr)
		{
			pc->OnComponentHit.AddDynamic(this, &UPickUpBehaviorComponent::OnHit);
		}
	}
}

void UPickUpBehaviorComponent::PostOnGrabRelease(UWRTK_InteractManager* im, IGrabShapeInterface* coll)
{
	Super::PostOnGrabRelease(im, coll);

	bIsBeingGrabbed = false;
	DetachFromHand(im, coll);
}

USceneComponent* UPickUpBehaviorComponent::GetTopMostParent()
{
	if (bShouldMoveOnlyThisActor)
	{
		return Cast<USceneComponent>(AffectedComponents[0]);
	}
	else
	{
		USceneComponent* TopMostParent = Cast<USceneComponent>(AffectedComponents[0]);
		while (TopMostParent->GetAttachParent() != nullptr)
		{
			TopMostParent = TopMostParent->GetAttachParent();
		}

		return TopMostParent;
	}

	return nullptr;
}

void UPickUpBehaviorComponent::PostOnPinch(UWRTK_InteractManager* im, IGrabShapeInterface* coll)
{
	Super::PostOnPinch(im, coll);

	if (bIsBeingGrabbed || bIsBeingPinched)
		return;

	im->HandAnimInstance->SetHoldingObject(true, coll->GetPinchAnim());
	bIsBeingPinched = true;
	AttachToHand(false, im->HandActor, coll);
}

void UPickUpBehaviorComponent::PostOnPinchRelease(UWRTK_InteractManager* im, IGrabShapeInterface* coll)
{
	Super::PostOnPinchRelease(im, coll);
	bIsBeingPinched = false;
	DetachFromHand(im, coll);
}

// Attach the object to the hand
void UPickUpBehaviorComponent::AttachToHand(bool bIsGrab, AWRTK_HandActor* hand, IGrabShapeInterface* coll)
{
	if (coll == nullptr || AffectedComponents.Num() > 0 && AffectedComponents[0] == nullptr)
	{
		return;
	}

	SavedColl = coll;
	GrabbingHand = hand;

	USceneComponent* sc = Cast<UPrimitiveComponent>(AffectedComponents[0]);
	if (sc == nullptr)
	{
		return;
	}

	//choose the right socket for grab/pinch and hand
	FName socket;
	if (bIsBeingGrabbed == true)
	{
		if (GrabbingHand->IsRightHand() == true)
		{
			socket = SavedColl->GetRGrabSocket();
		}
		else
		{
			socket = SavedColl->GetLGrabSocket();
		}
	}
	else //we are being pinched
	{
		if (GrabbingHand->IsRightHand() == true)
		{
			socket = SavedColl->GetRPinchSocket();
		}
		else
		{
			socket = SavedColl->GetLPinchSocket();
		}
	}

	if (coll->GetShouldSnapToSocket() == true)
	{
		GrabbingHand->TempGrabScene->SetWorldLocationAndRotation(sc->GetSocketLocation(socket), sc->GetSocketRotation(socket));
	}
	else
	{
		FTransform dummyTransform = sc->GetComponentTransform();

		//we might have grabbed the object far from the actual hand mesh, so we want to act as if we grabbed it at the nearest point on the hand mesh
		if (ShouldMoveGrabLocationOntoHand == true && GrabbingHand->HandMesh != nullptr)
		{
			FVector closestGrabLocOnHandMesh;
			GrabbingHand->HandMesh->GetClosestPointOnCollision(dummyTransform.GetLocation(), closestGrabLocOnHandMesh);
			dummyTransform.SetLocation(closestGrabLocOnHandMesh);
		}

		//workaround because left hand is right hand but with z *= -1
		if (GrabbingHand->IsRightHand() == false && LeftHandIsRescaledRightHand == true)
		{
			dummyTransform.SetScale3D(FVector(dummyTransform.GetScale3D().X, dummyTransform.GetScale3D().Y, dummyTransform.GetScale3D().Z * -1.0f));
		}

		//we will use the tempgrabscene component to determine where to move the object
		GrabbingHand->TempGrabScene->SetWorldTransform(dummyTransform);
	}

	for (UActorComponent* ac : AffectedComponents)
	{
		UPrimitiveComponent* pc = Cast<UPrimitiveComponent>(ac);
		if (pc != nullptr)
		{
			pc->SetSimulatePhysics(false);
		}
	}

	//attach the dummy hand to the component
	if (bShouldAttachHand == true && GrabbingHand != nullptr)
	{
		GrabbingHand->AttachDummyHandToComponent(true, sc, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void UPickUpBehaviorComponent::DetachFromHand(UWRTK_InteractManager* im, IGrabShapeInterface* coll)
{
	if (coll == nullptr || im == nullptr)
	{
		return;
	}

	if (GrabbingHand->PhysicsHandleComponent->GrabbedComponent != nullptr)
	{
		GrabbingHand->PhysicsHandleComponent->InterpolationSpeed = GrabbingHand->PhysicsHandleBaseInterpSpeed;
		GrabbingHand->PhysicsHandleComponent->ReleaseComponent();
	}

	if(bShouldAttachHand)
		GrabbingHand->AttachDummyHandToComponent(false, nullptr, FAttachmentTransformRules::KeepWorldTransform);
	GrabbingHand = nullptr;

	im->HandAnimInstance->SetHoldingObject(false, coll->GetPinchAnim());

	for (UActorComponent* ac : AffectedComponents)
	{
		UPrimitiveComponent* pc = Cast<UPrimitiveComponent>(ac);
		if (pc != nullptr)
		{
			if (coll->GetShouldHaveGravityWhenReleased())
			{
				pc->SetSimulatePhysics(true);
			}
		}
	}
}

void UPickUpBehaviorComponent::BehaviorTickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::BehaviorTickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GrabbingHand == nullptr)
	{
		return;
	}

	GrabbingHand->PhysicsHandleComponent->SetTargetLocationAndRotation(GrabbingHand->TempGrabScene->GetComponentLocation(), GrabbingHand->TempGrabScene->GetComponentRotation());

	FVector finalLoc;
	FRotator finalRot;

	//choose the right socket for grab/pinch and hand
	FName socket;
	if (bIsBeingGrabbed == true)
	{
		if (GrabbingHand->IsRightHand() == true)
		{
			socket = SavedColl->GetRGrabSocket();
		}
		else
		{
			socket = SavedColl->GetLGrabSocket();
		}
	}
	else //we are being pinched
	{
		if (GrabbingHand->IsRightHand() == true)
		{
			socket = SavedColl->GetRPinchSocket();
		}
		else
		{
			socket = SavedColl->GetLPinchSocket();
		}
	}

	if (SavedColl->GetShouldSnapToSocket())
	{
		finalLoc = GrabbingHand->HandMesh->GetSocketLocation(socket);
	}
	else
	{
		if (bPhysDragTowardsHand == true)
		{
			GrabbingHand->TempGrabScene->SetWorldLocationAndRotation(GrabbingHand->GrabSphereComponent->GetComponentLocation(), GrabbingHand->GrabSphereComponent->GetComponentRotation());
		}
		finalLoc = GrabbingHand->TempGrabScene->GetComponentLocation();
	}

	if (bShouldRotate == true)
	{
		FTransform tempTransform = GrabbingHand->TempGrabScene->GetComponentTransform();
		if (SavedColl->GetShouldSnapToSocket() == true)
		{
			tempTransform = GrabbingHand->HandMesh->GetSocketTransform(socket);
		}

		finalRot = tempTransform.Rotator();
	}

	for (UActorComponent* ac : AffectedComponents)
	{
		USceneComponent* sc = Cast<USceneComponent>(ac);
		if (bShouldRotate == false)
		{
			finalRot = sc->GetComponentRotation();
		}

		//check if there's a line of spheres from the object to the hand; if so, teleport the object to the hand; otherwise, turn on the physics handle
		FHitResult hitResult;
		FCollisionShape sweepSphere = FCollisionShape::MakeSphere(SweepSphereRadius);
		FCollisionQueryParams sweepParams;
		
		//ignore self in sweep
		sweepParams.AddIgnoredComponent(Cast<UPrimitiveComponent>(sc));

		//ignore children in sweep
		TArray<USceneComponent*> scChildren;
		sc->GetChildrenComponents(true, scChildren);
		TArray<UPrimitiveComponent*> scPrimitiveChildren;
		for (USceneComponent* c : scChildren)
		{
			UPrimitiveComponent* pc = Cast<UPrimitiveComponent>(c);
			if (pc != nullptr)
			{
				scPrimitiveChildren.Add(pc);
			}
		}
		sweepParams.AddIgnoredComponents(scPrimitiveChildren);

		//for debugging
		/*const FName TraceTag("MyTraceTag");
		GrabbingHand->GetWorld()->DebugDrawTraceTag = TraceTag;
		sweepParams.TraceTag = GrabbingHand->GetWorld()->DebugDrawTraceTag;*/

		UPrimitiveComponent* pc = Cast<UPrimitiveComponent>(sc);
		if (pc != nullptr)
		{
			GrabbingHand->GetWorld()->SweepSingleByChannel(hitResult, sc->GetComponentLocation(), GrabbingHand->TempGrabScene->GetComponentLocation(), sc->GetComponentRotation().Quaternion(),
				ECollisionChannel::ECC_PhysicsBody, pc->GetCollisionShape(), sweepParams);
		}
		else
		{
			GrabbingHand->GetWorld()->SweepSingleByChannel(hitResult, sc->GetComponentLocation(), GrabbingHand->TempGrabScene->GetComponentLocation(), sc->GetComponentRotation().Quaternion(),
				ECollisionChannel::ECC_PhysicsBody, sweepSphere, sweepParams);
		}
		
		bool passedSphereSweep = hitResult.Actor == nullptr || hitResult.Actor == GrabbingHand || hitResult.Component == sc;

		//if there's a path to the hand and the object is close to the hand and oriented like the hand, teleport it to the hand
		float translateDistanceSq = FVector::DistSquared(sc->GetComponentLocation(), GrabbingHand->TempGrabScene->GetComponentLocation());
		float rotateDistanceSq = FVector::DistSquared(sc->GetForwardVector(), GrabbingHand->TempGrabScene->GetForwardVector());

		if (ShouldInterpolateInTeleportMode == true &&
			teleportedLastFrame == false &&
			translateDistanceSq > MaxTeleportWithoutInterpolateDistance * MaxTeleportWithoutInterpolateDistance &&
			rotateDistanceSq > MaxTeleportWithoutInterpolateDistance * MaxTeleportWithoutInterpolateDistance)
		{
			//interpolate the location and rotation
			finalLoc = FMath::VInterpTo(sc->GetComponentLocation(), finalLoc, DeltaTime, TransitionFromPhysHandleToTeleportInterpSpeed);
			finalRot = FMath::RInterpTo(sc->GetComponentRotation(), finalRot, DeltaTime, TransitionFromPhysHandleToTeleportInterpSpeed);
		}

		if (ShouldTryToTeleportObjectToHand &&
			objectWasHitRecently == false &&
			(ShouldSweepPathFromObjectToHand == false || passedSphereSweep == true) &&
			(ShouldUsePhysicsHandleIfObjectIsFarOrRotated == false ||
				translateDistanceSq < MinPhysicsHandleTranslateDistance * MinPhysicsHandleTranslateDistance &&
				rotateDistanceSq < MinPhysicsHandleRotateDistance * MinPhysicsHandleRotateDistance))
		{
			//for debugging
			//DrawDebugSphere(GetWorld(), sc->GetComponentLocation(), 8.0f, 3, FColor::Green);

			//if we have a physics handle, let go
			if (GrabbingHand != nullptr && GrabbingHand->PhysicsHandleComponent->GrabbedComponent != nullptr)
			{
				//release physics handle if we have one
				pc = Cast<UPrimitiveComponent>(sc);
				if (pc != nullptr)
				{
					pc->SetSimulatePhysics(false);
				}
				GrabbingHand->PhysicsHandleComponent->ReleaseComponent();

				//for debugging
				//UE_LOG(LogTemp, Warning, TEXT("	just released with physics handle"));
			}
			
			//teleport the object to the hand
			FTransform preSweepTransform = sc->GetComponentTransform();
			sc->SetWorldLocationAndRotation(finalLoc, finalRot, ShouldSweepTeleportDestination, &hitResult);
			if (ShouldSweepTeleportDestination && hitResult.bBlockingHit == true)
			{
				sc->SetWorldTransform(preSweepTransform);
				useAndManagePhysHandle(DeltaTime, sc, translateDistanceSq, finalLoc, finalRot);
				NoteRecentHit();

				teleportedLastFrame = false;
			}
			else
			{
				teleportedLastFrame = true;
			}
		}
		else
		{
			useAndManagePhysHandle(DeltaTime, sc, translateDistanceSq, finalLoc, finalRot);

			if (ShouldSweepPathFromObjectToHand == true && passedSphereSweep == false)
			{
				NoteRecentHit();
			}

			teleportedLastFrame = false;
		}
	}
}

void UPickUpBehaviorComponent::useAndManagePhysHandle(float DeltaTime, USceneComponent* sc, float translateDistanceSq, FVector goalPos, FRotator goalRot)
{
	//for debugging
	//DrawDebugSphere(GetWorld(), hitResult.Location, SweepSphereRadius * 5, 5, FColor::Orange);
	//DrawDebugSphere(GetWorld(), hitResult.Location, SweepSphereRadius, 5, FColor::Red);
	//DrawDebugSphere(GetWorld(), GrabbingHand->PhysicsHandleComponent->TargetTransform.GetLocation(), SweepSphereRadius * 2.0f, 10.0f, FColor::Cyan);

	if (GrabbingHand == nullptr)
	{
		return;
	}

	//if we don't have a physics handle, turn it on
	if (GrabbingHand->PhysicsHandleComponent->GrabbedComponent == nullptr)
	{
		UPrimitiveComponent* pc = Cast<UPrimitiveComponent>(sc);
		if (pc != nullptr)
		{
			if (SavedColl->GetShouldHaveGravityWhenReleased())
			{
				pc->SetSimulatePhysics(true);
			}
			GrabbingHand->PhysicsHandleComponent->GrabComponentAtLocationWithRotation(Cast<UPrimitiveComponent>(sc), PhysicsGrabRootBoneName, Cast<UPrimitiveComponent>(sc)->GetComponentLocation(), Cast<UPrimitiveComponent>(sc)->GetComponentRotation());
		}

		OnAddHapticFeedback.Broadcast(GrabAndReleaseHapticAmount);
	}
	else
	{
		if (translateDistanceSq < MinPhysicsHandleTranslateDistance * MinPhysicsHandleTranslateDistance)
		{
			//increase the physhandle interp speed as the object gets closer to the hand
			float InterpolationSpeedAddition = 1.0f;
			if (translateDistanceSq != 0)
			{
				float distanceFraction = MinPhysicsHandleTranslateDistance * MinPhysicsHandleTranslateDistance / translateDistanceSq;
				InterpolationSpeedAddition = FMath::Min(2.0f, distanceFraction);
			}
			float goalInterpSpeed = GrabbingHand->PhysicsHandleComponent->InterpolationSpeed + GrabbingHand->PhysicsHandleComponent->InterpolationSpeed + InterpolationSpeedAddition * DeltaTime;
			GrabbingHand->PhysicsHandleComponent->InterpolationSpeed = FMath::Max(GrabbingHand->PhysicsHandleBaseInterpSpeed, FMath::Min(GrabbingHand->PhysicsHandleBaseInterpSpeed * 100.0f, goalInterpSpeed));

			//for debugging
			//UE_LOG(LogTemp, Warning, TEXT("goal interpspeed is %f and interpspeed is %f"), goalInterpSpeed, GrabbingHand->PhysicsHandleComponent->InterpolationSpeed);
		}
		else
		{
			GrabbingHand->PhysicsHandleComponent->InterpolationSpeed = GrabbingHand->PhysicsHandleBaseInterpSpeed;
		}
	}

	if (ShouldCorrectPhysHandleRotationToMirrorController == true)
	{
		//correct the rotation regardless of what the physics handle does
		sc->SetWorldRotation(goalRot, ShouldSweepTeleportDestination);
	}

	if (ShouldCorrectPhysHandleLocationToMirrorController == true)
	{
		FHitResult hitResult;

		//correct the location regardless of what the physics handle does
		FVector preTeleportLoc = sc->GetComponentLocation();
		sc->SetWorldLocation(goalPos, ShouldSweepTeleportDestination, &hitResult);
		if (hitResult.bBlockingHit == true)
		{
			FVector surfaceLoc;
			hitResult.Component->GetClosestPointOnCollision(GrabbingHand->TempGrabScene->GetComponentLocation(), surfaceLoc);
			sc->SetWorldLocation(surfaceLoc);
		}
	}
	
	//for debugging
	//DrawDebugSphere(GetWorld(), sc->GetComponentLocation(), 5.0f, 3, FColor::Red);
}

void UPickUpBehaviorComponent::OnHit(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit)
{
	NoteRecentHit();
}

void UPickUpBehaviorComponent::NoteRecentHit()
{
	objectWasHitRecently = true;
	GetWorld()->GetTimerManager().SetTimer(objectWasHitRecentlyTimerHandle, this, &UPickUpBehaviorComponent::OnObjectWasHitRecentlyTimer, ObjectWasHitRecentlyTimerMaxTime, false);
}

void UPickUpBehaviorComponent::OnObjectWasHitRecentlyTimer()
{
	objectWasHitRecently = false;
}