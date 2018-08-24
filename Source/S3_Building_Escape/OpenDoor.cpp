// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "Gameframework/Actor.h"
#include "Runtime/Engine/Classes/Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	owner = GetOwner();

	if (!pressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing pressure plate"), *owner->GetName());
	}
}

void UOpenDoor::OpenDoor()
{
	// Set the door rotation
	if (!owner) { return; }
	owner->SetActorRotation(FRotator(0.f, openAngle, 0.f));
}

void UOpenDoor::CloseDoor()
{
	// Create rotator
	FRotator newRotation = FRotator(0.f, 0.f, 0.f);

	// Set the door rotation
	if (!owner) { return; }
	owner->SetActorRotation(newRotation);
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll the Trigger Volume
	if (GetTotalMassOfActors() > 30.0f)	// TODO make into a parameter
	{
		OpenDoor();
		lastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}

	// Check if it's time to close the door
	if (GetWorld()->GetTimeSeconds() - lastDoorOpenTime > doorCloseDelay)
	{
		CloseDoor();
	}
}

float UOpenDoor::GetTotalMassOfActors()
{
	float totalMass = 0;
	
	// Find all the overlapping actors
	TArray<AActor*> overlappingActors;
	if (!pressurePlate) { return totalMass; }
	pressurePlate->GetOverlappingActors(OUT overlappingActors);

	// Iterate through them adding their masses
	for (const auto* actor : overlappingActors)
	{
		totalMass += actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s on Pressure Plate"), *actor->GetName())
	}

	return totalMass;
}