// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"
#include "ActionRPG/ActionRPG.h"

#define THIRTY 30




// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	/*UE_LOG(LogTemp, Warning, TEXT("Siema"));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Green, FString("Siema!"));
	}*/

	UWorld* World = GetWorld();
	
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();

	
	DRAW_SPHERE(Location)
	DRAW_LINE(Location, Location + Forward * 100.f)

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	
	/*UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), DeltaTime);

	if (GEngine)
	{
		FString Name = GetName();
		FString Messege = FString::Printf(TEXT("Item Name: %s"), *Name);
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Green, Messege);

		UE_LOG(LogTemp, Warning, TEXT("Item Name: %s"), *Name);
	}*/
}

