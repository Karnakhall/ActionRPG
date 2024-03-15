// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetHealthPercent(float Percent);	//Funkcja do ustawiania procentu øycia
private:
	UPROPERTY()
	class UHealthBar* HealthBarWidget;	//Wskaünik do widgetu paska øycia
};
