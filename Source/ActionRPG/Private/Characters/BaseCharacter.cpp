// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"	//Potrzebujemy tego nag��wka aby m�c u�y� BoxComponent
#include "Items/Weapons/Weapon.h"	//Potrzebujemy tego nag��wka aby m�c podnie�� bro� ora u�y� AWeapon
#include "Components/AttributeComponent.h"	// Potrzebujemy tego nag��wka aby nasz "BaseCharacter" m�g� dziedziczy� z funkcji AttributeComponent
#include "Components/CapsuleComponent.h" // for UCapsuleComponent"
#include "Kismet/GameplayStatics.h"
#include "ActionRPG/DebugMacros.h"
#include "Characters/CharacterTypes.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Tworzymy domy�lny subobiekt "Attributes" z klasy UAttributeComponent
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // Set the collision response to Camera
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter)	// Sprawdzamy czy Attributes nie jest nullpointerem i czy "enemy" �yje
	{
		DirectionalHitReact(Hitter->GetActorLocation());	// Wywo�ujemy funkcj� DirectionalHitReact z argumentem ImpactPoint
	}
	else Die();	// Wywo�ujemy funkcj� Die
	

	PlayHitSound(ImpactPoint);	// Wywo�ujemy funkcj� PlayHitSound z argumentem ImpactPoint. // Odtwarzamy d�wi�k otrzymania ciosu

	SpawnHitParticles(ImpactPoint);	// Wywo�ujemy funkcj� SpawnHitParticles z argumentem ImpactPoint. // Odtwarzamy system particle  otrzymania ciosu
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::Die_Implementation()
{
	Tags.Add("Dead");
	PlayDeathMontage();	// Odtwarzamy animacj� �mierci
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)	//Je�li AnimInstance i EquipMontage nie s� nullpointerami, to odtwarzamy animacj� equip
	{
		AnimInstance->Montage_Play(HitReactMontage);
		//Po wyborze sekcji animacji, odtwarzamy j�
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}
// Deklarujemy funkcj� DirectionalHitReact z argumentem ImpactPoint kt�ra pokazuje z jakiego kierunku otrzymali�my cios
void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();	// Pobieramy wektor Forward dla naszego "enemy"
	// Lower Impact Point to the Enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();	// Pobieramy wektor ToHit dla naszego "enemy". Otrzymujemy wektor (ImpactPoint - GetActorLocation()) i normalizujemy go (obliczamy wektor jednostkowy). GetSafeNormal powoduje, �e nie b�dziemy dzieli� przez 0

	//	Forward * ToHit = (magnitude)|Forward| * |ToHit| * cos(theta) // Obliczamy k�t mi�dzy Forward i ToHit
	// |Forward| = 1, |ToHit| = 1, Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);	// Obliczamy k�t mi�dzy Forward i ToHit
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	//	convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta); // Zamieniamy radiany na stopnie

	//if CrossProduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	if (CrossProduct.Z < 0)
	{
		Theta *= -1;	// Je�li CrossProduct.Z jest mniejsze od 0, to Theta mno�ymy przez -1
	}

	FName Section("FromBack");	// Deklarujemy zmienn� Section i przypisujemy jej warto�� "FromBack"

	if (Theta >= -45.f && Theta < 45.f)	// Je�li Theta jest wi�ksze lub r�wne -45 i mniejsze od 45
	{
		Section = FName("FromFront");	// Przypisujemy zmiennej Section warto�� "FromFront"
	}
	else if (Theta >= -135.f && Theta < -45.f)	// Je�li Theta jest wi�ksze lub r�wne -135 i mniejsze od -45
	{
		Section = FName("FromLeft");	// Przypisujemy zmiennej Section warto�� "FromLeft"
	}
	else if (Theta >= 45.f && Theta < 135.f)	// Je�li Theta jest wi�ksze lub r�wne 45 i mniejsze od 135
	{
		Section = FName("FromRight");	// Przypisujemy zmiennej Section warto�� "FromRight"
	}
	/*else if (Theta >= 135.f || Theta < -135.f)	// Je�li Theta jest wi�ksze lub r�wne 135 lub mniejsze od -135
	{
	Section = "FromBack";	// Przypisujemy zmiennej Section warto�� "FromBack"
	}*/


	PlayHitReactMontage(Section);	// Odtwarzamy animacj� otrzymania ciosu z danego kierunku

	/* Je�li chcemy zobaczy� jak dzia�a nasz kod, mo�emy odkomentowa� poni�sze linie
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

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)	// Sprawdzamy czy HitSound nie jest nullpointerem
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint);	// Odtwarzamy d�wi�k otrzymania ciosu
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld())	// Sprawdzamy czy HitParticles nie jest nullpointerem
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint);	// Odtwarzamy particle system otrzymania ciosu
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);	// Wywo�ujemy funkcj� ReceiveDamage z klasy UAttributeComponent
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();	//Pobieramy AnimInstance
	//Sprawdzamy czy to nie jest nullpointer
	if (AnimInstance && Montage)	//Je�li AnimInstance i AttackMontage nie s� nullpointerami, to odtwarzamy animacj� ataku
	{
		AnimInstance->Montage_Play(Montage);
		//Po wyborze sekcji animacji, odtwarzamy j�
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	//Dzi�ki sposobowi poni�ej mo�emy dodawac i usuwa� nielimitowan� ilo�� sekcji animacji ataku, bez potrzeby ich r�cznego dodawania do kodu
	if (SectionNames.Num() <= 0) return -1;	// Sprawdzamy czy SectionNames nie jest mniejsze lub r�wne 0
	const int32 MaxSectionIndex = SectionNames.Num() - 1;	// Pobieramy maksymalny indeks sekcji. Musimy ustawi� -1 aby nie wyj�� poza zakres sekcji i nieywo�a� b��du
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);	//Losujemy numer sekcji
	PlayMontageSection(Montage, SectionNames[Selection]);	//Wywo�ujemy funkcj� PlayMontageSection z AttackMontage i SectionNames[Selection]

	return Selection;
}

int32 ABaseCharacter::PlayAttackMontage()
{

	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);

	/*Dzi�ki sposobowi poni�ej mo�emy dodawac i usuwa� nielimitowan� ilo�� sekcji animacji ataku, bez potrzeby ich r�cznego dodawania do kodu
	if (AttackMontageSections.Num() <= 0) return;	// Sprawdzamy czy AttackMontageSections nie jest mniejsze lub r�wne 0
	const int32 MaxSectionIndex = AttackMontageSections.Num() - 1;	// Pobieramy maksymalny indeks sekcji. Musimy ustawi� -1 aby nie wyj�� poza zakres sekcji i nieywo�a� b��du
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);	//Losujemy numer sekcji
	PlayMontageSection(AttackMontage, AttackMontageSections[Selection]);	//Wywo�ujemy funkcj� PlayMontageSection z AttackMontage i AttackMontageSections[Selection]
	*/
	/*Stary spos�b odtwarzania animacji, powy�ej zmieniony na wydajnieszy
	Super::PlayAttackMontage();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();	//Pobieramy AnimInstance
	//Sprawdzamy czy to nie jest nullpointer
	if (AnimInstance && AttackMontage)	//Je�li AnimInstance i AttackMontage nie s� nullpointerami, to odtwarzamy animacj� ataku
	{
		AnimInstance->Montage_Play(AttackMontage);
		//Mamy 3 sekcje w animacji ataku, wi�c losujemy kt�ra z nich zostanie odtworzona, wi�c dodajemy liczb� losow� 0, 1 albo 2
		const int32 Selection = FMath::RandRange(0, 2);	//Troch� jak rzut monet�, generuje nam 0, 1 albo 2
		//Tworzymy zmienn�, kt�ra b�dzie przechowywa� nazw� sekcji animacji - pozostawiamy j� pust� poniewa� sekcja zostanie wybrana przez switch.
		FName SectionName = FName();
		//Wybieramy sekcj� animacji ataku i zmieniamy si� pomi�dzy nimi
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			//Break jest potrzebny, �eby wyj�� z p�tli switch
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		case 2:
			SectionName = FName("Attack3");
			break;
		default:
			break;

		}
		//Po wyborze sekcji animacji, odtwarzamy j�
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
	*/
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Default"));
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();	//Pobieramy AnimInstance
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.2f, AttackMontage);	//Zatrzymujemy animacj� ataku
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();	//Je�li CombatTarget jest nullptr, to zwracamy pusty wektor
	
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();	//Pobieramy lokalizacj� CombatTarget
	const FVector Location = GetActorLocation();	//Pobieramy lokalizacj� naszego "enemy"

	const FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();	//Obliczamy wektor od celu do nas
	TargetToMe * WarpTargetDistance;	//Mno�ymy wektor przez WarpTargetDistance

	//DRAW_SPHERE(CombatTargetLocation + TargetToMe);	// Rysuje sfere w miejscu gdzie jest cel
	return CombatTargetLocation + TargetToMe;	//Zwracamy lokalizacj� celu + wektor
}

FVector ABaseCharacter::GetRotationWarpTarget()	//We want our enemy face to CombatTarget
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::DisableCapsule() // Wy��czamy kolizj� kapsu�y po �mierci przeciwnika
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// Wy��czamy kolizj� kapsu�y po �mierci przeciwnika
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);	//Wy��czamy kolizj� na meshe postaci aby nie m�g� przeciwnik zada� obra�e� po �mierci
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::DodgeEnd()
{
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())	//Je�li bro� jest wyekwipowana i nie jest nullptr, to wtedy mo�emy w��czy� lub wy��czy� kolizj�
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();	//Czyscimy tablic� aktor�w, kt�r� mamy w broni
	}
}


