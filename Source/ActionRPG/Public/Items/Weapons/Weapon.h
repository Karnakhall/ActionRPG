// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API AWeapon : public AItem
{
	GENERATED_BODY()
protected:
	//Delegate function for sphere overlap
	//UFUNCTION nie mo¿e byæ w klasie pochodnej. Dostaniemy b³êdy w kompilacji 
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	//Delegate funtion for sphere endoverlap
	//UFUNCTION nie mo¿e byæ w klasie pochodnej. Dostaniemy b³êdy w kompilacji
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
