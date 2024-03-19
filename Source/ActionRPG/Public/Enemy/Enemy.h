// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"	// Potrzebujemy tego nag³ówka aby nasz "AEnemy" móg³ dziedziczyæ z funkcji HitInterface 
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;	// Zadeklarowany w SlashCharacter.h
class UAttributeComponent;	//Zadeklarowany w AttributeComponent.h
class UHealthBarComponent;	//Zadeklarowany w HealthBarComponent.h

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
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;	// Implementujemy funkcjê GetHit z interfejsu HitInterface
	void DirectionalHitReact(const FVector& ImpactPoint);	//Funkcja do reakcji na cios w zale¿noœci od kierunku

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;	//Funkcja do otrzymywania obra¿eñ

private:

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;	// WskaŸnik do komponentu atrybutów

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;	// WskaŸnik do komponentu widgetu paska ¿ycia
	/**
	*Animation montages
	*/
	//Pokazujemy to w edytorze blueprint pod kategori¹ "Montages"
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;	//Animacja otrzymania ciosu

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;	//Animacja œmierci

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* HitSound;	//DŸwiêk otrzymania ciosu. Przechowujemy ten asset we wskaŸniku

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticles;	//Particle system otrzymania ciosu. Przechowujemy ten asset we wskaŸniku

	UPROPERTY()
	AActor* CombatTarget;	//WskaŸnik do celu walki

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;	//Promieñ walki w którym bêdziemy widzieli pasek przeciwnika

	/*
	* Navigation
	*/
	UPROPERTY()
	class AAIController* EnemyController;	//WskaŸnik do kontrolera AI
	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;	//WskaŸnik do celu patrolu

	// Array of patrol points
	TArray<AActor*> PatrolTargets;	//Tablica celów patrolu
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Die();	//Funkcja do œmierci
	/**
	* Play Montage functions
	*/
	void PlayHitReactMontage(const FName& SectionName);	//Funkcja do odtwarzania animacji otrzymania ciosu

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;	//Zmienna do œledzenia pozycji œmierci

public:	
	
};
