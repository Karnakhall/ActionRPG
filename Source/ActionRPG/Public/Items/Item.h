// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class ACTIONRPG_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	float RunningTime;

	UPROPERTY(EditAnywhere)	// (EditDefaultsOnly)I can change Amplitude in the blueprint BP_Item only
	float Amplitude = 0.25f;	// I can assigned Amplitude here
	
	UPROPERTY(EditAnywhere)	// (EditInstanceOnly) I can change TimeConstant in the Instances in the game only
	float TimeConstant = 5.f;

};
