// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"	//Musimy dodaæ plik nag³ówkowy z naszym bohaterem aby móc podnieœæ broñ
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"


AWeapon::AWeapon() 
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));	//Tworzymy nowy BoxComponent
	WeaponBox->SetupAttachment(GetRootComponent());	//Przypisujemy go do naszego g³ównego komponentu
}
void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
	AttachMeshToSocket(InParent, InSocketName);
	//Zmieniamy stan broni na EIS_Equipped po podniesieniu
	ItemState = EItemState::EIS_Equipped;
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
	if (Sphere)
	{	//Teraz bêdziemy chcieli wy³¹czyæ kolizjê naszej broni z naszym bohaterem
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	}
}
//Funkcja do "doczepienia" broni do odpowiedniego socketu lub stworzonego nowego socketu
void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	//Tworzymy zasady "doczepienia" broni do wybranego socketu w rêce
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);
	
	/*Maj¹c powy¿sz¹ funkcjê void AWeapon::Equip(USceneComponent* InParent, FName InSocketName) mogê usun¹æ poni¿sze castowanie i if statement
	//Sprawdzamy czy to jest nasz bohater, jeœli tak to podnoœmy broñ i doczepiomy do socketu w d³oni, który stworzylismy
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter)
	{
		//Zasady przyczepienia broni do socketu d³oni
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
		ItemMesh->AttachToComponent(SlashCharacter->GetMesh(), TransformRules, FName("RightHandSocket"//Podajemy nazwê socketu do któego ma zostac doczepiona broñ));
	}
	*/
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
