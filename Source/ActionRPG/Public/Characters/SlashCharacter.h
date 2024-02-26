// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"	//Potrzebujemy tego nag³ówka aby móc u¿yæ enuma
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;

UCLASS()
class ACTIONRPG_API ASlashCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASlashCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Moving the character forward and backward
	void MoveForward(float Value);
	//Moving the character right and left
	void MoveRight(float Value);
	//Turning the character
	void Turn(float Value);
	//Looking up and down
	void LookUp(float Value);
	//Funtcion to equip
	void EKeyPresed();
	//Function to attack
	void Attack();
private:
	/* Rozwi¹zanie dla zwyk³ej wersji c++
	//Variable to keep track of the character state
	CharacterState State = Unequipped;
	*/

	//Variable to keep track of the character state
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	
	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;

	//Item that is currently overlapping and we can try to pickup
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	/**
	*Animation montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;
public:
	//Function to pickup items
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	//Function to get the character state and make it public. Const nie pozwala na zmiane niczego w funkcji poza zmian¹ state
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
