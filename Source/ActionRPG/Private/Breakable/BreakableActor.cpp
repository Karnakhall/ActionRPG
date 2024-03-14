// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));	// Create the GeometryCollectionComponent
	SetRootComponent(GeometryCollection);	// Set the GeometryCollectionComponent as the root component
	GeometryCollection->SetGenerateOverlapEvents(true);	// Enable overlap events
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);	// Ignore camera
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);	// Ignore pawn

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));	// Create the CapsuleComponent
	Capsule->SetupAttachment(GetRootComponent());	// Attach the CapsuleComponent to the root component
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);	// Ignore all channels
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);	// Block pawn
}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

	// example how doing this in c++ = GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::GetHit);	// Bind the GetHit function to the OnChaosBreakEvent
	
}

// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)	// Implement the GetHit function from the HitInterface
{
	if (bBroken)	// If the object is already broken
	{
		return;	// Return
	}
	bBroken = true;	// Set the broken flag to true

	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)	//
	{
		FVector Location = GetActorLocation();	// Get the location of the actor
		Location.Z += 75.0f;	// Increase the Z value of the location

		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);	// Select a random treasure class. FMath::RandRange returns a random integer between the two values passed as arguments
		World->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, GetActorRotation());	// Spawn the treasure at the location
	}
}

