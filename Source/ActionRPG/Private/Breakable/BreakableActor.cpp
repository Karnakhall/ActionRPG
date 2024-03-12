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
	Capsule->SetupAttachment(GeometryCollection);	// Attach the CapsuleComponent to the GeometryCollectionComponent
	Capsule->SetGenerateOverlapEvents(true);	// Enable overlap events
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);	// Ignore camera
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);	// Overlap pawn
}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)	// Implement the GetHit function from the HitInterface
{
	UWorld* World = GetWorld();
	if (World && TreasureClass)	// Check if the world and the treasure class are not null
	{
		FVector Location = GetActorLocation();	// Get the location of the actor
		Location.Z += 75.0f;	// Increase the Z value of the location
		World->SpawnActor<ATreasure>(TreasureClass, Location, GetActorRotation());	// Spawn the treasure at the location
	}
}

