// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;	//It's need for create a box collision
/**
 * 
 */
UCLASS()
class ACTIONRPG_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();	//Constructor
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	//Funkcja do "doczepienia" broni do odpowiedniego socketu lub stworzonego nowego socketu
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;	//Array of actors to ignore
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Delegate function for sphere overlap
	//UFUNCTION nie mo¿e byæ w klasie pochodnej. Dostaniemy b³êdy w kompilacji. Mamy UFUNCTION z funkcji z której dziedziczymy
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	//Delegate funtion for sphere endoverlap
	//UFUNCTION nie mo¿e byæ w klasie pochodnej. Dostaniemy b³êdy w kompilacji
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	//Delegate function for box overlap
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);	//Function to create fields
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	//For the sound of the equip weapon
	USoundBase* EquipSound;
	
	//Sphere collision for the weapon
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	//For the sphere collision
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	//For the sphere collision
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;	//Damage of the weapon

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }	//Getter for the weapon box
};
