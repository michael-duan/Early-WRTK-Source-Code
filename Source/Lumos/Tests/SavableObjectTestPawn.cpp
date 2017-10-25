// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "LumosGameModeBase.h"
#include "SavableObjectTestPawn.h"


// Sets default values
ASavableObjectTestPawn::ASavableObjectTestPawn(): 
	TestString(TEXT("Default Value")), 
	StringSize(TestString.Len()),
	SaveFile(TEXT("SavedGame"))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create a dummy root component we can attach things to.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// Create a camera and a visible object
	UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	// Attach our camera and visible object to our root component. Offset and rotate the camera.
	OurCamera->SetupAttachment(RootComponent);
	OurCamera->SetRelativeLocation(FVector(-250.0f, 0.0f, 250.0f));
	OurCamera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	OurVisibleComponent->SetupAttachment(RootComponent);
}

// Called every frame
void ASavableObjectTestPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASavableObjectTestPawn::SetupPlayerInputComponent(class UInputComponent* InComp)
{
	Super::SetupPlayerInputComponent(InComp);
	InComp->BindAction("KB U", IE_Pressed, this, &ASavableObjectTestPawn::ChangeString);
	InComp->BindAction("KB O", IE_Pressed, this, &ASavableObjectTestPawn::OutputString);
	InComp->BindAction("KB I", IE_Pressed, this, &ASavableObjectTestPawn::SaveGame);
	InComp->BindAction("KB P", IE_Pressed, this, &ASavableObjectTestPawn::LoadGame);
}

void ASavableObjectTestPawn::SaveObject(TArray<uint8>& data)
{
	data.SetNumZeroed(sizeof(uint32) + StringSize * sizeof(TCHAR), false);
	FMemory::Memcpy(data.GetData(), &StringSize, sizeof(uint32));
	FMemory::Memcpy(data.GetData() + sizeof(uint32), *TestString, StringSize * sizeof(TCHAR));
}

void ASavableObjectTestPawn::LoadObject(TArray<uint8>& data)
{
	FMemory::Memcpy(&StringSize, data.GetData(), sizeof(uint32));
	TCHAR* str = new TCHAR[StringSize];
	FMemory::Memcpy(str, data.GetData() + sizeof(uint32), StringSize * sizeof(TCHAR));
	TestString.Reset();
	TestString.Append(str, StringSize);
	delete [] str;
}

// Called when the game starts or when spawned
void ASavableObjectTestPawn::BeginPlay()
{
	Super::BeginPlay();
	SLManager = ((ALumosGameModeBase*)GetWorld()->GetAuthGameMode())->GetSaveLoadManager();
	SLManager->RegisterSavableObject(FString(TEXT("ASavableObjectTestPawn")), this);
}

void ASavableObjectTestPawn::ChangeString()
{
	TestString = FString(TEXT("Changed Value"));
	StringSize = TestString.Len();
}

void ASavableObjectTestPawn::OutputString()
{
	UE_LOG(LogClass, Log, TEXT("SavableObjectTest: %s"), *TestString);
}

void ASavableObjectTestPawn::SaveGame()
{
	SLManager->SaveAll(SaveFile);
}

void ASavableObjectTestPawn::LoadGame()
{
	SLManager->LoadAll(SaveFile);
}
