// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

//Dodanie statusu przedmiotu
enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped,
};

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")	// (EditDefaultsOnly)I can change Amplitude in the blueprint BP_Item only
	float Amplitude = 0.25f;	// I can assigned Amplitude here
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")	// (EditInstanceOnly) I can change TimeConstant in the Instances in the game only
	float TimeConstant = 5.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	//float RotationRate = 90.f;


	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	template<typename T>
	T Avg(T First, T Second);

	//Delegate function for sphere overlap
	UFUNCTION() //Without UFUNCTION() it will not bind
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Delegate funtion for sphere endoverlap
	UFUNCTION()	//Without UFUNCTION() it will not bind
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ItemMesh;
	//Enum to keep track of the item state
	EItemState ItemState = EItemState::EIS_Hovering;

	//Sphere component for overlaping
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))	// meta = (AllowPrivateAccess = "true") allows to see and change variables in private section
	float RunningTime;		// Time for sine wave
};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}
