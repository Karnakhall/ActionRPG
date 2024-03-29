// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"	// Potrzebujemy tego nag³ówka aby Enemy móg³ dziedziczyæ funkcje z BaseCharacter
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;	//Zadeklarowany w HealthBarComponent.h
class UPawnSensingComponent;	//Zadeklarowany w PawnSensingComponent.h

UCLASS()
class ACTIONRPG_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CheckPatrolTarget();

	void CheckCombatTarget();

	// Called to bind functionality to input
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;	// Implementujemy funkcjê GetHit z interfejsu HitInterface
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;	//Funkcja do otrzymywania obra¿eñ
	virtual void Destroyed() override;
private:

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;	// WskaŸnik do komponentu widgetu paska ¿ycia

	UPawnSensingComponent* PawnSensing;	// WskaŸnik do komponentu PawnSensing

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;	//pos³u¿y do spawnowania broni

	UPROPERTY()
	AActor* CombatTarget;	//WskaŸnik do celu walki

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;	//Promieñ walki w którym bêdziemy widzieli pasek przeciwnika

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;	//Promieñ walki w którym bêdziemy mogli zaatakowac przeciwnika i na odwró

	/*
	* Navigation
	*/
	UPROPERTY()
	class AAIController* EnemyController;	//WskaŸnik do kontrolera AI
	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")//, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* PatrolTarget;	//WskaŸnik do celu patrolu

	// Array of patrol points
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;	//Tablica celów patrolu

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;	//Promieñ walki w którym bêdziemy widzieli pasek przeciwnika

	FTimerHandle PatrolTimer;	//Timer do patrolu
	void PatrolTimerFinished();	//Funkcja do zakoñczenia patrolu

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;	//Minimalny czas oczekiwania
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;	//Maksymalny czas oczekiwania
	
	/** AI behavior */
	void HideHealthBar();	//Funkcja do ukrycia paska ¿ycia
	void ShowHealthBar();	//Funkcja do pokazania paska ¿ycia
	void LoseInterest();	//Funkcja do utraty zainteresowania
	void StartPatrolling();	//Funkcja do rozpoczêcia patrolowania
	void ChaseTarget();	//Funkcja do goniennia celu
	bool IsOutsideCombatRadius();	//Sprawdzenie czy przeciwnik jest poza zasiêgiem walki
	bool IsOutsideAttackRadius();	//Sprawdzenie czy przeciwnik jest poza zasiêgiem ataku
	bool IsInsideAttackRadius();	//Sprawdzenie czy przeciwnik jest w zasiêgu ataku
	bool IsChasing();	//Sprawdzenie czy przeciwnik goni
	bool IsAttacking();	//Sprawdzenie czy przeciwnik atakuje
	bool IsDead();	//Sprawdzenie czy przeciwnik jest martwy
	bool IsEngaged();	//Sprawdzenie czy przeciwnik jest zaanga¿owany w walkê
	void ClearPatrolTimer();	//Funkcja do wyczyszczenia timera patrolu

	/** Combat */
	void StartAttackTimer();	//Funkcja do rozpoczêcia timera ataku
	void ClearAttackTimer();	//Funkcja do wyczyszczenia timera ataku


	FTimerHandle AttackTimer;	//Timer do ataku

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 125.f;	//Prêdkoœæ patrolowania

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;	//Prêdkoœæ goniennia

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Die() override;	//Funkcja do œmierci
	bool InTargetRange(AActor* Target, double Radius);	//Funkcja do sprawdzania czy przeciwnik jest w zasiêgu
	void MoveToTarget(AActor* Target);	//Funkcja do poruszania siê do oznaczonych celów
	AActor* ChoosePatrolTarget();	//Funkcja do wyboru celu patrolu
	virtual void Attack() override;	//Funkcja do ataku
	virtual bool CanAttack() override;	//Funkcja do sprawdzania czy przeciwnik mo¿e zaatakowaæ
	virtual void HandleDamage(float DamageAmount) override;	//Funkcja do obs³ugi obra¿eñ
	virtual int32 PlayDeathMontage() override;	//Funkcja do odtwarzania animacji œmierci
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 8.f;

	UFUNCTION()
	void PawnSeen(APawn* Pawn);	//Funkcja do widzenia pionka. Callback


	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;	//Zmienna do œledzenia pozycji œmierci

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;	//Stan przeciwnika

public:	
	
};
