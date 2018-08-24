// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Gameframework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Public/CollisionQueryParams.h"
#include "Components/PrimitiveComponent.h"

#define OUT		// does nothing, just a label for out parameters

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!physicsHandle) { return; }
	// if physics handle is attached
	if (physicsHandle->GrabbedComponent)
	{
		// move the object we're holding
		physicsHandle->SetTargetLocation(GetReachLineEnd());
	}
}

/// Look for attached Physics Handle
void UGrabber::FindPhysicsHandleComponent()
{
	physicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (physicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing PhysicsHandle component"), *GetOwner()->GetName())
	}
}

/// Look for attached Input Component (only appears at runtime)
void UGrabber::SetupInputComponent()
{
	inputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (inputComponent)
	{
		inputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		inputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *GetOwner()->GetName());
	}
}

void UGrabber::Grab()
{
	/// Line trace and see if we reach any actors with physics body collision channel set
	auto hitResult = GetFirstPhysicsBodyInReach();
	auto componentToGrab = hitResult.GetComponent();	// gets the mesh in our case
	auto actorHit = hitResult.GetActor();

	/// If we hit something then attach a physics handle
	if (actorHit)
	{
		if (!physicsHandle) { return; }
		physicsHandle->GrabComponentAtLocationWithRotation(componentToGrab, NAME_None,
			componentToGrab->GetOwner()->GetActorLocation(),
			componentToGrab->GetOwner()->GetActorRotation()
		);
	}
}

void UGrabber::Release()
{
	if (!physicsHandle) { return; }
	physicsHandle->ReleaseComponent();
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	/// Line-trace (raycast) out to reach distance
	FHitResult hitResult;
	FCollisionQueryParams traceParameters(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(OUT hitResult, GetReachLineStart(),
		GetReachLineEnd(), FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		traceParameters
	);

	return hitResult;
}

FVector UGrabber::GetReachLineStart()
{
	/// Get player viewpoint this tick
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT playerViewPointLocation,
		OUT playerViewPointRotation
	);

	return playerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd()
{
	/// Get player viewpoint this tick
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT playerViewPointLocation,
		OUT playerViewPointRotation
	);

	return playerViewPointLocation + playerViewPointRotation.Vector() * reach;
}
