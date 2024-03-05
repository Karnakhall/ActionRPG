// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"	// Potrzebujemy tego nag³ówka aby nasz "AEnemy" móg³ dziedziczyæ z funkcji HitInterface 
#include "Enemy.generated.h"

class UAnimMontage;	// Zadeklarowany w SlashCharacter.h

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

private:
	/**
	*Animation montages
	*/
	//Pokazujemy to w edytorze blueprint pod kategori¹ "Montages"
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;	//Animacja otrzymania ciosu


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/**
	* Play Montage functions
	*/
	void PlayHitReactMontage(const FName& SectionName);	//Funkcja do odtwarzania animacji otrzymania ciosu

public:	
	
};
