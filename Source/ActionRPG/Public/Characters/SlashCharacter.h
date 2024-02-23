// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;

//Allows us to use the enum in blueprint
UENUM(BlueprintType)
//Enum to keep track of the character state
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped")/*UMETA makro pozwala zmieniæ wyœwietlan¹ nazwê w blueprintach */,
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "EquippedOneHandedWeapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "EquippedTwoHandedWeapon"),
};
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

public:
	//Function to pickup items
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }


};
