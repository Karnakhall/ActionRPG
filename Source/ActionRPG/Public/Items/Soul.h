// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API ASoul : public AItem
{
	GENERATED_BODY()
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
private:
	
	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 Souls;
public:
	FORCEINLINE int32 GetSouls() const { return Souls; }	//Getter do dusz
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }	//Setter do dusz
};
