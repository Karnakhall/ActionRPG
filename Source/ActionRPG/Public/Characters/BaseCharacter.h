// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"	// Potrzebujemy tego nag³ówka aby nasz "AEnemy" móg³ dziedziczyæ z funkcji HitInterface 
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;	//Zadeklarowany w AttributeComponent.h
class UAnimMontage;	// Zadeklarowany w SlashCharacter.h

UCLASS()
class ACTIONRPG_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)	//Dziêki temu mo¿emy wywo³aæ t¹ funkcjê z blueprintu
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);	//Funkcja do w³¹czenia i wy³¹czenia kolizji na broni
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Function to attack
	virtual void Attack();
	//Funkcja do œmierci
	virtual void Die();

	/**
	* Play Montage functions
	*/
	
	//Function to play attack montage
	virtual void PlayAttackMontage();
	//Funkcja do odtwarzania animacji otrzymania ciosu
	void PlayHitReactMontage(const FName& SectionName);	
	//Funkcja do reakcji na cios w zale¿noœci od kierunku
	void DirectionalHitReact(const FVector& ImpactPoint);	

	//Sprawdzamy boolem czy postac mo¿e zaatakowaæ
	virtual bool CanAttack();

	UFUNCTION(BlueprintCallable)	//Dziêki temu mo¿emy wywo³aæ t¹ funkcjê z blueprintu
	//Function to stop attack montage
	virtual void AttackEnd();

	//Variable to keep track of the attack combo
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;	//WskaŸnik do broni

	/**
	*Animation montages
	*/
	//Pokazujemy to w edytorze blueprint pod kategori¹ "Montages"
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	//Pokazujemy to w edytorze blueprint pod kategori¹ "Montages"
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;	//Animacja otrzymania ciosu

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;	//Animacja œmierci
	/*
	* Components
	*/

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;	// WskaŸnik do komponentu atrybutów

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* HitSound;	//DŸwiêk otrzymania ciosu. Przechowujemy ten asset we wskaŸniku

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticles;	//Particle system otrzymania ciosu. Przechowujemy ten asset we wskaŸniku
};
