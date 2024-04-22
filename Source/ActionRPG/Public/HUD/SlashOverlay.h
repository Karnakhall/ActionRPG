// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetHealthBarPercent(float Percent);	// This function is used to set the HealthProgressBar's Percent variable
	void SetStaminaBarPercent(float Percent);	// This function is used to set the StaminaProgressBar's Percent variable
	void SetGoldText(int32 Gold);	// This function is used to set the GoldText's Text variable
	void SetSoulsText(int32 Souls);	// This function is used to set the SoulsText's Text variable

private:
	UPROPERTY(meta = (BindWidget))	// This macro is used to bind the HealthProgressBar widget to the HealthProgressBar variable
	class UProgressBar* HealthProgressBar;	// This variable is used to store the HealthProgressBar widget

	UPROPERTY(meta = (BindWidget))	// This macro is used to bind the StaminaProgressBar widget to the StaminaProgressBar variable
	class UProgressBar* StaminaProgressBar;	// This variable is used to store the StaminaProgressBar widget

	UPROPERTY(meta = (BindWidget))	// This macro is used to bind the StaminaProgressBar widget to the StaminaProgressBar variable
	class UTextBlock* GoldText;	// This variable is used to store the GoldText widget

	UPROPERTY(meta = (BindWidget))	// This macro is used to bind the StaminaProgressBar widget to the StaminaProgressBar variable
	class UTextBlock* SoulsText;	// This variable is used to store the SoulsText widget
};
