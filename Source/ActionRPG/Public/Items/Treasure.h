// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API ATreasure : public AItem
{
	GENERATED_BODY()
protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;	//Override the OnSphereOverlap function from the base class

private:

	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 Gold;	//Ilo�� z�ota, jak� otrzymamy po podniesieniu przedmiotu

public:
	FORCEINLINE int32 GetGold() const { return Gold; }	//Getter do z�ota
};
