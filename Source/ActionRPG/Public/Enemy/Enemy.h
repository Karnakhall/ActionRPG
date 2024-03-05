// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"	// Potrzebujemy tego nag³ówka aby nasz "AEnemy" móg³ dziedziczyæ z funkcji HitInterface 
#include "Enemy.generated.h"

UCLASS()
class ACTIONRPG_API AEnemy : public ACharacter, public IHitInterface	// Dziedziczymy z IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit(const FVector& ImpactPoint) override;	// Implementujemy funkcjê GetHit z interfejsu HitInterface
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
};
