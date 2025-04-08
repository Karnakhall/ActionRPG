// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAttributeComponent::ReceiveDamage(float Damage)	//Funkcja do otrzymywania obra�e�
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);	//Odejmujemy obra�enia od �ycia, ale nie jest mniejsze ni� 0 albo wi�ksze ni� maks. �ycie
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);	//Odejmujemy koszt staminy od staminy, ale nie jest mniejsze ni� 0 albo wi�ksze ni� maks. stamina
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;	//Zwracamy warto�� �ycia w procentach
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;	//Zwracamy warto�� staminy w procentach
}

bool UAttributeComponent::IsAlive()	//Funkcja do sprawdzania czy aktor �yje
{
	return Health > 0.f;	//Je�li �ycie jest wi�ksze ni� 0, to aktor �yje
}

void UAttributeComponent::AddGold(int32 AmountOfGold)
{
	Gold += AmountOfGold;	//Dodajemy z�oto
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls)
{
	Souls += NumberOfSouls;	//Dodajemy dusze
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);	//Powolna regeneracja staminy
}

