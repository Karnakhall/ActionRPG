// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h" // for USkeletalMeshComponent
#include "Components/CapsuleComponent.h" // for UCapsuleComponent"
#include "ActionRPG/DebugMacros.h"	// Potrzebujemy tego nag³ówka aby móc u¿ywaæ makr debuguj¹cych
#include "Kismet/KismetSystemLibrary.h"

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
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
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

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit(const FVector& ImpactPoint)	// Deklarujemy funkcjê GetHit z Enemy.h
{
	DRAW_SPHRE_COLOR(ImpactPoint, FColor::Orange);	// Rysujemy kulkê w kolorze pomarañczowym gdy uderzymy mieczem w "enemy"
	PlayHitReactMontage(FName("FromLeft"));	// Odtwarzamy animacjê otrzymania ciosu

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
	// Print the angle to the screen
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta));
	}
	// Draw a debug arrow to show the forward direction
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	// Draw a debug arrow from the enemy's location to the hit location
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
}

