// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"	//Musimy doda� plik nag��wkowy z naszym bohaterem aby m�c podnie�� bro�
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"	//Potrzebujemy tego nag��wka aby m�c u�ywa� funkcji z interfejsu HitInterface
#include "NiagaraComponent.h"	//Potrzebujemy tego nag��wka aby m�c u�ywa� efekt�w Niagara
 

AWeapon::AWeapon() 
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));	//Tworzymy nowy BoxComponent
	WeaponBox->SetupAttachment(GetRootComponent());	//Przypisujemy go do naszego g��wnego komponentu
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);	//Ustawiamy kolizj� na NoCollision
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);	//Sprawdza wszysktie checkboxy w collision presets czy overlapuj�
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);	//Ignoruje kolizj� z naszym bohaterem

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));	//Tworzymy nowy komponent
	BoxTraceStart->SetupAttachment(GetRootComponent());	//Przypisujemy go do naszego g��wnego komponentu
	
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));	//Tworzymy nowy komponent
	BoxTraceEnd->SetupAttachment(GetRootComponent());	//Przypisujemy go do naszego g��wnego komponentu
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	//Dodajemy delegata do naszej funkcji 
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

//Funkcja do podniesienia broni
void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	//Zmieniamy stan broni na EIS_Equipped po podniesieniu
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);	//Ustawiamy nowego w�a�ciciela
	SetInstigator(NewInstigator);	//Ustawiamy nowego instigatora
	AttachMeshToSocket(InParent, InSocketName);	//Doczepiamy bro� do socketu
	DisableSphereCollision();
	PlayEquipSound();
	DeactivateEmbers();
}
void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)	//Sprawdzamy czy emberseffect is not nullptr
	{
		ItemEffect->Deactivate();	//Deaktywujemy efekt
	}
}
void AWeapon::DisableSphereCollision()
{
	if (Sphere)
	{	//Teraz b�dziemy chcieli wy��czy� kolizj� naszej broni z naszym bohaterem
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}
//Funkcja do "doczepienia" broni do odpowiedniego socketu lub stworzonego nowego socketu
void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	//Tworzymy zasady "doczepienia" broni do wybranego socketu w r�ce
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

/*void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);
	
	/*Maj�c powy�sz� funkcj� void AWeapon::Equip(USceneComponent* InParent, FName InSocketName) mog� usun�� poni�sze castowanie i if statement
	//Sprawdzamy czy to jest nasz bohater, je�li tak to podno�my bro� i doczepiomy do socketu w d�oni, kt�ry stworzylismy
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter)
	{
		//Zasady przyczepienia broni do socketu d�oni
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
		ItemMesh->AttachToComponent(SlashCharacter->GetMesh(), TransformRules, FName("RightHandSocket"//Podajemy nazw� socketu do kt�ego ma zostac doczepiona bro�));
	}
	
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
*/

//Delegate function for box overlap
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor))	//Sprawdzamy czy aktor jest tego samego typu
	{
		return;
	}

	FHitResult BoxHit;	//Tworzymy zmienn� przechowuj�c� informacje o trafieniu
	BoxTrace(BoxHit);

	if (BoxHit.GetActor())	//Sprawdzamy czy trafili�my w "aktora"
	{
		if (ActorIsSameType(BoxHit.GetActor()))	//Sprawdzamy czy aktor jest tego samego typu
		{
			return;
		}

		UGameplayStatics::ApplyDamage(
			BoxHit.GetActor(),	//Zadajmy obra�enia trafionemu aktorowi
			Damage,	//Obra�enia
			GetInstigator()->GetController(),	//Instigator
			this,		//Obiekt, kt�ry zadaje obra�enia
			UDamageType::StaticClass()	//Typ obra�e�
		);	//Zadajemy obra�enia trafionemu aktorowi

		ExecuteGetHit(BoxHit);	//Wywo�ujemy funkcj� GetHit z interfejsu HitInterface
		CreateFields(BoxHit.ImpactPoint);	//Tworzymy pole "zniszczenia" na podstawie punktu trafienia
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());	//Castujemy trafionego aktora do interfejsu HitInterface
	if (HitInterface)	//Sprawdzamy czy trafiony aktor ma interfejs HitInterface
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());	//Wywo�ujemy funkcj� GetHit z interfejsu HitInterface native
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	//Pobieramy lokalizacj� naszego BoxTraceStart. Pami�taj, �e jest to lokalizacja w przestrzeni �wiata
	const FVector Start = BoxTraceStart->GetComponentLocation();
	//Pobieramy lokalizacj� naszego BoxTraceEnd. Pami�taj, �e jest to lokalizacja w przestrzeni �wiata
	const FVector End = BoxTraceEnd->GetComponentLocation();

	//Tworzymy tablic� aktor�w do ignorowania
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);	//Dodajemy nasz obiekt do tablicy aktor�w do ignorowania. Dzi�ki temu nie b�dziemy sprawdza� kolizji z nasz� postaci�
	ActorsToIgnore.Add(GetOwner());	//Dodajemy w�a�ciciela do tablicy aktor�w do ignorowania. Dzi�ki temu nie b�dziemy sprawdza� kolizji z w�a�cicielem

	for (AActor* Actor : IgnoreActors)	//Iterujemy przez tablic� IgnoreActors
	{
		ActorsToIgnore.AddUnique(Actor);	//Dodajemy aktor�w do tablicy ActorsToIgnore. Dodajemy AddUnique aby�my nie dodali tego samego "aktora" dwa razy.
	}

	//funkcja pozwala na sprawdzenie, czy w danej przestrzeni (okre�lonej przez pude�ko) znajduj� si� obiekty, oraz zbieranie informacji na temat tych obiekt�w, takich jak ich lokalizacja czy w�a�ciwo�ci
	UKismetSystemLibrary::BoxTraceSingle(
		this,	//Obiekt wywo�uj�cy
		Start,	//Pocz�tek box trace
		End,	//Koniec box trace
		BoxTraceExtent,	//Rozmiar box trace
		BoxTraceStart->GetComponentRotation(),	//Rotacja box trace
		ETraceTypeQuery::TraceTypeQuery1,	//Typ trace
		false,	//Ignoruj w�asny obiekt
		ActorsToIgnore,	//Tablica aktor�w do ignorowania
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,	//Rysuj debug trace po zmianie na ForDuration
		BoxHit,	//Zmienna przechowuj�ca informacje o trafieniu
		true	//Ignoruj kolizje
	);
	IgnoreActors.AddUnique(BoxHit.GetActor());	//Dodajemy trafionego aktora do tablicy IgnoreActors
}
