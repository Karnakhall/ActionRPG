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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;	//Implementujemy funkcjê GetHit z interfejsu HitInterface

	//Function to attack
	virtual void Attack();
	//Funkcja do œmierci
	virtual void Die();
	//Funkcja do reakcji na cios w zale¿noœci od kierunku
	void DirectionalHitReact(const FVector& ImpactPoint);
	//Funkcja do odtwarzania dŸwiêku otrzymania ciosu
	void PlayHitSound(const FVector& ImpactPoint);	
	//Funkcja do spawnowania particle systemu otrzymania ciosu
	void SpawnHitParticles(const FVector& ImpactPoint);	
	//Funkcja do obs³ugi obra¿eñ
	virtual void HandleDamage(float DamageAmount);	
	//Funkcja do wy³¹czenia kapsu³y kolizji
	void DisableCapsule();	
	//Sprawdzamy boolem czy postac mo¿e zaatakowaæ
	virtual bool CanAttack();
	//Sprawdzamy czy postaæ ¿yje
	bool IsAlive();	


	UFUNCTION(BlueprintCallable)	//Dziêki temu mo¿emy wywo³aæ t¹ funkcjê z blueprintu
	//Function to stop attack montage
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)	//Dziêki temu mo¿emy wywo³aæ t¹ funkcjê z blueprintu
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);	//Funkcja do w³¹czenia i wy³¹czenia kolizji na broni

	/**
	* Play Montage functions
	*/
	
	//Funkcja do odtwarzania animacji otrzymania ciosu
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
		
	

	//Variable to keep track of the attack combo
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;	//WskaŸnik do broni

	/*
	* Components
	*/

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;	// WskaŸnik do komponentu atrybutów

private:
//Variable to keep track of the attack combo
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);	//Funkcja do wywo³ania losowej sekcji animacji i odtwarzania
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);	//Funkcja do wywo³ania losowej sekcji animacji i odtwarzania

	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;	//DŸwiêk otrzymania ciosu. Przechowujemy ten asset we wskaŸniku

	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;	//Particle system otrzymania ciosu. Przechowujemy ten asset we wskaŸniku

	//Pokazujemy to w edytorze blueprint pod kategori¹ "Montages"
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	//Pokazujemy to w edytorze blueprint pod kategori¹ "Montages"
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* HitReactMontage;	//Animacja otrzymania ciosu

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;	//Animacja œmierci

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;	//Tablica nazw animacji ataku


	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> DeathMontageSections;	//Tablica nazw animacji œmierci
};
