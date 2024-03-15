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
	
public:	// Getters and setters
	void ReceiveDamage(float Damage);	//Funkcja do otrzymywania obra¿eñ
	float GetHealthPercent();	//Funkcja do pobierania ¿ycia
};
