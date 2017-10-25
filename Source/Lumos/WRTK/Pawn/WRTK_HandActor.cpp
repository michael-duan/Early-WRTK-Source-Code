// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "WRTK_InteractManager.h"
#include "WRTK_HandAnimInstance.h"
#include "WRTK_HandActor.h"
#include "WRTK_MasterPawn.h"


// Sets default values
AWRTK_HandActor::AWRTK_HandActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));

	GrabSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphereComponent"));
	GrabSphereComponent->SetupAttachment(HandMesh);

	PinchSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PinchSphereComponent"));
	PinchSphereComponent->SetupAttachment(HandMesh);
	
	TempGrabScene = CreateDefaultSubobject<USphereComponent>(TEXT("TempGrabScene"));
	TempGrabScene->SetupAttachment(HandMesh);

	FistSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("FistSphereComponent"));
	FistSphereComponent->SetupAttachment(HandMesh);

	PhysicsHandleComponent = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandleComponent"));
	PhysicsHandleComponent->InterpolationSpeed = PhysicsHandleBaseInterpSpeed;

	InteractManagerComponent = CreateDefaultSubobject<UWRTK_InteractManager>(TEXT("InteractManagerComponent"));

	// pass the hand actor's sphere component into the interact manager's reference to a sphere component


	GrabSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWRTK_HandActor::OnGrabOverlapBegin);        // set up a notification for when this component overlaps something
	GrabSphereComponent->OnComponentEndOverlap.AddDynamic(this, &AWRTK_HandActor::OnGrabOverlapEnd);      // set up a notification for when this component overlaps something

	PinchSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWRTK_HandActor::OnPinchOverlapBegin);        // set up a notification for when this component overlaps something
	PinchSphereComponent->OnComponentEndOverlap.AddDynamic(this, &AWRTK_HandActor::OnPinchOverlapEnd);      // set up a notification for when this component overlaps something

	UsingKBM = false;
	Phi = 0;
	Theta = 0;
	Radius = 0;
	KBMYaw = 0.0f;
	KBMPitch = 0.0f;
	KBMRoll = 0.0f;
}

// Called when the game starts or when spawned
void AWRTK_HandActor::BeginPlay()
{
	Super::BeginPlay();

	// pass the hand actor's anim instance
	InteractManagerComponent->HandAnimInstance = Cast<UWRTK_HandAnimInstance>(HandMesh->GetAnimInstance());
	InteractManagerComponent->GrabSphereComponent = GrabSphereComponent;
	InteractManagerComponent->PinchSphereComponent = PinchSphereComponent;


	HandMeshMaterialInstance = UMaterialInstanceDynamic::Create(HandMesh->GetMaterial(0), this);
	HandMesh->SetMaterial(0, HandMeshMaterialInstance);

	InteractManagerComponent->HandActor = this;
}

bool AWRTK_HandActor::IsRightHand()
{
	if (HandName == "RightHand")
		return true;
	else
		return false;
}

// Called every frame
void AWRTK_HandActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UsingKBM && MovedSinceTick)
	{
		// Update the position from the spherical coordinates.
		FVector origin = OwningMasterPawn->GetActorLocation();

		// Calculate displacement in each direction
		float dx = Radius*FMath::Sin(Phi)*FMath::Cos(Theta);
		float dy = Radius*FMath::Sin(Phi)*FMath::Sin(Theta);
		float dz = Radius*FMath::Cos(Phi);
		FVector displacement(dx, dy, dz);

		// Update orientation and position
		SetActorLocation(origin + displacement);
		SetActorRotation(FRotator(KBMPitch, KBMYaw, KBMRoll));
		
		MovedSinceTick = false;
	}

	if (bShouldInterpToGhostHand)
	{
		FVector GhostHandLocation = HandMesh->GetComponentLocation();
		FVector DummyHandLocation = DummyHand->GetComponentLocation();

		DummyHand->SetWorldLocation(FMath::VInterpTo(DummyHandLocation, GhostHandLocation, DeltaTime, DummyHandInterpSpeed));
		
		FRotator GhostHandRotator = HandMesh->GetComponentRotation();
		FRotator DummyHandRotator = DummyHand->GetComponentRotation();

		DummyHand->SetWorldRotation(FMath::RInterpTo(DummyHandRotator, GhostHandRotator, DeltaTime, DummyHandInterpSpeed));

		if (DummyHandLocation.Equals(GhostHandLocation, 1))
		{
			bShouldInterpToGhostHand = false;
			DummyHand->SetWorldLocationAndRotation(GhostHandLocation, GhostHandRotator);
			DummyHand->AttachToComponent(HandSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		}
	}


	if (bShouldKeepRotation)
	{
		FRotator GhostHandRotator = HandMesh->GetComponentRotation();

		if (bTempReplaceThis == true)
		{
			DummyHand->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			bTempReplaceThis = false;
		}

		DummyHand->SetWorldRotation(GhostHandRotator);
	}
}

void AWRTK_HandActor::OnGrabOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(InteractManagerComponent)
		InteractManagerComponent->OnGrabOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWRTK_HandActor::OnGrabOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (InteractManagerComponent)
		InteractManagerComponent->OnGrabOverlapEnd(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}

void AWRTK_HandActor::OnPinchOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (InteractManagerComponent)
		InteractManagerComponent->OnPinchOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWRTK_HandActor::OnPinchOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (InteractManagerComponent)
		InteractManagerComponent->OnPinchOverlapEnd(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}

void AWRTK_HandActor::EnableKBM()
{
	UsingKBM = true;

	// Reasonable defaults for hand positions.
	Phi = FMath::DegreesToRadians(90.0f); // Along Equator
	Radius = 40.0f; // Slightly away from body.
	Theta = IsRightHand() ? 35.0f : -35.0f; // Shifted toward side
	Theta = FMath::DegreesToRadians(Theta);

	// Default to no rotation
	KBMYaw = 0.0f;
	KBMPitch = 0.0f;
	KBMRoll = IsRightHand() ? 0.0f : 180.0f;
}

void AWRTK_HandActor::UpdatePhi(float amount)
{
	if (!UsingKBM) return;

	Phi += FMath::DegreesToRadians(amount);
	KBMPitch -= amount; // Adjust the pitch to flow with movement
	MovedSinceTick = true;
}

void AWRTK_HandActor::UpdateTheta(float amount)
{
	if (!UsingKBM) return;
	Theta += FMath::DegreesToRadians(amount);
	KBMYaw += amount; // Adjust the yaw to flow with movement
	MovedSinceTick = true;
}

void AWRTK_HandActor::UpdateRadius(float amount)
{
	if (!UsingKBM) return;
	Radius += 3 * amount;
	if (Radius < 1.0f) Radius = 1.0f; // Don't let radius be negative. Doesn't make sense.
	MovedSinceTick = true;
}

void AWRTK_HandActor::UpdateKBMYaw(float amount)
{
	KBMYaw += amount;
	MovedSinceTick = true;
}

void AWRTK_HandActor::UpdateKBMPitch(float amount)
{
	KBMPitch += amount;
	MovedSinceTick = true;
}

void AWRTK_HandActor::UpdateKBMRoll(float amount)
{
	KBMRoll += 5*amount;
	MovedSinceTick = true;
}

void AWRTK_HandActor::AttachDummyHandToComponent(bool bShouldAttach, USceneComponent* OtherComp, FAttachmentTransformRules Rules)
{
	if (DummyHand == nullptr)
	{
		return;
	}

	if (bShouldAttach)
	{
		DummyHand->AttachToComponent(OtherComp, Rules);
		bShouldInterpToGhostHand = false;
	}
	else
	{
		DummyHand->DetachFromParent(true);
		bShouldInterpToGhostHand = true;
		bShouldKeepRotation = false;
		//DummyHand->AttachToComponent(HandSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}
