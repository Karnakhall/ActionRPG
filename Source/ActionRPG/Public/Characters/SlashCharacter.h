// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"	//Potrzebujemy tego nag³ówka aby SlachCharacter móg³ dziedziczyæ funkcje z BaseCharacter
#include "CharacterTypes.h"	//Potrzebujemy tego nag³ówka aby móc u¿yæ enuma
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;

UCLASS()
class ACTIONRPG_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASlashCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	/*Jeden ze sposobów w³¹czenia i wy³¹czenia kolizji na broni
	UFUNCTION(BlueprintCallable)	//Dziêki temu mo¿emy wywo³aæ t¹ funkcjê z blueprintu
	void EnableWeaponCollision();	//Funkcja do w³¹czenia kolizji broni

	UFUNCTION(BlueprintCallable)	//Dziêki temu mo¿emy wywo³aæ t¹ funkcjê z blueprintu
	void DisableWeaponCollision();	//Funkcja do wy³¹czenia kolizji broni
	*/
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	* Callbacks for input
	*/
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
	virtual void Attack() override;

	/**
	* Play Montage functions
	*/

	//Function to play attack montage
	virtual void PlayAttackMontage() override;

	
	//UFUNCTION(BlueprintCallable) nie potrzebujemy tego tutaj, poniewa¿ dziedziczymy ca³oœæ z Base Character
	//Function to stop attack montage
	virtual void AttackEnd() override;
	//Sprawdzamy boolem czy postac mo¿e zaatakowaæ
	virtual bool CanAttack() override;

	//Function to play equip montage. We don't need to select a section random. We want to chose whether to play the equip or unequip section
	void PlayEquipMontage(const FName& SectionName);
	//Sprawdzamy boolem czy postaæ mo¿e uzyæ animacji equip lub unequip
	bool CanDisarm();
	//Sprawdzamy boolem czy postaæ mo¿e za³o¿yæ broñ
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	//Disarm function for attaching the weapon to the back
	void Disarm();

	UFUNCTION(BlueprintCallable)
	//Arm function for attaching the weapon to the hand
	void Arm();

	UFUNCTION(BlueprintCallable)
	//Function to know where is the end of equipping
	void FinishEquipping();
private:
	/* Rozwi¹zanie dla zwyk³ej wersji c++
	//Variable to keep track of the character state
	CharacterState State = Unequipped;
	*/

	//Variable to keep track of the character state
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	//Variable to keep track of the action state
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true")/*Dodajemy AllowPrivateAccess aby mieæ dostêp do statusu z prywatnej sekcji*/)
	EActionState ActionState = EActionState::EAS_Unoccupied;

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

	
	//Montage to equip and unequipped the weapon
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;
public:
	//Function to pickup items
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	//Function to get the character state and make it public. Const nie pozwala na zmiane niczego w funkcji poza zmian¹ state
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
