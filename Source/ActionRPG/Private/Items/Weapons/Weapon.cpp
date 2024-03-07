// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"	//Musimy dodaæ plik nag³ówkowy z naszym bohaterem aby móc podnieœæ broñ
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"	//Potrzebujemy tego nag³ówka aby móc u¿ywaæ funkcji z interfejsu HitInterface
 

AWeapon::AWeapon() 
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));	//Tworzymy nowy BoxComponent
	WeaponBox->SetupAttachment(GetRootComponent());	//Przypisujemy go do naszego g³ównego komponentu
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);	//Ustawiamy kolizjê na NoCollision
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);	//Sprawdza wszysktie checkboxy w collision presets czy overlapuj¹
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);	//Ignoruje kolizjê z naszym bohaterem

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));	//Tworzymy nowy komponent
	BoxTraceStart->SetupAttachment(GetRootComponent());	//Przypisujemy go do naszego g³ównego komponentu
	
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));	//Tworzymy nowy komponent
	BoxTraceEnd->SetupAttachment(GetRootComponent());	//Przypisujemy go do naszego g³ównego komponentu
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	//Dodajemy delegata do naszej funkcji 
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

//Funkcja do podniesienia broni
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

//Delegate function for box overlap
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Pobieramy lokalizacjê naszego BoxTraceStart. Pamiêtaj, ¿e jest to lokalizacja w przestrzeni œwiata
	const FVector Start = BoxTraceStart->GetComponentLocation();
	//Pobieramy lokalizacjê naszego BoxTraceEnd. Pamiêtaj, ¿e jest to lokalizacja w przestrzeni œwiata
	const FVector End = BoxTraceEnd->GetComponentLocation();
	
	//Tworzymy tablicê aktorów do ignorowania
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);	//Dodajemy nasz obiekt do tablicy aktorów do ignorowania. Dziêki temu nie bêdziemy sprawdzaæ kolizji z nasz¹ postaci¹
	
	for (AActor* Actor : IgnoreActors)	//Iterujemy przez tablicê IgnoreActors
	{
		ActorsToIgnore.AddUnique(Actor);	//Dodajemy aktorów do tablicy ActorsToIgnore. Dodajemy AddUnique abyœmy nie dodali tego samego "aktora" dwa razy.
	}
	
	FHitResult BoxHit;//Tworzymy lokaln¹ zmienn¹, która bêdzie przechowywaæ informacje o FHITResult
	//funkcja pozwala na sprawdzenie, czy w danej przestrzeni (okreœlonej przez pude³ko) znajduj¹ siê obiekty, oraz zbieranie informacji na temat tych obiektów, takich jak ich lokalizacja czy w³aœciwoœci
	UKismetSystemLibrary::BoxTraceSingle(
		this,	//Obiekt wywo³uj¹cy
		Start,	//Pocz¹tek box trace
		End,	//Koniec box trace
		FVector(5.f, 5.f, 5.f),	//Rozmiar box trace
		BoxTraceStart->GetComponentRotation(),	//Rotacja box trace
		ETraceTypeQuery::TraceTypeQuery1,	//Typ trace
		false,	//Ignoruj w³asny obiekt
		ActorsToIgnore,	//Tablica aktorów do ignorowania
		EDrawDebugTrace::ForDuration,	//Rysuj debug trace
		BoxHit,	//Zmienna przechowuj¹ca informacje o trafieniu
		true	//Ignoruj kolizje
		);
	if (BoxHit.GetActor())	//Sprawdzamy czy trafiliœmy w "aktora"
	{
		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());	//Castujemy trafionego aktora do interfejsu HitInterface
		if (HitInterface)	//Sprawdzamy czy trafiony aktor ma interfejs HitInterface
		{
			HitInterface->GetHit(BoxHit.ImpactPoint);	//Wywo³ujemy funkcjê GetHit z interfejsu HitInterface
		}
		IgnoreActors.AddUnique(BoxHit.GetActor());	//Dodajemy trafionego aktora do tablicy IgnoreActors
	}
}
