// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercent(float Percent)	
{
	if (HealthProgressBar)	// Check if the HealthProgressBar variable is valid
	{
		HealthProgressBar->SetPercent(Percent);	// Set the HealthProgressBar's Percent variable
	}
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)	// Check if the StaminaProgressBar variable is valid
	{
		StaminaProgressBar->SetPercent(Percent);	// Set the StaminaProgressBar's Percent variable
	}
}

void USlashOverlay::SetGoldText(int32 Gold)
{
	if (GoldText)	// Check if the GoldText variable is valid
	{
		const FString String = FString::Printf(TEXT("%d"), Gold);	// Convert the Gold variable to a string
		const FText Text = FText::FromString(String);	// Convert the string to a FText variable
		GoldText->SetText(Text);	// Set the GoldText's Text variable
	}
}

void USlashOverlay::SetSoulsText(int32 Souls)
{
	if (SoulsText)	// Check if the GoldText variable is valid
	{
		const FString String = FString::Printf(TEXT("%d"), Souls);	// Convert the Gold variable to a string
		const FText Text = FText::FromString(String);	// Convert the string to a FText variable
		SoulsText->SetText(Text);	// Set the GoldText's Text variable
	}
}
