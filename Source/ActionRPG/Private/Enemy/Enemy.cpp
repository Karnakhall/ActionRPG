// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h" // for USkeletalMeshComponent
#include "GameFramework/CharacterMovementComponent.h" // for UCharacterMovementComponent
#include "Perception/PawnSensingComponent.h"	// for UPawnSensingComponent
#include "Components/AttributeComponent.h"	// Potrzebujemy tego nag³ówka aby nasz "AEnemy" móg³ dziedziczyæ z funkcji AttributeComponent
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"	// Potrzebujemy tego nag³ówka aby nasz "AEnemy" móg³ dziedziczyæ z funkcji Weapon
#include "Kismet/KismetSystemLibrary.h"


#include "Navigation/PathFollowingComponent.h"
#include "ActionRPG/DebugMacros.h"	// Potrzebujemy tego nag³ówka aby móc u¿ywaæ makr debuguj¹cych



// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);	// Set the collision object type to WorldDynamic
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);	// Set the collision response to Visibility)
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);	// Set the collision response to Camera
	GetMesh()->SetGenerateOverlapEvents(true);	// Set the mesh to generate overlap events
	
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));	// Tworzymy domyœlny subobiekt "HealthBarWidget" z klasy UHealthBarComponent
	HealthBarWidget->SetupAttachment(GetRootComponent());	// Ustawiamy HealthBarWidget jako podobiekt do naszego "enemy"

	GetCharacterMovement()->bOrientRotationToMovement = true;	// Obracamy naszego "enemy" w kierunku ruchu
	
	bUseControllerRotationPitch = false;	// Nie u¿ywamy kontrolera do obracania siê w osi X
	bUseControllerRotationYaw = false;	// Nie u¿ywamy kontrolera do obracania siê w osi Y
	bUseControllerRotationRoll = false;	// Nie u¿ywamy kontrolera do obracania siê w osi Z

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));	// Tworzymy domyœlny subobiekt "PawnSensing" z klasy UPawnSensingComponent
	PawnSensing->SetPeripheralVisionAngle(45.f);	// Ustawiamy k¹t widzenia na 45 stopni
	PawnSensing->SightRadius = 2000.f;	// Ustawiamy promieñ widzenia na 1200
	PawnSensing->HearingThreshold = 600.f;	// Ustawiamy próg s³yszenia na 600

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead()) return;	// Wywo³ujemy funkcjê IsDed która sprawdza czy EnemyState jest równy EES_Dead, jeœli tak to zwracamy
	if (EnemyState > EEnemyState::EES_Patrolling)	//Jeœli EnemyState jest wiêkszy od EES_Patrolling to:
	{
		CheckCombatTarget();	// Wywo³ujemy funkcjê CheckCombatTarget
	}
	else
	{
		CheckPatrolTarget();	// Wywo³ujemy funkcjê CheckPatrolTarget
	}

	/*if (PatrolTarget && EnemyController) ca³y kod poni¿ej mam powy¿szym if statementem
	{
		if (InTargetRange(PatrolTarget, PatrolRadius))	//Jeœli odleg³oœæ miêdzy nami a naszym "enemy"(PatrolTarget) jest mniejsza ni¿ PatrolRadius to:
		{
			TArray<AActor*> ValidTargets;	// Tworzymy tablicê ValidTargets
			for (AActor* Target : PatrolTargets)	// Pêtla for, która sprawdza czy Target jest ró¿ny od PatrolTarget
			{
				if (Target != PatrolTarget)	// Jeœli Target jest ró¿ny od PatrolTarget, to dodajemy go do tablicy ValidTargets
				{
					ValidTargets.AddUnique(Target);		// Dodajemy Target do tablicy ValidTargets
				}
			}


			const int32 NumPatrolTargets = PatrolTargets.Num() - 1;	//Pobieramy iloœæ PatrolTargets
			if (NumPatrolTargets > 0)
			{
				const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets);	//Losujemy numer z tablicy PatrolTargets
				AActor* Target = PatrolTargets[TargetSelection];	//Przypisujemy Target wartoœæ PatrolTargets[TargetSelection]
				PatrolTarget = Target;	//Przypisujemy PatrolTarget wartoœæ Target

				FAIMoveRequest MoveRequest;	// Tworzymy strukturê FAIMoveRequest
				MoveRequest.SetGoalActor(PatrolTarget);	// Ustawiamy cel ruchu na PatrolTarget
				MoveRequest.SetAcceptanceRadius(15.f);	// Ustawiamy promieñ akceptacji na 15
				EnemyController->MoveTo(MoveRequest);	// Wywo³ujemy funkcjê MoveTo z kontrolera przeciwnika
			}

		}
	}
	*/
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);	// Wywo³ujemy funkcjê HandleDamage z argumentem DamageAmount
	CombatTarget = EventInstigator->GetPawn();	// Przypisujemy CombatTarget wartoœæ EventInstigator->GetPawn()
	
	if (IsInsideAttackRadius())	//Jeœli postaæ jest w zasiêgu ataku to ustawiamy "enemy" state na attak
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
		EquippedWeapon->Destroy();	// Usuwamy broñ jesli bêdziemy chcieli aby przeciwnik upuszcza³ broñ, mo¿emy to zrobic w³aœnie w tym miejscu
	}
	Super::Destroyed();
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)	// Deklarujemy funkcjê GetHit z Enemy.h
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);	// Wywo³ujemy funkcjê GetHit_Implementation z BaseCharacter
	if (!IsDead()) ShowHealthBar();	//Wywo³ujemy funkcjê ShowHealthBar
	ClearPatrolTimer();	//Wywo³ujemy funkcjê ClearPatrolTimer
	ClearAttackTimer();	//Wywo³ujemy funkcjê ClearAttackTimer

	StopAttackMontage();	//Wywo³ujemy funkcjê StopAttackMontage
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	//DRAW_SPHRE_COLOR(ImpactPoint, FColor::Orange);	// Rysujemy kulkê w kolorze pomarañczowym gdy uderzymy mieczem w "enemy"
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);	// Dodajemy dynamicznie funkcjê PawnSeen do delegata OnSeePawn
	}
	InitializeEnemy();	// Wywo³ujemy funkcjê InitializeEnemy

	Tags.Add(FName("Enemy"));
}

void AEnemy::Die()
{
	Super::Die();	// Wywo³ujemy funkcjê Die z BaseCharacter
	EnemyState = EEnemyState::EES_Dead;	// Ustawiamy EnemyState na EES_Dead
	
	ClearAttackTimer();	// Czyœcimy timer ataku
	//Jeœli przeciwnik ma HealthBarWidget, to ustawiamy jego widocznoœæ na false w momencie œmierci
	HideHealthBar();

	DisableCapsule();	// Wywo³ujemy funkcjê DisableCapsule, która wy³¹æza kolizjê kapsu³y po smierci
	SetLifeSpan(DeathLifeSpan);	// Ustawiamy czas po którym, cia³o przeciwnika znika po 3 sekundach od jego œmierci
	GetCharacterMovement()->bOrientRotationToMovement = false;	// Wy³¹czamy obracanie siê w kierunku ruchu
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);		// Wy³¹czamy kolizjê broni gdy przeicwnik umiera
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
	CheckCombatTarget();	//Wywo³ujemy funkcjê CheckCombatTarget
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


//Funkcja do inicjalizacji przeciwnika. Ustawa stan pocz¹tkowy, ustawia kontroler AI, ustawia cel patrolu, ustawia pasek ¿ycia
void AEnemy::InitializeEnemy()
{
	//	Przypisujemy wskaŸnik do kontrolera przeciwnika
	EnemyController = Cast<AAIController>(GetController());

	// Wywo³ujemy funkcjê MoveToTarget z argumentem PatrolTarget
	MoveToTarget(PatrolTarget);

	//Wywo³ujemy funkcjê HideHealthBar która ukrywa pasek ¿ycia
	HideHealthBar();

	// Wywo³ujemy funkcjê SpawnDefaultWeapon która tworzy spawnuje broñ
	SpawnDefaultWeapon();
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();	// Wywo³ujemy funkcjê ChoosePatrolTarget
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);	// Losujemy czas oczekiwania
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);	// Ustawiamy timer na 5 sekund
		//MoveToTarget(PatrolTarget);	// Wywo³ujemy funkcjê MoveToTarget z argumentem PatrolTarget

	}
}

void AEnemy::CheckCombatTarget()
{
	// const double DistanceToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).Size();	//Mamy to w InTargetRange. Obliczamy odleg³oœæ miêdzy nami a naszym "enemy"(CombatTarget)
	if (IsOutsideCombatRadius())	//Jeœli odleg³oœæ miêdzy nami a naszym "enemy"(CombatTarget) jest wiêksza ni¿ CombatRadius to:
	{
		ClearAttackTimer();	//Wywo³ujemy funkcjê ClearAttackTimer
		LoseInterest();	//Wywo³ujemy funkcjê LoseInterest
		if (!IsEngaged()) StartPatrolling();	//Wywo³ujemy funkcjê StartPatrolling


		UE_LOG(LogTemp, Warning, TEXT("Lose interest"))
	}
	else if (IsOutsideAttackRadius() && !IsChasing())	//Jeœli odleg³oœæ miêdzy nami a naszym "enemy"(CombatTarget) jest wiêksza ni¿ AttackRadius i EnemyState nie jest równy EES_Chasing to:
	{
		ClearAttackTimer();
		if (!IsEngaged()) ChaseTarget();	//Wywo³ujemy funkcjê ChaseTarget

		UE_LOG(LogTemp, Warning, TEXT("Chasing"))
	}
	else if (CanAttack())	//Jeœli odleg³oœæ miêdzy nami a naszym "enemy"(CombatTarget) jest mniejsza ni¿ AttackRadius i EnemyState nie jest równy EES_Attacking to:
	{
		/*// Inside attack range, attack character
		EnemyState = EEnemyState::EES_Attacking;	//Ustawiamy EnemyState na EES_Attacking
		// TODO: Attack montage
		Attack();	//Wywo³ujemy funkcjê Attack
		*/
		StartAttackTimer();	//Funkcja do rozpoczêcia timera ataku
		UE_LOG(LogTemp, Warning, TEXT("Attacking"))
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);	// Wywo³ujemy funkcjê MoveToTarget z argumentem PatrolTarget
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);	//I ustawiamy widocznoœæ paska ¿ycia na false
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);	//I ustawiamy widocznoœæ paska ¿ycia na true
	}
}

// Outside combat radius, lose interest
void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;	//Ustawiamy CombatTarget na nullptr
	HideHealthBar();	//Wywo³ujemy funkcjê HideHealthBar
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;	//Ustawiamy EnemyState na EES_Patrolling
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;	//Ustawiamy maksymaln¹ prêdkoœæ chodzenia na 150
	MoveToTarget(PatrolTarget);	//Wywo³ujemy funkcjê MoveToTarget z argumentem PatrolTarget
}

void AEnemy::ChaseTarget()	// Outside attack range, chase character
{
	EnemyState = EEnemyState::EES_Chasing;	//Ustawiamy EnemyState na EES_Chasing
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;	//Ustawiamy maksymaln¹ prêdkoœæ pogoni
	MoveToTarget(CombatTarget);	//Wywo³ujemy funkcjê MoveToTarget z argumentem CombatTarget
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
	return EnemyState == EEnemyState::EES_Dead; // Sprawdzamy czy EnemyState jest równy EES_Dead, jeœli tak to zwracamy
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);	//Czyœcimy timer
}

void AEnemy::StartAttackTimer()	//Funkcja do rozpoczêcia timera ataku
{
	EnemyState = EEnemyState::EES_Attacking;	//Ustawiamy EnemyState na EES_Attacking
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);		//Losujemy czas ataku
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime); 	//Ustawiamy timer na czas ataku
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);	//Czyœcimy timer ataku
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)	// Deklarujemy funkcjê InTargetRange z Enemy.h
{
	if (Target == nullptr) return false;	// Sprawdzamy czy Target nie jest nullpointerem
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();	//Obliczamy odleg³oœæ miêdzy nami a naszym "enemy"(CombatTarget)
	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_SPHERE_SingleFrame(Target->GetActorLocation());
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;	// Sprawdzamy czy EnemyController i Target nie s¹ nullpointerami
	FAIMoveRequest MoveRequest;	// Tworzymy strukturê FAIMoveRequest
	MoveRequest.SetGoalActor(Target);	// Ustawiamy cel ruchu na PatrolTarget
	MoveRequest.SetAcceptanceRadius(50.f);	// Ustawiamy promieñ akceptacji na 50
	//FNavPathSharedPtr NavPath;	// Tworzymy wskaŸnik do œcie¿ki nawigacji
	EnemyController->MoveTo(MoveRequest/*, &NavPath*/);	// Wywo³ujemy funkcjê MoveTo z kontrolera przeciwnika. &NavPath w³¹æzamy, gdy chcemy zobaczyc œciê¿kê poruszania siê
	/*
	TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();	// Tworzymy tablicê PathPoints i przypisujemy jej wartoœæ œcie¿ki nawigacji
	//Pêtla for, która rysuje kule debugowania na œcie¿ce nawigacji
	for (auto& Point : PathPoints)
	{
		const FVector& Location = Point.Location;	// Pobieramy lokalizacjê punktu
		DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 10.f);	// Rysujemy sferê debugowania
	}
	*/
}

AActor* AEnemy::ChoosePatrolTarget()	// Deklarujemy funkcjê ChoosePatrolTarget z Enemy.h
{
	TArray<AActor*> ValidTargets;	// Tworzymy tablicê ValidTargets
	for (AActor* Target : PatrolTargets)	// Pêtla for, która sprawdza czy Target jest ró¿ny od PatrolTarget
	{
		if (Target != PatrolTarget)	// Jeœli Target jest ró¿ny od PatrolTarget, to dodajemy go do tablicy ValidTargets
		{
			ValidTargets.AddUnique(Target);		// Dodajemy Target do tablicy ValidTargets
		}
	}


	const int32 NumPatrolTargets = ValidTargets.Num();	//Pobieramy iloœæ PatrolTargets
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);	//Losujemy numer z tablicy PatrolTargets
		return ValidTargets[TargetSelection];	//Przypisujemy Target wartoœæ PatrolTargets[TargetSelection]
		/*PatrolTarget = Target;	//Przypisujemy PatrolTarget wartoœæ Target

		FAIMoveRequest MoveRequest;	// Tworzymy strukturê FAIMoveRequest
		MoveRequest.SetGoalActor(PatrolTarget);	// Ustawiamy cel ruchu na PatrolTarget
		MoveRequest.SetAcceptanceRadius(15.f);	// Ustawiamy promieñ akceptacji na 15
		EnemyController->MoveTo(MoveRequest);	// Wywo³ujemy funkcjê MoveTo z kontrolera przeciwnika*/
	}
	return nullptr;
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();	// Pobieramy œwiat
	if (World && WeaponClass)	// 
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);	// Tworzymy domyœln¹ broñ
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);	// Wyposa¿amy broñ
		EquippedWeapon = DefaultWeapon;	// Przypisujemy EquippedWeapon wartoœæ DefaultWeapon
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&	// Sprawdzamy czy EnemyState nie jest równy EES_Dead
		EnemyState != EEnemyState::EES_Chasing &&	// Sprawdzamy czy EnemyState nie jest równy EES_Chasing
		EnemyState < EEnemyState::EES_Attacking &&	// Sprawdzamy czy EnemyState jest mniejszy od EES_Attacking
		SeenPawn->ActorHasTag(FName("EngageableTarget")) &&	// Sprawdzamy czy Actor ma tag
		!SeenPawn->ActorHasTag(FName("Dead"));	// Sprawdzamy czy Actor ma tag, jesli ma to nie podbiegamy do niego gdy ju¿ umar³

	if (bShouldChaseTarget)	//Jeœli bShouldChaseTarget jest true to:
	{
		CombatTarget = SeenPawn;	// Przypisujemy CombatTarget wartoœæ SeenPawn
		ClearPatrolTimer();	// Wywo³ujemy funkcjê ClearPatrolTimer
		ChaseTarget();	// Wywo³ujemy funkcjê ChaseTarget
	}
	
	/*Powy¿ej jest skrócona wersja poni¿szych warunków
	
	kiedy ma byc goniony bohater warunki poni¿ej
	if (EnemyState == EEnemyState::EES_Chasing) return;	// Sprawdzamy czy EnemyState jest równy EES_Chasing, sprawdzamy to tutaj poniewa¿ chcemy aby poni¿szy statement wykona³ siê tylko raz a nie ci¹gle
	if (SeenPawn->ActorHasTag(FName("SlashCharacter")))	// Sprawdzamy czy Actor ma tag
	{
		ClearPatrolTimer();	// Wywo³ujemy funkcjê ClearPatrolTimer
		GetCharacterMovement()->MaxWalkSpeed = 300.f;	// Ustawiamy maksymaln¹ prêdkoœæ chodzenia na 300
		CombatTarget = SeenPawn;	// Przypisujemy CombatTarget wartoœæ SeenPawn

		if (EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Chasing;	// Ustawiamy EnemyState na EES_Chasing
			MoveToTarget(CombatTarget);	// Wywo³ujemy funkcjê MoveToTarget z argumentem CombatTarget i powinien zacz¹æ nas goniæ
			UE_LOG(LogTemp, Warning, TEXT("Pawn Seen! and chasing"))
		}
	}
	*/
}


