// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"


class UGeometryCollectionComponent;	// Forward declaration
UCLASS()
class ACTIONRPG_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakableActor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;	// Implementujemy funkcjê GetHit z interfejsu HitInterface
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;	// Kapsu³a, która bêdzie wykorzystywana do wykrywania kolizji z bohaterem

private:
	

	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TSubclassOf<class ATreasure> TreasureClass;	// Klasa przedmiotu, który ma byæ spawnowany po zniszczeniu

};
