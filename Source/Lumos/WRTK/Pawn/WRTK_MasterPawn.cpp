// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "HeadMountedDisplay.h"
#include "HeadMountedDisplayTypes.h"
#include "MotionControllerComponent.h"
#include "Engine.h"
#include "WRTK_HandActor.h"
#include "WRTK_InteractManager.h"
#include "WRTK_MasterPawn.h"
#include "WRTK_PlayerController.h"
#include "WRTK/TeleportMarker.h"


// Sets default values
AWRTK_MasterPawn::AWRTK_MasterPawn()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LeftSocket = CreateDefaultSubobject<USceneComponent>(TEXT("LeftSocket"));
	RightSocket = CreateDefaultSubobject<USceneComponent>(TEXT("RightSocket"));
	CameraNode = CreateDefaultSubobject<USceneComponent>(TEXT("CameraNode"));


	LeftMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	RightMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));

	MotionControllerNode = CreateDefaultSubobject<USceneComponent>(TEXT("MotionControllerNode"));

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));

	LeftMotionController->SetupAttachment(MotionControllerNode);
	RightMotionController->SetupAttachment(MotionControllerNode);

	LeftSocket->SetupAttachment(LeftMotionController);
	RightSocket->SetupAttachment(RightMotionController);

	MainCamera->SetupAttachment(CameraNode);

	LeftHandDummy = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandDummy"));
	RightHandDummy = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandDummy"));

	LeftHandDummy->SetupAttachment(LeftSocket);
	RightHandDummy->SetupAttachment(RightSocket);

	LeftHandRenderMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandRenderMesh"));
	RightHandRenderMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandRenderMesh"));

	LeftHandRenderMesh->SetupAttachment(LeftSocket);
	RightHandRenderMesh->SetupAttachment(RightSocket);

}

// Called when the game starts or when spawned
void AWRTK_MasterPawn::BeginPlay()
{
	Super::BeginPlay();

	LeftHandActor = GetWorld()->SpawnActor<AWRTK_HandActor>(HandClass, FVector(0, 0, 0), FRotator(0, 0, 0));
	RightHandActor = GetWorld()->SpawnActor<AWRTK_HandActor>(HandClass, FVector(0, 0, 0), FRotator(0, 0, 0));

	LeftHandActor->OwningMasterPawn = this;
	RightHandActor->OwningMasterPawn = this;
	
	LeftHandActor->HandEnum = EControllerHand::Left;
	RightHandActor->HandEnum = EControllerHand::Right;

	LeftHandActor->HandName = "LeftHand";
	RightHandActor->HandName = "RightHand";

	LeftHandActor->DummyHand = LeftHandDummy;
	RightHandActor->DummyHand = RightHandDummy;

	LeftHandActor->HandSocket = LeftSocket;
	RightHandActor->HandSocket = RightSocket;

	RightHandActor->AttachToComponent(RightSocket, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true));
	LeftHandActor->AttachToComponent(LeftSocket, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true));

	// set the master pose of the dummy hands to the actual hand
	LeftHandDummy->SetMasterPoseComponent(LeftHandActor->HandMesh);
	RightHandDummy->SetMasterPoseComponent(RightHandActor->HandMesh);

	LeftHandRenderMesh->SetMasterPoseComponent(LeftHandActor->HandMesh);
	RightHandRenderMesh->SetMasterPoseComponent(RightHandActor->HandMesh);

	
	if (!GEngine->HMDDevice.IsValid())
	{
		// If we do not have the Oculus we can't use the motion controllers or HMD tracking
		LeftMotionController->DestroyComponent();
		RightMotionController->DestroyComponent();
		CameraNode->DestroyComponent();

		// Move the Camera to an appropriate height to start play
		if (MainCamera) MainCamera->SetRelativeLocation(FVector(0, 0, 20)); // Center within the pawn by default

		// Enable KBM Controls for hands
		RightHandActor->EnableKBM();
		LeftHandActor->EnableKBM();
	}
	else {
		LeftHandActor->OwningMotionController = LeftMotionController;
		RightHandActor->OwningMotionController = RightMotionController;
	}

}

// Called every frame
void AWRTK_MasterPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWRTK_MasterPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// check what kind of HMD has been connected
	if (GEngine->HMDDevice.IsValid())
	{
		EHMDDeviceType::Type HMDType = GEngine->HMDDevice->GetHMDDeviceType();
		if (HMDType == EHMDDeviceType::DT_OculusRift)
		{
			// is an oculus rift
			// bind inputs

			//basic buttons for the oculus touch

			InputComponent->BindAction("Oculus Left Trigger CapTouch", IE_Pressed, this, &AWRTK_MasterPawn::LTriggerCapTouched);
			InputComponent->BindAction("Oculus Left Trigger CapTouch", IE_Released, this, &AWRTK_MasterPawn::LTriggerCapUntouched);

			InputComponent->BindAction("Oculus Right Trigger CapTouch", IE_Pressed, this, &AWRTK_MasterPawn::RTriggerCapTouched);
			InputComponent->BindAction("Oculus Right Trigger CapTouch", IE_Released, this, &AWRTK_MasterPawn::RTriggerCapUntouched);

			InputComponent->BindAction("Oculus A Button", IE_Pressed, this, &AWRTK_MasterPawn::AButtonPressed);
			InputComponent->BindAction("Oculus A Button", IE_Released, this, &AWRTK_MasterPawn::AButtonReleased);

			InputComponent->BindAction("Oculus B Button", IE_Pressed, this, &AWRTK_MasterPawn::BButtonPressed);
			InputComponent->BindAction("Oculus B Button", IE_Released, this, &AWRTK_MasterPawn::BButtonReleased);

			InputComponent->BindAction("Oculus X Button", IE_Pressed, this, &AWRTK_MasterPawn::XButtonPressed);
			InputComponent->BindAction("Oculus X Button", IE_Released, this, &AWRTK_MasterPawn::XButtonReleased);

			InputComponent->BindAction("Oculus Y Button", IE_Pressed, this, &AWRTK_MasterPawn::YButtonPressed);
			InputComponent->BindAction("Oculus Y Button", IE_Released, this, &AWRTK_MasterPawn::YButtonReleased);

			InputComponent->BindAction("Oculus A Button CapTouch", IE_Pressed, this, &AWRTK_MasterPawn::ACapTouched);
			InputComponent->BindAction("Oculus A Button CapTouch", IE_Released, this, &AWRTK_MasterPawn::ACapUntouched);

			InputComponent->BindAction("Oculus B Button CapTouch", IE_Pressed, this, &AWRTK_MasterPawn::BCapTouched);
			InputComponent->BindAction("Oculus B Button CapTouch", IE_Released, this, &AWRTK_MasterPawn::BCapUntouched);

			InputComponent->BindAction("Oculus X Button CapTouch", IE_Pressed, this, &AWRTK_MasterPawn::XCapTouched);
			InputComponent->BindAction("Oculus X Button CapTouch", IE_Released, this, &AWRTK_MasterPawn::XCapUntouched);

			InputComponent->BindAction("Oculus Y Button CapTouch", IE_Pressed, this, &AWRTK_MasterPawn::YCapTouched);
			InputComponent->BindAction("Oculus Y Button CapTouch", IE_Released, this, &AWRTK_MasterPawn::YCapUntouched);

			InputComponent->BindAction("Oculus Left Thumbstick CapTouch", IE_Pressed, this, &AWRTK_MasterPawn::LThumbCapTouched);
			InputComponent->BindAction("Oculus Left Thumbstick CapTouch", IE_Released, this, &AWRTK_MasterPawn::LThumbCapUntouched);

			InputComponent->BindAction("Oculus Right Thumbstick CapTouch", IE_Pressed, this, &AWRTK_MasterPawn::RThumbCapTouched);
			InputComponent->BindAction("Oculus Right Thumbstick CapTouch", IE_Released, this, &AWRTK_MasterPawn::RThumbCapUntouched);

			InputComponent->BindAction("Oculus Left Grip", IE_Pressed, this, &AWRTK_MasterPawn::LGripPressed);
			InputComponent->BindAction("Oculus Left Grip", IE_Released, this, &AWRTK_MasterPawn::LGripReleased);

			InputComponent->BindAction("Oculus Right Grip", IE_Pressed, this, &AWRTK_MasterPawn::RGripPressed);
			InputComponent->BindAction("Oculus Right Grip", IE_Released, this, &AWRTK_MasterPawn::RGripReleased);

			InputComponent->BindAction("Oculus Left Trigger", IE_Pressed, this, &AWRTK_MasterPawn::LTriggerPressed);
			InputComponent->BindAction("Oculus Left Trigger", IE_Released, this, &AWRTK_MasterPawn::LTriggerReleased);

			InputComponent->BindAction("Oculus Right Trigger", IE_Pressed, this, &AWRTK_MasterPawn::RTriggerPressed);
			InputComponent->BindAction("Oculus Right Trigger", IE_Released, this, &AWRTK_MasterPawn::RTriggerReleased);

			InputComponent->BindAxis("Oculus Left Grip Axis", this, &AWRTK_MasterPawn::LGripAxis);
			InputComponent->BindAxis("Oculus Left Trigger Axis", this, &AWRTK_MasterPawn::LTriggerAxis);

			InputComponent->BindAxis("Oculus Right Grip Axis", this, &AWRTK_MasterPawn::RGripAxis);
			InputComponent->BindAxis("Oculus Right Trigger Axis", this, &AWRTK_MasterPawn::RTriggerAxis);

			if (MovementEnabled)
			{
				InputComponent->BindAxis("Oculus Left Thumbstick Y Axis", this, &AWRTK_MasterPawn::LStickTiltUp);
				InputComponent->BindAxis("Oculus Left Thumbstick X Axis", this, &AWRTK_MasterPawn::LStickTiltRight);
				InputComponent->BindAxis("Oculus Right Thumbstick Y Axis", this, &AWRTK_MasterPawn::RStickTiltUp);
				InputComponent->BindAxis("Oculus Right Thumbstick X Axis", this, &AWRTK_MasterPawn::RStickTiltRight);
			}

		}
		else
		{
			// not an oculus rift
		}
	}
	else
	{
		// Not on any VR. Set up test KB+M Controls
		InputComponent->BindAxis("KB Forward Axis", this, &AWRTK_MasterPawn::LStickTiltUp);
		InputComponent->BindAxis("KB Strafe Axis", this, &AWRTK_MasterPawn::LStickTiltRight);

		// Rotate camera utilizing mouse
		MainCamera->bUsePawnControlRotation = true;
		InputComponent->BindAxis("Mouse X", this, &AWRTK_MasterPawn::AddMouseX);
		InputComponent->BindAxis("Mouse Y", this, &AWRTK_MasterPawn::AddMouseY);

		// Modifier tracking used for using axis input conditionally for hands
		InputComponent->BindAction("KB Left Ctrl", IE_Pressed, this, &AWRTK_MasterPawn::LeftCtrlPressed);
		InputComponent->BindAction("KB Left Ctrl", IE_Released, this, &AWRTK_MasterPawn::LeftCtrlReleased);
		InputComponent->BindAction("KB Left Shift", IE_Pressed, this, &AWRTK_MasterPawn::LeftShiftPressed);
		InputComponent->BindAction("KB Left Shift", IE_Released, this, &AWRTK_MasterPawn::LeftShiftReleased);
		InputComponent->BindAction("KB Left Alt", IE_Pressed, this, &AWRTK_MasterPawn::LeftAltPressed);
		InputComponent->BindAction("KB Left Alt", IE_Released, this, &AWRTK_MasterPawn::LeftAltReleased);
		InputComponent->BindAxis("Mouse Scroll", this, &AWRTK_MasterPawn::ScrollWheel);

		// Add some grip controls using same functions as oculus
		InputComponent->BindAction("Mouse Left", IE_Pressed, this, &AWRTK_MasterPawn::LGripPressed);
		InputComponent->BindAction("Mouse Left", IE_Released, this, &AWRTK_MasterPawn::LGripReleased);

		InputComponent->BindAction("Mouse Right", IE_Pressed, this, &AWRTK_MasterPawn::RGripPressed);
		InputComponent->BindAction("Mouse Right", IE_Released, this, &AWRTK_MasterPawn::RGripReleased);

		InputComponent->BindAction("KB Q", IE_Pressed, this, &AWRTK_MasterPawn::LTriggerPressed);
		InputComponent->BindAction("KB Q", IE_Released, this, &AWRTK_MasterPawn::LTriggerReleased);

		InputComponent->BindAction("KB E", IE_Pressed, this, &AWRTK_MasterPawn::RTriggerPressed);
		InputComponent->BindAction("KB E", IE_Released, this, &AWRTK_MasterPawn::RTriggerReleased);

		InputComponent->BindAction("KB F", IE_Pressed, this, &AWRTK_MasterPawn::FKeyPressed);
	}

}

void AWRTK_MasterPawn::RGripPressed()
{
	if(RightHandActor)
		RightHandActor->InteractManagerComponent->GripPressed();
}

void AWRTK_MasterPawn::RGripReleased()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->GripReleased();
}

void AWRTK_MasterPawn::RGripAxis(float amount)
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->GripAxis(amount);
}

void AWRTK_MasterPawn::RTriggerCapTouched()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->TriggerCapTouched();
}

void AWRTK_MasterPawn::RTriggerCapUntouched()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->TriggerCapUntouched();
}

void AWRTK_MasterPawn::RTriggerPressed()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->TriggerPressed();
}

void AWRTK_MasterPawn::RTriggerReleased()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->TriggerReleased();
}

void AWRTK_MasterPawn::RTriggerAxis(float amount)
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->TriggerAxis(amount);
}

void AWRTK_MasterPawn::RThumbCapTouched()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->ThumbCapTouched();
}

void AWRTK_MasterPawn::RThumbCapUntouched()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->ThumbCapUntouched();
}


void AWRTK_MasterPawn::LGripPressed()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->GripPressed();
}

void AWRTK_MasterPawn::LGripReleased()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->GripReleased();
}

void AWRTK_MasterPawn::LGripAxis(float amount)
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->GripAxis(amount);
}

void AWRTK_MasterPawn::LTriggerCapTouched()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->TriggerCapTouched();
}

void AWRTK_MasterPawn::LTriggerCapUntouched()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->TriggerCapUntouched();
}

void AWRTK_MasterPawn::LTriggerPressed()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->TriggerPressed();
}

void AWRTK_MasterPawn::LTriggerReleased()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->TriggerReleased();
}

void AWRTK_MasterPawn::LTriggerAxis(float amount)
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->TriggerAxis(amount);
}

void AWRTK_MasterPawn::LThumbCapTouched()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->ThumbCapTouched();
}

void AWRTK_MasterPawn::LThumbCapUntouched()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->ThumbCapUntouched();
}

void AWRTK_MasterPawn::ACapTouched()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->CapTouched2();
}

void AWRTK_MasterPawn::ACapUntouched()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->CapUntouched2();
}

void AWRTK_MasterPawn::BCapTouched()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->CapTouched1();
}

void AWRTK_MasterPawn::BCapUntouched()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->CapUntouched1();
}

void AWRTK_MasterPawn::XCapTouched()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->CapTouched1();
}

void AWRTK_MasterPawn::XCapUntouched()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->CapUntouched1();
}

void AWRTK_MasterPawn::YCapTouched()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->CapTouched2();
}

void AWRTK_MasterPawn::YCapUntouched()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->CapUntouched2();
}

void AWRTK_MasterPawn::AButtonPressed()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->Pressed2();
	AttemptTeleport(); // Try to teleport the actor
}

void AWRTK_MasterPawn::AButtonReleased()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->Released2();

}

void AWRTK_MasterPawn::BButtonPressed()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->Pressed1();

}

void AWRTK_MasterPawn::BButtonReleased()
{
	if (LeftHandActor)
	LeftHandActor->InteractManagerComponent->Released1();

}

void AWRTK_MasterPawn::XButtonPressed()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->Pressed2();
}

void AWRTK_MasterPawn::XButtonReleased()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->Released2();
}

void AWRTK_MasterPawn::YButtonPressed()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->Pressed1();
}

void AWRTK_MasterPawn::YButtonReleased()
{
	if (RightHandActor)
	RightHandActor->InteractManagerComponent->Released1();
}


void AWRTK_MasterPawn::LStickTiltUp(float amount)
{
	if (MainCamera && amount != 0)
	{
		FVector forward = MainCamera->GetForwardVector();
		forward.Z = 0; // remove the z component to project onto slopes
		forward.Normalize();

		// Project our input vector into the ground plane based on its normal
		FVector groundNormal = GetGroundNormal();
		FVector inputVector = forward - FVector::DotProduct(groundNormal, forward) * groundNormal;
		inputVector.Normalize();

		GetCharacterMovement()->AddInputVector(inputVector * MoveSpeed * amount);
	}
}

void AWRTK_MasterPawn::LStickTiltRight(float amount)
{	
	if (MainCamera && amount != 0)
	{
		FVector right = MainCamera->GetRightVector();
		right.Z = 0;
		right.Normalize();

		// Project our input vector into the ground plane based on its normal
		FVector groundNormal = GetGroundNormal();
		FVector inputVector = right - FVector::DotProduct(groundNormal, right) * groundNormal;
		inputVector.Normalize();

		GetCharacterMovement()->AddInputVector(inputVector * MoveSpeed * amount);
	}
}

void AWRTK_MasterPawn::RStickTiltUp(float amount)
{
	// Currently unused
}

void AWRTK_MasterPawn::RStickTiltRight(float amount)
{
	float absAmt = FMath::Abs(amount);

	if (MainCamera && absAmt >= 0.8f && LastRStickTiltRightAmount <= 0.8f && MovementEnabled)
	{
		float rotateAmount = amount > 0 ? 30.0f : -30.0f;
		FRotator rotation = FRotator(0, rotateAmount, 0);
		AddActorLocalRotation(rotation);
	}

	LastRStickTiltRightAmount = absAmt;
}

void AWRTK_MasterPawn::AddMouseX(float amount)
{
	if (MainCamera && LeftHandActor && RightHandActor)
	{

		if ((!LeftShiftHeld && !LeftCtrlHeld)) // Move the Camera
			AddControllerYawInput(amount);

		if (LeftShiftHeld) // Controlling Right Hand
		{
			if (LeftAltHeld) // Rotate
				RightHandActor->UpdateKBMYaw(amount);
			else // move
				RightHandActor->UpdateTheta(amount);
		}

		if (LeftCtrlHeld) // Controlling Left Hand
		{
			if (LeftAltHeld) // Rotate
				LeftHandActor->UpdateKBMYaw(amount);
			else // Move
				LeftHandActor->UpdateTheta(amount);
		}
	}
}

void AWRTK_MasterPawn::AddMouseY(float amount)
{
	if (MainCamera && LeftHandActor && RightHandActor)
	{
		if ((!LeftShiftHeld && !LeftCtrlHeld)) // Camera Control
			AddControllerPitchInput(-amount);

		if (LeftShiftHeld) // Controlling Right Hand
		{
			if (LeftAltHeld) // Rotate
				RightHandActor->UpdateKBMPitch(amount);
			else // move
				RightHandActor->UpdatePhi(-amount);
		}

		if (LeftCtrlHeld) // Controlling Left Hand
		{
			if (LeftAltHeld) // Rotate
				LeftHandActor->UpdateKBMPitch(amount);
			else // Move
				LeftHandActor->UpdatePhi(-amount);
		}
	}
}

void AWRTK_MasterPawn::ScrollWheel(float amount)
{
	if (LeftHandActor && RightHandActor && amount != 0.0f)
	{
		if (LeftShiftHeld) // Controlling Right Hand
		{
			if (LeftAltHeld) // Rotate
				RightHandActor->UpdateKBMRoll(amount);
			else // move
				RightHandActor->UpdateRadius(amount);
		}

		if (LeftCtrlHeld) // Controlling Left Hand
		{
			if (LeftAltHeld) // Rotate
				LeftHandActor->UpdateKBMRoll(amount);
			else // Move
				LeftHandActor->UpdateRadius(amount);
		}
	}
}

void AWRTK_MasterPawn::FKeyPressed()
{
	AttemptTeleport();
}

FVector AWRTK_MasterPawn::GetGroundNormal()
{
	// Perform a downward trace in order to project movement into ground plane
	const FName tag("Movement Trace Tag");
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(this); // Don't let the trace hit ourselves.
	traceParams.TraceTag = tag;
	traceParams.bTraceAsyncScene = true;
	traceParams.bReturnPhysicalMaterial = true;

	// Fire the trace downward to hit the ground.
	// Will return first hit. Player should always be above some ground
	FVector startPos = GetCharacterMovement()->GetActorFeetLocation();
	FVector endPos = startPos - 100 * GetActorUpVector();
	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByObjectType(Hit, startPos, endPos,
		FCollisionObjectQueryParams::AllObjects, traceParams);

	FVector groundNormal;

	// If we hit the ground move
	if (Hit.bBlockingHit)
	{
		groundNormal = Hit.ImpactNormal; groundNormal.Normalize();


	}
	else // if we didn't hit ground assume we are crossing narrow ground and should move horizontal
	{
		groundNormal = FVector(0, 0, 1);
	}

	return groundNormal;
}

void AWRTK_MasterPawn::AttemptTeleport()
{
	// First cast a ray forward and see if we hit anything
	float teleportRange = 10000.0f;

	// Positions
	FVector startPos = MainCamera->GetComponentLocation();
	FVector endPos = startPos + teleportRange * MainCamera->GetForwardVector();

	const FName tag("Movement Trace Tag");
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(this); // Don't let the trace hit ourselves.
	traceParams.AddIgnoredActor(pCurrentTeleportLocation);
	traceParams.TraceTag = tag;
	traceParams.bTraceAsyncScene = true;
	traceParams.bReturnPhysicalMaterial = true;
	//GetWorld()->DebugDrawTraceTag = tag;;

	FHitResult Hit(ForceInit);
	bool didHit = GetWorld()->LineTraceSingleByObjectType(Hit, startPos, endPos,
		FCollisionObjectQueryParams::AllObjects, traceParams);

	if (didHit)
	{
		// We hit something. see if it is a teleport marker
		TArray<UTeleportMarker*> TeleportComponent;
		if (!Hit.GetActor()) return;
		Hit.GetActor()->GetComponents(TeleportComponent);

		if (TeleportComponent.Num() > 0)
		{
			for (auto tc : TeleportComponent)
			{
				// Hit immediately, so must have already been there.
				// if location was null it was because we just spawned. try again and return
				if (Hit.Distance < 5)
				{
					pCurrentTeleportLocation = Hit.GetActor();
					AttemptTeleport();
					return;
				}

				// Make sure what we hit was enabled
				if (!tc->GetTPEnabled())
				{
					return;
				}

				// Should only have one teleport component so in here if we valid bois
				this->TeleportTo(tc->GetOwnedPosition() + GetActorUpVector()*90.0f, this->GetActorRotation(), false, true);
				if (tc->RotateOnTeleport)
				{
					FRotator currRot = this->GetActorRotation();
					currRot = MainCamera->GetComponentRotation();
					currRot.Pitch = 0; currRot.Roll = 0;
					FRotator needed = tc->GetOwnedRotation() - currRot;
					AddActorLocalRotation(needed);
				}

				// Update where we hit in order to not collide with it next run
				pCurrentTeleportLocation = Hit.GetActor();
			}
		}
	}
}
