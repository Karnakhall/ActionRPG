// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"	// Potrzebujemy tego nag��wka aby Enemy m�g� dziedziczy� funkcje z BaseCharacter
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

	/** <AActor> */
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;	//Funkcja do otrzymywania obra�e�
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface> */
	// Called to bind functionality to input
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;	// Implementujemy funkcj� GetHit z interfejsu HitInterface
	/** </IHitInterface> */

protected:

	// Called when the game starts or when spawned
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;	//Funkcja do �mierci
	void SpawnSoul();	//Funkcja do spawnowania duszy po �mierci przeciwnika
	virtual void Attack() override;	//Funkcja do ataku
	virtual bool CanAttack() override;	//Funkcja do sprawdzania czy przeciwnik mo�e zaatakowa�
	virtual void AttackEnd() override;	//Funkcja do zako�czenia ataku
	virtual void HandleDamage(float DamageAmount) override;	//Funkcja do obs�ugi obra�e�
	//virtual int32 PlayDeathMontage() override;	//Funkcja do odtwarzania animacji �mierci
	/** </ABaseCharacter> */

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;	//Stan przeciwnika



private:

	/** AI Behavior */
	void InitializeEnemy();		//Funkcja do inicjalizacji przeciwnika. Ustawa stan pocz�tkowy, ustawia kontroler AI, ustawia cel patrolu, ustawia pasek �ycia
	void CheckPatrolTarget();	//Funkcja do sprawdzania celu patrolu
	void CheckCombatTarget();	//Funkcja do sprawdzania celu walki
	void PatrolTimerFinished();	//Funkcja do zako�czenia patrolu
	void HideHealthBar();	//Funkcja do ukrycia paska �ycia
	void ShowHealthBar();	//Funkcja do pokazania paska �ycia
	void LoseInterest();	//Funkcja do utraty zainteresowania
	void StartPatrolling();	//Funkcja do rozpocz�cia patrolowania
	void ChaseTarget();	//Funkcja do goniennia celu
	bool IsOutsideCombatRadius();	//Sprawdzenie czy przeciwnik jest poza zasi�giem walki
	bool IsOutsideAttackRadius();	//Sprawdzenie czy przeciwnik jest poza zasi�giem ataku
	bool IsInsideAttackRadius();	//Sprawdzenie czy przeciwnik jest w zasi�gu ataku
	bool IsChasing();	//Sprawdzenie czy przeciwnik goni
	bool IsAttacking();	//Sprawdzenie czy przeciwnik atakuje
	bool IsDead();	//Sprawdzenie czy przeciwnik jest martwy
	bool IsEngaged();	//Sprawdzenie czy przeciwnik jest zaanga�owany w walk�
	void ClearPatrolTimer();	//Funkcja do wyczyszczenia timera patrolu
	
	/** Combat */
	void StartAttackTimer();	//Funkcja do rozpocz�cia timera ataku
	void ClearAttackTimer();	//Funkcja do wyczyszczenia timera ataku
	bool InTargetRange(AActor* Target, double Radius);	//Funkcja do sprawdzania czy przeciwnik jest w zasi�gu
	void MoveToTarget(AActor* Target);	//Funkcja do poruszania si� do oznaczonych cel�w
	AActor* ChoosePatrolTarget();	//Funkcja do wyboru celu patrolu
	void SpawnDefaultWeapon();	//Funkcja do spawnowania broni
	UFUNCTION()
	void PawnSeen(APawn* Pawn);	//Funkcja widzenia dla pionka. Callback for OnSeePawn in UPawnSensingComponent

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;	// Wska�nik do komponentu widgetu paska �ycia

	UPawnSensingComponent* PawnSensing;	// Wska�nik do komponentu PawnSensing

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AWeapon> WeaponClass;	//pos�u�y do spawnowania broni


	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 500.f;	//Promie� walki w kt�rym b�dziemy widzieli pasek przeciwnika

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AttackRadius = 150.f;	//Promie� walki w kt�rym b�dziemy mogli zaatakowac przeciwnika i na odwr�

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AcceptanceRadius = 50.f;	//Promie� walki w kt�rym b�dziemy mogli zaatakowac przeciwnika i na odwr�

	UPROPERTY()
	class AAIController* EnemyController;	//Wska�nik do kontrolera AI
	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")//, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* PatrolTarget;	//Wska�nik do celu patrolu

	// Array of patrol points
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;	//Tablica cel�w patrolu

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;	//Promie� walki w kt�rym b�dziemy widzieli pasek przeciwnika

	FTimerHandle PatrolTimer;	//Timer do patrolu

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;	//Minimalny czas oczekiwania

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;	//Maksymalny czas oczekiwania

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 125.f;	//Pr�dko�� patrolowania

	FTimerHandle AttackTimer;	//Timer do ataku

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;	//Pr�dko�� goniennia

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 5.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ASoul> SoulClass;	//Klasa duszy
};
