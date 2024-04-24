// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "ActionRPG/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Characters/SlashCharacter.h"	//Musimy dodaæ plik nag³ówkowy z naszym bohaterem aby móc podnieœæ broñ
#include "NiagaraComponent.h"





// Sets default values
AItem::AItem() // I can asigned here Amplitude(0.25f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	// I can assigned amplitude here Amplitude = 0.25f;


	//Added sphere component
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EmbersEffect"));	// Create the NiagaraComponent
	ItemEffect->SetupAttachment(GetRootComponent());	// Attach the NiagaraComponent to the root component
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	//Binding the callback to the delegate
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);

	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
	// Moja funkcja
	/*int32 AvgInt = Avg<int32>(1, 3);
	UE_LOG(LogTemp, Warning, TEXT("Avg of 1 and 3: %d"), AvgInt);

	float AvgFloat = Avg<float>(3.45f, 7.86f);
	UE_LOG(LogTemp, Warning, TEXT("Avg of 3.45 and 7.86: %f"), AvgFloat);*/

	/*UE_LOG(LogTemp, Warning, TEXT("Siema"));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Green, FString("Siema!"));
	}*/

	// UWorld* World = GetWorld();

	// SetActorLocation(FVector(0.f, 0.f, 100.f));
	// SetActorRotation(FRotator(0.f, 45.f, 0.f));
	
	// FVector Location = GetActorLocation();
	// FVector Forward = GetActorForwardVector();
	
	// DRAW_SPHERE(Location);
	//DRAW_LINE(Location, Location + Forward * 100.f);
	//DRAW_POINT(Location + Forward * 100.f);
	//zamiast dwóch powy¿szych linijek makro, poni¿ej zosta³a stworzona jedna linijka makro któa zawiera dwie powy¿sze instrukcje w sobie
	// DRAW_VECTOR(Location, Location + Forward * 100.f);
}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}
//Delagate function
void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/* Poni¿szy kod sprwdza czy actor overlappuje z naszym przedmiotem i wyœwietlamy informacjê na ekranie
	//Taking name of the object that overlapped with the sphere
	const FString OtherActorName = OtherActor->GetName();
	//show messege on engine screen
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 30.f, FColor::Red, OtherActorName);
	}
	*/
	
	//Sprawdzamy czy bohater overlappuje z naszym przedmiotem, jesli tak, mo¿emy podnieœæ przedmiot
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(this);
	}
}
//Delegate function end overlaping actor
void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*
	//Taking name of the object that end overlapped with the sphere
	const FString OtherActorName = //We can add some temporary string object FString("Ending Overlap with: ") + OtherActor->GetName();
	//Show messege on engine screen
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Emerald, OtherActorName);
	} 
	*/
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(nullptr);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RunningTime += DeltaTime;
	
	if (ItemState == EItemState::EIS_Hovering)	//Jeœli nasz przedmiot jest w stanie Hovering, to unosimy go i opuszczamy
	{
		//Hovering item
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
	

	/*Poruszanie aktorem(sfer¹), razem z pokazaniem vektora ruchu // Movement rate in units of cm/s
	float MovementRate = 50.f;
	float RotationRate = 45.f;

	MovementRate * DeltaTime (cm/s) * (s/frame) = (cm/frame)
	AddActorWorldOffset(FVector(MovementRate * DeltaTime, 0.f, 0.f));			// Przesuwamy "aktora" o 50 w osi X, bez wzglêdu na to ile FPS ma dany komputer
	AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
	*/

	//RunningTime += DeltaTime; - This is the same RunningTime = RunningTime + DeltaTime

	//AddActorWorldRotation(FRotator(0.f, 100.f * DeltaTime, 0.f));	// Actor rotator - Section 6 Challenge

	//float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);		// period = 2*pi/K  funkcja powoduj¹ca unoszenie siê i opadanie danego obiektu

	//AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

	/*DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);
	
	FVector AvgVector = Avg<FVector>(GetActorLocation(), FVector::ZeroVector);
	DRAW_POINT_SingleFrame(AvgVector);*/

	// FRotator AvgRotator = Avg<FRotator>(GetActorRotation(), FRotator::ZeroRotator);  W tym przypadku nasza funkcja nie dzia³a poniewa¿ w odniesieniu nie ma wbudowanego dzielenia, wiêc nie mo¿emy go wykorzystac w tym przypadku
	
	/*Informacje wyœwietlacj¹ce w output log oraz na ekranie
	UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), DeltaTime);

	if (GEngine)
	{
		FString Name = GetName();
		FString Messege = FString::Printf(TEXT("Item Name: %s"), *Name);
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Green, Messege);

		UE_LOG(LogTemp, Warning, TEXT("Item Name: %s"), *Name);
	}*/
}

