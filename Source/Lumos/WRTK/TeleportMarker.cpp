// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "TeleportMarker.h"

FVector UTeleportMarker::GetOwnedPosition()
{
	return OwnedPostion->GetActorLocation();
}

FRotator UTeleportMarker::GetOwnedRotation()
{
	return OwnedPostion->GetActorRotation();
}

void UTeleportMarker::BeginPlay()
{
	Super::BeginPlay();
}