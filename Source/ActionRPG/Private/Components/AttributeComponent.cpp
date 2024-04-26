// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAttributeComponent::ReceiveDamage(float Damage)	//Funkcja do otrzymywania obra¿eñ
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);	//Odejmujemy obra¿enia od ¿ycia, ale nie jest mniejsze ni¿ 0 albo wiêksze ni¿ maks. ¿ycie
}

float UAttributeComponent::GetHealthPercent()
{
	return Health/MaxHealth;	//Zwracamy wartoœæ ¿ycia w procentach
}

bool UAttributeComponent::IsAlive()	//Funkcja do sprawdzania czy aktor ¿yje
{
	return Health > 0.f;	//Jeœli ¿ycie jest wiêksze ni¿ 0, to aktor ¿yje
}

void UAttributeComponent::AddGold(int32 AmountOfGold)
{
	Gold += AmountOfGold;	//Dodajemy z³oto
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls)
{
	Souls += NumberOfSouls;	//Dodajemy dusze
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

