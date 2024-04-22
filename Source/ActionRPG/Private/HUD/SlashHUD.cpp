// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();	// Get the first player controller
		if (Controller && SlashOverlayClass)	// If the Controller and SlashOverlayClass are valid
		{
			USlashOverlay* SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass);	// Create the SlashOverlay widget
			SlashOverlay->AddToViewport();	// Add the SlashOverlay widget to the viewport
		}
	}
}
