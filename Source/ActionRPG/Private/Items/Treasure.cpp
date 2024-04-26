// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/SlashCharacter.h"
#include "Components/SphereComponent.h"


void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Sprawdzamy czy bohater overlappuje z naszym przedmiotem, jesli tak, mo¿emy podnieœæ przedmiot
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter)
	{
		SpawnPickupSound();	//Spawn the pickup sound
		
		Destroy();	//Jeœli bohater overlappuje z naszym przedmiotem, to niszczymy go
	}
}
