// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h" // for USkeletalMeshComponent
#include "Components/CapsuleComponent.h" // for UCapsuleComponent"
#include "GameFramework/CharacterMovementComponent.h" // for UCharacterMovementComponent
#include "ActionRPG/DebugMacros.h"	// Potrzebujemy tego nag³ówka aby móc u¿ywaæ makr debuguj¹cych
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"	// Potrzebujemy tego nag³ówka aby nasz "AEnemy" móg³ dziedziczyæ z funkcji AttributeComponent
#include "HUD/HealthBarComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"



// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);	// Set the collision object type to WorldDynamic
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);	// Set the collision response to Visibility)
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);	// Set the collision response to Camera
	GetMesh()->SetGenerateOverlapEvents(true);	// Set the mesh to generate overlap events
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // Set the collision response to Camera
	
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));	// Tworzymy domyœlny subobiekt "Attributes" z klasy UAttributeComponent
	
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));	// Tworzymy domyœlny subobiekt "HealthBarWidget" z klasy UHealthBarComponent
	HealthBarWidget->SetupAttachment(GetRootComponent());	// Ustawiamy HealthBarWidget jako podobiekt do naszego "enemy"

	GetCharacterMovement()->bOrientRotationToMovement = true;	// Obracamy naszego "enemy" w kierunku ruchu
	
	bUseControllerRotationPitch = false;	// Nie u¿ywamy kontrolera do obracania siê w osi X
	bUseControllerRotationYaw = false;	// Nie u¿ywamy kontrolera do obracania siê w osi Y
	bUseControllerRotationRoll = false;	// Nie u¿ywamy kontrolera do obracania siê w osi Z
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (HealthBarWidget)	// Sprawdzamy czy HealthBarWidget nie jest nullpointerem
	{
		HealthBarWidget->SetVisibility(false);	// Ustawiamy widocznoœæ paska ¿ycia na false
	}
	
	//	Przypisujemy wskaŸnik do kontrolera przeciwnika
	EnemyController = Cast<AAIController>(GetController());
	if (EnemyController && PatrolTarget)
	{
		FAIMoveRequest MoveRequest;	// Tworzymy strukturê FAIMoveRequest
		MoveRequest.SetGoalActor(PatrolTarget);	// Ustawiamy cel ruchu na PatrolTarget
		MoveRequest.SetAcceptanceRadius(15.f);	// Ustawiamy promieñ akceptacji na 15
		FNavPathSharedPtr NavPath;	// Tworzymy wskaŸnik do œcie¿ki nawigacji
		EnemyController->MoveTo(MoveRequest, &NavPath);	// Wywo³ujemy funkcjê MoveTo z kontrolera przeciwnika
		TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();	// Tworzymy tablicê PathPoints i przypisujemy jej wartoœæ œcie¿ki nawigacji
		//Pêtla for, która rysuje kule debugowania na œcie¿ce nawigacji
		for (auto& Point : PathPoints)
		{
			const FVector& Location = Point.Location;	// Pobieramy lokalizacjê punktu
			DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 10.f);	// Rysujemy sferê debugowania
		}
	}
}

void AEnemy::Die()
{
	//	TODO: play death montage
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)	//Jeœli AnimInstance i EquipMontage nie s¹ nullpointerami, to odtwarzamy animacjê equip
	{
		AnimInstance->Montage_Play(DeathMontage);
		//Mamy dwie sekcje w animacji ataku, wiêc losujemy która z nich zostanie odtworzona, wiêc dodajemy liczbê losow¹ 0 albo 1
		const int32 Selection = FMath::RandRange(0, 5);	//Trochê jak rzut monet¹, generuje nam 0 albo 1
		//Tworzymy zmienn¹, która bêdzie przechowywaæ nazwê sekcji animacji - pozostawiamy j¹ pust¹ poniewa¿ sekcja zostanie wybrana przez switch.
		FName SectionName = FName();
		//Wybieramy sekcjê animacji ataku i zmieniamy siê pomiêdzy nimi
		switch (Selection)
		{
		case 0:
			SectionName = FName("Death1");
			DeathPose = EDeathPose::EDP_Death1;	// Przypisujemy zmiennej DeathPose wartoœæ EDP_Death1
			//Break jest potrzebny, ¿eby wyjœæ z pêtli switch
			break;
		case 1:
			SectionName = FName("Death2");
			DeathPose = EDeathPose::EDP_Death2;
			break;
		case 2:
			SectionName = FName("Death3");
			DeathPose = EDeathPose::EDP_Death3;
			break;
		case 3:
			SectionName = FName("Death4");
			DeathPose = EDeathPose::EDP_Death4;
			break;
		case 4:
			SectionName = FName("Death5");
			DeathPose = EDeathPose::EDP_Death5;
			break;
		case 5:
			SectionName = FName("Death6");
			DeathPose = EDeathPose::EDP_Death6;
			break;
		default:
			break;
		}
		//Po wyborze sekcji animacji, odtwarzamy j¹
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
	//Jeœli przeciwnik ma HealthBarWidget, to ustawiamy jego widocznoœæ na false w momencie œmierci
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);	//Ustawiamy widocznoœæ paska ¿ycia na false
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// Wy³¹czamy kolizjê kapsu³y po œmierci przeciwnika
	SetLifeSpan(5.f);	// Ustawiamy czas po którym, cia³o przeciwnika znika po 3 sekundach od jego œmierci
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)	// Deklarujemy funkcjê PlayHitReactMontage z Enemy.h
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)	//Jeœli AnimInstance i EquipMontage nie s¹ nullpointerami, to odtwarzamy animacjê equip
	{
		AnimInstance->Montage_Play(HitReactMontage);
		//Po wyborze sekcji animacji, odtwarzamy j¹
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CombatTarget)
	{
		const double DistanceToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).Size();	//Obliczamy odleg³oœæ miêdzy nami a naszym "enemy"(CombatTarget)
		if (DistanceToTarget > CombatRadius)	//Jeœli odleg³oœæ miêdzy nami a naszym "enemy"(CombatTarget) jest wiêksza ni¿ CombatRadius to:
		{
			CombatTarget = nullptr;	//Ustawiamy CombatTarget na nullptr
			if (HealthBarWidget)
			{
				HealthBarWidget->SetVisibility(false);	//I ustawiamy widocznoœæ paska ¿ycia na false
			}
		}
	}

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)	// Deklarujemy funkcjê GetHit z Enemy.h
{
	//DRAW_SPHRE_COLOR(ImpactPoint, FColor::Orange);	// Rysujemy kulkê w kolorze pomarañczowym gdy uderzymy mieczem w "enemy"
	if (HealthBarWidget)	// Sprawdzamy czy HealthBarWidget nie jest nullpointerem
	{
		HealthBarWidget->SetVisibility(true);	// Ustawiamy widocznoœæ paska ¿ycia na true w momencie gdy przeciwnik otrzyma cios
	}

	if (Attributes && Attributes->IsAlive())	// Sprawdzamy czy Attributes nie jest nullpointerem i czy "enemy" ¿yje
	{
		DirectionalHitReact(ImpactPoint);	// Wywo³ujemy funkcjê DirectionalHitReact z argumentem ImpactPoint
	}
	else
	{
		Die();	// Wywo³ujemy funkcjê Die
	}
	
	if (HitSound)	// Sprawdzamy czy HitSound nie jest nullpointerem
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint);	// Odtwarzamy dŸwiêk otrzymania ciosu
	}
	if (HitParticles && GetWorld())	// Sprawdzamy czy HitParticles nie jest nullpointerem
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint);	// Odtwarzamy particle system otrzymania ciosu
	}
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)	// Deklarujemy funkcjê DirectionalHitReact z argumentem ImpactPoint która pokazuje z jakiego kierunku otrzymaliœmy cios
{
	const FVector Forward = GetActorForwardVector();	// Pobieramy wektor Forward dla naszego "enemy"
	// Lower Impact Point to the Enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();	// Pobieramy wektor ToHit dla naszego "enemy". Otrzymujemy wektor (ImpactPoint - GetActorLocation()) i normalizujemy go (obliczamy wektor jednostkowy). GetSafeNormal powoduje, ¿e nie bêdziemy dzieliæ przez 0

	//	Forward * ToHit = (magnitude)|Forward| * |ToHit| * cos(theta) // Obliczamy k¹t miêdzy Forward i ToHit
	// |Forward| = 1, |ToHit| = 1, Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);	// Obliczamy k¹t miêdzy Forward i ToHit
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	//	convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta); // Zamieniamy radiany na stopnie

	//if CrossProduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	if (CrossProduct.Z < 0)
	{
		Theta *= -1;	// Jeœli CrossProduct.Z jest mniejsze od 0, to Theta mno¿ymy przez -1
	}

	FName Section("FromBack");	// Deklarujemy zmienn¹ Section i przypisujemy jej wartoœæ "FromBack"

	if (Theta >= -45.f && Theta < 45.f)	// Jeœli Theta jest wiêksze lub równe -45 i mniejsze od 45
	{
		Section = FName("FromFront");	// Przypisujemy zmiennej Section wartoœæ "FromFront"
	}
	else if (Theta >= -135.f && Theta < -45.f)	// Jeœli Theta jest wiêksze lub równe -135 i mniejsze od -45
	{
		Section = FName("FromLeft");	// Przypisujemy zmiennej Section wartoœæ "FromLeft"
	}
	else if (Theta >= 45.f && Theta < 135.f)	// Jeœli Theta jest wiêksze lub równe 45 i mniejsze od 135
	{
		Section = FName("FromRight");	// Przypisujemy zmiennej Section wartoœæ "FromRight"
	}
	/*else if (Theta >= 135.f || Theta < -135.f)	// Jeœli Theta jest wiêksze lub równe 135 lub mniejsze od -135
	{
	Section = "FromBack";	// Przypisujemy zmiennej Section wartoœæ "FromBack"
	}*/


	PlayHitReactMontage(Section);	// Odtwarzamy animacjê otrzymania ciosu z danego kierunku

	/* Jeœli chcemy zobaczyæ jak dzia³a nasz kod, mo¿emy odkomentowaæ poni¿sze linie
	// Draw a debug arrow to show the cross product
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);

	// Print the angle to the screen
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta));
	}
	// Draw a debug arrow to show the forward direction
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	// Draw a debug arrow from the enemy's location to the hit location
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
	*/
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes && HealthBarWidget)
	{
		Attributes->ReceiveDamage(DamageAmount);	// Wywo³ujemy funkcjê ReceiveDamage z klasy UAttributeComponent

		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());	
		
	}
	CombatTarget = EventInstigator->GetPawn();	// Przypisujemy CombatTarget wartoœæ EventInstigator->GetPawn()
	return DamageAmount;	
}

