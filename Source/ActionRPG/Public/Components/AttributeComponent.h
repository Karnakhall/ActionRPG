// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);	//Funkcja do regeneracji staminy
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Current health of the actor
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;
	// Maximum health of the actor
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;	

	// Current stamina of the actor
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;
	// Maximum stamina of the actor
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;	//Zmienna do przechowywania z³ota

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;	//Zmienna do przechowywania dusz

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;	//Regeneracja staminy na sekundê
	
public:	// Getters and setters
	void ReceiveDamage(float Damage);	//Funkcja do otrzymywania obra¿eñ
	void UseStamina(float StaminaCost);	//Funkcja do otrzymywania staminy
	float GetHealthPercent();	//Funkcja do pobierania ¿ycia
	float GetStaminaPercent();	//Funkcja do pobierania staminy
	bool IsAlive();	//Funkcja do sprawdzania czy aktor ¿yje
	void AddGold(int32 AmountOfGold);	//Funkcja do dodawania z³ota
	void AddSouls(int32 NumberOfSouls);	//Funkcja do dodawania dusz


	FORCEINLINE int32 GetGold() const { return Gold; }		//Getter do z³ota
	FORCEINLINE int32 GetSouls() const { return Souls; }	//Getter do z³ota
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }	//Getter do kosztu uniku
	FORCEINLINE float GetStamina() const { return Stamina; }	//Getter do staminy

};
