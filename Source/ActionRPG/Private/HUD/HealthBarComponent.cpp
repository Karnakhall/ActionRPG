// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	
	if (HealthBarWidget == nullptr)	//Jeøeli wskaünik do widgetu jest nullptr
	{
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());	//Pobieramy wskaünik do widgetu paska øycia
	}

	if (HealthBarWidget && HealthBarWidget->HealthBar)	//Jeøeli wskaünik do widgetu paska øycia oraz wskaünik do paska øycia sπ poprawne
	{
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}
