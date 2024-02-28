// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
/**
 * 
 */
UCLASS()
class ACTIONRPG_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	void Equip(USceneComponent* InParent, FName InSocketName);
	//Funkcja do "doczepienia" broni do odpowiedniego socketu lub stworzonego nowego socketu
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);
protected:
	//Delegate function for sphere overlap
	//UFUNCTION nie mo¿e byæ w klasie pochodnej. Dostaniemy b³êdy w kompilacji 
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	//Delegate funtion for sphere endoverlap
	//UFUNCTION nie mo¿e byæ w klasie pochodnej. Dostaniemy b³êdy w kompilacji
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	//For the sound of the equip weapon
	USoundBase* EquipSound;
};
