// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h" // for USkeletalMeshComponent
#include "GameFramework/CharacterMovementComponent.h" // for UCharacterMovementComponent
#include "Perception/PawnSensingComponent.h"	// for UPawnSensingComponent
#include "Components/AttributeComponent.h"	// Potrzebujemy tego nag��wka aby nasz "AEnemy" m�g� dziedziczy� z funkcji AttributeComponent
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"	// Potrzebujemy tego nag��wka aby nasz "AEnemy" m�g� dziedziczy� z funkcji Weapon
#include "Kismet/KismetSystemLibrary.h"
#include "Items/Soul.h"	// Potrzebujemy tego nag��wka aby nasz "AEnemy" m�g� dziedziczy� z funkcji Soul

#include "Navigation/PathFollowingComponent.h"
#include "ActionRPG/DebugMacros.h"	// Potrzebujemy tego nag��wka aby m�c u�ywa� makr debuguj�cych



// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);	// Set the collision object type to WorldDynamic
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);	// Set the collision response to Visibility)
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);	// Set the collision response to Camera
	GetMesh()->SetGenerateOverlapEvents(true);	// Set the mesh to generate overlap events
	
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));	// Tworzymy domy�lny subobiekt "HealthBarWidget" z klasy UHealthBarComponent
	HealthBarWidget->SetupAttachment(GetRootComponent());	// Ustawiamy HealthBarWidget jako podobiekt do naszego "enemy"

	GetCharacterMovement()->bOrientRotationToMovement = true;	// Obracamy naszego "enemy" w kierunku ruchu
	
	bUseControllerRotationPitch = false;	// Nie u�ywamy kontrolera do obracania si� w osi X
	bUseControllerRotationYaw = false;	// Nie u�ywamy kontrolera do obracania si� w osi Y
	bUseControllerRotationRoll = false;	// Nie u�ywamy kontrolera do obracania si� w osi Z

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));	// Tworzymy domy�lny subobiekt "PawnSensing" z klasy UPawnSensingComponent
	PawnSensing->SetPeripheralVisionAngle(45.f);	// Ustawiamy k�t widzenia na 45 stopni
	PawnSensing->SightRadius = 2000.f;	// Ustawiamy promie� widzenia na 1200
	PawnSensing->HearingThreshold = 600.f;	// Ustawiamy pr�g s�yszenia na 600

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead()) return;	// Wywo�ujemy funkcj� IsDed kt�ra sprawdza czy EnemyState jest r�wny EES_Dead, je�li tak to zwracamy
	if (EnemyState > EEnemyState::EES_Patrolling)	//Je�li EnemyState jest wi�kszy od EES_Patrolling to:
	{
		CheckCombatTarget();	// Wywo�ujemy funkcj� CheckCombatTarget
	}
	else
	{
		CheckPatrolTarget();	// Wywo�ujemy funkcj� CheckPatrolTarget
	}

	/*if (PatrolTarget && EnemyController) ca�y kod poni�ej mam powy�szym if statementem
	{
		if (InTargetRange(PatrolTarget, PatrolRadius))	//Je�li odleg�o�� mi�dzy nami a naszym "enemy"(PatrolTarget) jest mniejsza ni� PatrolRadius to:
		{
			TArray<AActor*> ValidTargets;	// Tworzymy tablic� ValidTargets
			for (AActor* Target : PatrolTargets)	// P�tla for, kt�ra sprawdza czy Target jest r�ny od PatrolTarget
			{
				if (Target != PatrolTarget)	// Je�li Target jest r�ny od PatrolTarget, to dodajemy go do tablicy ValidTargets
				{
					ValidTargets.AddUnique(Target);		// Dodajemy Target do tablicy ValidTargets
				}
			}


			const int32 NumPatrolTargets = PatrolTargets.Num() - 1;	//Pobieramy ilo�� PatrolTargets
			if (NumPatrolTargets > 0)
			{
				const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets);	//Losujemy numer z tablicy PatrolTargets
				AActor* Target = PatrolTargets[TargetSelection];	//Przypisujemy Target warto�� PatrolTargets[TargetSelection]
				PatrolTarget = Target;	//Przypisujemy PatrolTarget warto�� Target

				FAIMoveRequest MoveRequest;	// Tworzymy struktur� FAIMoveRequest
				MoveRequest.SetGoalActor(PatrolTarget);	// Ustawiamy cel ruchu na PatrolTarget
				MoveRequest.SetAcceptanceRadius(15.f);	// Ustawiamy promie� akceptacji na 15
				EnemyController->MoveTo(MoveRequest);	// Wywo�ujemy funkcj� MoveTo z kontrolera przeciwnika
			}

		}
	}
	*/
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);	// Wywo�ujemy funkcj� HandleDamage z argumentem DamageAmount
	CombatTarget = EventInstigator->GetPawn();	// Przypisujemy CombatTarget warto�� EventInstigator->GetPawn()
	
	if (IsInsideAttackRadius())	//Je�li posta� jest w zasi�gu ataku to ustawiamy "enemy" state na attak
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}
	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();	// Usuwamy bro� jesli b�dziemy chcieli aby przeciwnik upuszcza� bro�, mo�emy to zrobic w�a�nie w tym miejscu
	}
	Super::Destroyed();
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)	// Deklarujemy funkcj� GetHit z Enemy.h
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);	// Wywo�ujemy funkcj� GetHit_Implementation z BaseCharacter
	if (!IsDead()) ShowHealthBar();	//Wywo�ujemy funkcj� ShowHealthBar
	ClearPatrolTimer();	//Wywo�ujemy funkcj� ClearPatrolTimer
	ClearAttackTimer();	//Wywo�ujemy funkcj� ClearAttackTimer
	
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	
	StopAttackMontage();	//Wywo�ujemy funkcj� StopAttackMontage
	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();	//Funkcja do rozpocz�cia timera ataku
	}
	//DRAW_SPHRE_COLOR(ImpactPoint, FColor::Orange);	// Rysujemy kulk� w kolorze pomara�czowym gdy uderzymy mieczem w "enemy"
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);	// Dodajemy dynamicznie funkcj� PawnSeen do delegata OnSeePawn
	}
	InitializeEnemy();	// Wywo�ujemy funkcj� InitializeEnemy

	Tags.Add(FName("Enemy"));
}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();	// Wywo�ujemy funkcj� Die z BaseCharacter
	EnemyState = EEnemyState::EES_Dead;	// Ustawiamy EnemyState na EES_Dead
	
	ClearAttackTimer();	// Czy�cimy timer ataku
	//Je�li przeciwnik ma HealthBarWidget, to ustawiamy jego widoczno�� na false w momencie �mierci
	HideHealthBar();

	DisableCapsule();	// Wywo�ujemy funkcj� DisableCapsule, kt�ra wy���za kolizj� kapsu�y po smierci
	SetLifeSpan(DeathLifeSpan);	// Ustawiamy czas po kt�rym, cia�o przeciwnika znika po 3 sekundach od jego �mierci
	GetCharacterMovement()->bOrientRotationToMovement = false;	// Wy��czamy obracanie si� w kierunku ruchu
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);		// Wy��czamy kolizj� broni gdy przeicwnik umiera
	SpawnSoul();
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();	// Pobieramy �wiat
	if (World && SoulClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);	// Pobieramy lokalizacj� przeciwnika
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());	// Spawnujemy dusz� w miejscu �mierci przeciwnika
		if (SpawnedSoul)
		{
			SpawnedSoul->SetSouls(Attributes->GetSouls());	// Ustawiamy ilo�� dusz do zebrania
			SpawnedSoul->SetOwner(this);	// Ustawiamy w�a�ciciela
		}
	}
}

void AEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr) return;	// Sprawdzamy czy CombatTarget nie jest nullpointerem

	EnemyState = EEnemyState::EES_Engaged;	//Ustawiamy EnemyState na EES_Engaged
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();
	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;	//Ustawiamy EnemyState na EES_NoState
	CheckCombatTarget();	//Wywo�ujemy funkcj� CheckCombatTarget
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

/*int32 AEnemy::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
}*/


//Funkcja do inicjalizacji przeciwnika. Ustawa stan pocz�tkowy, ustawia kontroler AI, ustawia cel patrolu, ustawia pasek �ycia
void AEnemy::InitializeEnemy()
{
	//	Przypisujemy wska�nik do kontrolera przeciwnika
	EnemyController = Cast<AAIController>(GetController());

	// Wywo�ujemy funkcj� MoveToTarget z argumentem PatrolTarget
	MoveToTarget(PatrolTarget);

	//Wywo�ujemy funkcj� HideHealthBar kt�ra ukrywa pasek �ycia
	HideHealthBar();

	// Wywo�ujemy funkcj� SpawnDefaultWeapon kt�ra tworzy spawnuje bro�
	SpawnDefaultWeapon();
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();	// Wywo�ujemy funkcj� ChoosePatrolTarget
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);	// Losujemy czas oczekiwania
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);	// Ustawiamy timer na 5 sekund
		//MoveToTarget(PatrolTarget);	// Wywo�ujemy funkcj� MoveToTarget z argumentem PatrolTarget

	}
}

void AEnemy::CheckCombatTarget()
{
	// const double DistanceToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).Size();	//Mamy to w InTargetRange. Obliczamy odleg�o�� mi�dzy nami a naszym "enemy"(CombatTarget)
	if (IsOutsideCombatRadius())	//Je�li odleg�o�� mi�dzy nami a naszym "enemy"(CombatTarget) jest wi�ksza ni� CombatRadius to:
	{
		ClearAttackTimer();	//Wywo�ujemy funkcj� ClearAttackTimer
		LoseInterest();	//Wywo�ujemy funkcj� LoseInterest
		if (!IsEngaged()) StartPatrolling();	//Wywo�ujemy funkcj� StartPatrolling


		UE_LOG(LogTemp, Warning, TEXT("Lose interest"))
	}
	else if (IsOutsideAttackRadius() && !IsChasing())	//Je�li odleg�o�� mi�dzy nami a naszym "enemy"(CombatTarget) jest wi�ksza ni� AttackRadius i EnemyState nie jest r�wny EES_Chasing to:
	{
		ClearAttackTimer();
		if (!IsEngaged()) ChaseTarget();	//Wywo�ujemy funkcj� ChaseTarget

		UE_LOG(LogTemp, Warning, TEXT("Chasing"))
	}
	else if (CanAttack())	//Je�li odleg�o�� mi�dzy nami a naszym "enemy"(CombatTarget) jest mniejsza ni� AttackRadius i EnemyState nie jest r�wny EES_Attacking to:
	{
		/*// Inside attack range, attack character
		EnemyState = EEnemyState::EES_Attacking;	//Ustawiamy EnemyState na EES_Attacking
		// TODO: Attack montage
		Attack();	//Wywo�ujemy funkcj� Attack
		*/
		StartAttackTimer();	//Funkcja do rozpocz�cia timera ataku
		UE_LOG(LogTemp, Warning, TEXT("Attacking"))
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);	// Wywo�ujemy funkcj� MoveToTarget z argumentem PatrolTarget
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);	//I ustawiamy widoczno�� paska �ycia na false
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);	//I ustawiamy widoczno�� paska �ycia na true
	}
}

// Outside combat radius, lose interest
void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;	//Ustawiamy CombatTarget na nullptr
	HideHealthBar();	//Wywo�ujemy funkcj� HideHealthBar
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;	//Ustawiamy EnemyState na EES_Patrolling
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;	//Ustawiamy maksymaln� pr�dko�� chodzenia na 150
	MoveToTarget(PatrolTarget);	//Wywo�ujemy funkcj� MoveToTarget z argumentem PatrolTarget
}

void AEnemy::ChaseTarget()	// Outside attack range, chase character
{
	EnemyState = EEnemyState::EES_Chasing;	//Ustawiamy EnemyState na EES_Chasing
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;	//Ustawiamy maksymaln� pr�dko�� pogoni
	MoveToTarget(CombatTarget);	//Wywo�ujemy funkcj� MoveToTarget z argumentem CombatTarget
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead; // Sprawdzamy czy EnemyState jest r�wny EES_Dead, je�li tak to zwracamy
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);	//Czy�cimy timer
}

void AEnemy::StartAttackTimer()	//Funkcja do rozpocz�cia timera ataku
{
	EnemyState = EEnemyState::EES_Attacking;	//Ustawiamy EnemyState na EES_Attacking
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);		//Losujemy czas ataku
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime); 	//Ustawiamy timer na czas ataku
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);	//Czy�cimy timer ataku
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)	// Deklarujemy funkcj� InTargetRange z Enemy.h
{
	if (Target == nullptr) return false;	// Sprawdzamy czy Target nie jest nullpointerem
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();	//Obliczamy odleg�o�� mi�dzy nami a naszym "enemy"(CombatTarget)
	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_SPHERE_SingleFrame(Target->GetActorLocation());
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;	// Sprawdzamy czy EnemyController i Target nie s� nullpointerami
	FAIMoveRequest MoveRequest;	// Tworzymy struktur� FAIMoveRequest
	MoveRequest.SetGoalActor(Target);	// Ustawiamy cel ruchu na PatrolTarget
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);	// Ustawiamy promie� akceptacji
	//FNavPathSharedPtr NavPath;	// Tworzymy wska�nik do �cie�ki nawigacji
	EnemyController->MoveTo(MoveRequest/*, &NavPath*/);	// Wywo�ujemy funkcj� MoveTo z kontrolera przeciwnika. &NavPath w���zamy, gdy chcemy zobaczyc �ci�k� poruszania si�
	/*
	TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();	// Tworzymy tablic� PathPoints i przypisujemy jej warto�� �cie�ki nawigacji
	//P�tla for, kt�ra rysuje kule debugowania na �cie�ce nawigacji
	for (auto& Point : PathPoints)
	{
		const FVector& Location = Point.Location;	// Pobieramy lokalizacj� punktu
		DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 10.f);	// Rysujemy sfer� debugowania
	}
	*/
}

AActor* AEnemy::ChoosePatrolTarget()	// Deklarujemy funkcj� ChoosePatrolTarget z Enemy.h
{
	TArray<AActor*> ValidTargets;	// Tworzymy tablic� ValidTargets
	for (AActor* Target : PatrolTargets)	// P�tla for, kt�ra sprawdza czy Target jest r�ny od PatrolTarget
	{
		if (Target != PatrolTarget)	// Je�li Target jest r�ny od PatrolTarget, to dodajemy go do tablicy ValidTargets
		{
			ValidTargets.AddUnique(Target);		// Dodajemy Target do tablicy ValidTargets
		}
	}


	const int32 NumPatrolTargets = ValidTargets.Num();	//Pobieramy ilo�� PatrolTargets
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);	//Losujemy numer z tablicy PatrolTargets
		return ValidTargets[TargetSelection];	//Przypisujemy Target warto�� PatrolTargets[TargetSelection]
		/*PatrolTarget = Target;	//Przypisujemy PatrolTarget warto�� Target

		FAIMoveRequest MoveRequest;	// Tworzymy struktur� FAIMoveRequest
		MoveRequest.SetGoalActor(PatrolTarget);	// Ustawiamy cel ruchu na PatrolTarget
		MoveRequest.SetAcceptanceRadius(15.f);	// Ustawiamy promie� akceptacji na 15
		EnemyController->MoveTo(MoveRequest);	// Wywo�ujemy funkcj� MoveTo z kontrolera przeciwnika*/
	}
	return nullptr;
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();	// Pobieramy �wiat
	if (World && WeaponClass)	// 
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);	// Tworzymy domy�ln� bro�
		DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);	// Wyposa�amy bro�
		EquippedWeapon = DefaultWeapon;	// Przypisujemy EquippedWeapon warto�� DefaultWeapon
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&	// Sprawdzamy czy EnemyState nie jest r�wny EES_Dead
		EnemyState != EEnemyState::EES_Chasing &&	// Sprawdzamy czy EnemyState nie jest r�wny EES_Chasing
		EnemyState < EEnemyState::EES_Attacking &&	// Sprawdzamy czy EnemyState jest mniejszy od EES_Attacking
		SeenPawn->ActorHasTag(FName("EngageableTarget")) &&	// Sprawdzamy czy Actor ma tag
		!SeenPawn->ActorHasTag(FName("Dead"));	// Sprawdzamy czy Actor ma tag, jesli ma to nie podbiegamy do niego gdy ju� umar�

	if (bShouldChaseTarget)	//Je�li bShouldChaseTarget jest true to:
	{
		CombatTarget = SeenPawn;	// Przypisujemy CombatTarget warto�� SeenPawn
		ClearPatrolTimer();	// Wywo�ujemy funkcj� ClearPatrolTimer
		ChaseTarget();	// Wywo�ujemy funkcj� ChaseTarget
	}
	
	/*Powy�ej jest skr�cona wersja poni�szych warunk�w
	
	kiedy ma byc goniony bohater warunki poni�ej
	if (EnemyState == EEnemyState::EES_Chasing) return;	// Sprawdzamy czy EnemyState jest r�wny EES_Chasing, sprawdzamy to tutaj poniewa� chcemy aby poni�szy statement wykona� si� tylko raz a nie ci�gle
	if (SeenPawn->ActorHasTag(FName("SlashCharacter")))	// Sprawdzamy czy Actor ma tag
	{
		ClearPatrolTimer();	// Wywo�ujemy funkcj� ClearPatrolTimer
		GetCharacterMovement()->MaxWalkSpeed = 300.f;	// Ustawiamy maksymaln� pr�dko�� chodzenia na 300
		CombatTarget = SeenPawn;	// Przypisujemy CombatTarget warto�� SeenPawn

		if (EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Chasing;	// Ustawiamy EnemyState na EES_Chasing
			MoveToTarget(CombatTarget);	// Wywo�ujemy funkcj� MoveToTarget z argumentem CombatTarget i powinien zacz�� nas goni�
			UE_LOG(LogTemp, Warning, TEXT("Pawn Seen! and chasing"))
		}
	}
	*/
}


