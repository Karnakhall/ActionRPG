// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"	// Potrzebujemy tego nag��wka aby nasz "AEnemy" m�g� dziedziczy� z funkcji HitInterface
#include "Characters/CharacterTypes.h"
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
	
	/** Combat */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;	//Implementujemy funkcj� GetHit z interfejsu HitInterface

	//Function to attack
	virtual void Attack();

	UFUNCTION(BlueprintNativeEvent)
	//Funkcja do �mierci
	void Die();


	//Funkcja do reakcji na cios w zale�no�ci od kierunku
	void DirectionalHitReact(const FVector& ImpactPoint);
	//Funkcja do odtwarzania d�wi�ku otrzymania ciosu
	void PlayHitSound(const FVector& ImpactPoint);	
	//Funkcja do spawnowania particle systemu otrzymania ciosu
	void SpawnHitParticles(const FVector& ImpactPoint);	
	//Funkcja do obs�ugi obra�e�
	virtual void HandleDamage(float DamageAmount);	
	//Funkcja do wy��czenia kapsu�y kolizji
	void DisableCapsule();	
	//Sprawdzamy boolem czy postac mo�e zaatakowa�
	virtual bool CanAttack();
	//Sprawdzamy czy posta� �yje
	bool IsAlive();	
	//Funkcja do wy��czenia kolizji na meshe aby nie mo�na by�a zaatakowa� martwego przeciwnika
	void DisableMeshCollision();	

	UFUNCTION(BlueprintCallable)	//Dzi�ki temu mo�emy wywo�a� t� funkcj� z blueprintu
	//Function to stop attack montage
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)	//Dzi�ki temu mo�emy wywo�a� t� funkcj� z blueprintu
	//Function to stop dodge montage
	virtual void DodgeEnd();

	UFUNCTION(BlueprintCallable)	//Dzi�ki temu mo�emy wywo�a� t� funkcj� z blueprintu
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);	//Funkcja do w��czenia i wy��czenia kolizji na broni

	/**
	* Play Montage functions
	*/
	
	//Funkcja do odtwarzania animacji otrzymania ciosu
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	void StopAttackMontage();
	
	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();	//Funkcja do zwracania wektora przesuni�cia

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();	//Funkcja do zwracania wektora rotacji
	//Variable to keep track of the attack combo
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;	//Wska�nik do broni

	/*
	* Components
	*/

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;	// Wska�nik do komponentu atrybut�w

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;	//Wska�nik do celu walki

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;


	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;	//Zmienna do �ledzenia pozycji �mierci

private:
//Variable to keep track of the attack combo
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);	//Funkcja do wywo�ania losowej sekcji animacji i odtwarzania
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);	//Funkcja do wywo�ania losowej sekcji animacji i odtwarzania

	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;	//D�wi�k otrzymania ciosu. Przechowujemy ten asset we wska�niku

	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;	//Particle system otrzymania ciosu. Przechowujemy ten asset we wska�niku

	//Pokazujemy to w edytorze blueprint pod kategori� "Montages"
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	//Pokazujemy to w edytorze blueprint pod kategori� "Montages"
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* HitReactMontage;	//Animacja otrzymania ciosu

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;	//Animacja �mierci

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DodgeMontage;	//Animacja uniku

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;	//Tablica nazw animacji ataku


	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> DeathMontageSections;	//Tablica nazw animacji �mierci

	public:
		FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }	//Funkcja do zwracania pozycji �mierci
};
