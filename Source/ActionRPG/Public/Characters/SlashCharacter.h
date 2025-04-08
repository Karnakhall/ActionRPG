// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"	//Potrzebujemy tego nag��wka aby SlachCharacter m�g� dziedziczy� funkcje z BaseCharacter
#include "CharacterTypes.h"	//Potrzebujemy tego nag��wka aby m�c u�y� enuma
#include "Interfaces/PickupInterface.h"	//Potrzebujemy tego nag��wka aby m�c u�y� interfejsu do podnoszenia przedmiot�w
#include "InputActionValue.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class ASoul;
class ATreasure;
class UAnimMontage;
class USlashOverlay;
class UInputMappingContext;
class UInputAction;


UCLASS()
class ACTIONRPG_API ASlashCharacter : public ABaseCharacter, public IPickupInterface	//Dodajemy interfejs do podnoszenia przedmiot�w
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;	//Funkcja do otrzymywania obra�e�
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;

	/*Jeden ze sposob�w w��czenia i wy��czenia kolizji na broni
	UFUNCTION(BlueprintCallable)	//Dzi�ki temu mo�emy wywo�a� t� funkcj� z blueprintu
	void EnableWeaponCollision();	//Funkcja do w��czenia kolizji broni

	UFUNCTION(BlueprintCallable)	//Dzi�ki temu mo�emy wywo�a� t� funkcj� z blueprintu
	void DisableWeaponCollision();	//Funkcja do wy��czenia kolizji broni
	*/
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* SlashContext;	//Context for input mapping

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EKeyAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DodgeAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	/** Callbacks for input	*/
	//Moving the character forward and backward
	void MoveForward(float Value);
	//Moving the character right and left
	void MoveRight(float Value);
	//Turning the character
	void Turn(float Value);
	//Looking up and down
	void LookUp(float Value);
	//Funtcion to equip
	void EKeyPressed();
	//Function to attack
	virtual void Attack() override;
	//Function to dodge
	void Dodge();


	/** Combat */
	//UFUNCTION(BlueprintCallable) nie potrzebujemy tego tutaj, poniewa� dziedziczymy ca�o�� z Base Character
	
	void EquipWeapon(AWeapon* Weapon);
	//Function to stop attack montage
	virtual void AttackEnd() override;
	//Function to stop dodge montage
	virtual void DodgeEnd() override;
	//Sprawdzamy boolem czy postac mo�e zaatakowa�
	virtual bool CanAttack() override;

	//Sprawdzamy boolem czy posta� mo�e uzy� animacji equip lub unequip
	bool CanDisarm();
	//Sprawdzamy boolem czy posta� mo�e za�o�y� bro�
	bool CanArm();
	void Disarm();
	void Arm();
	//Function to play equip montage. We don't need to select a section random. We want to chose whether to play the equip or unequip section
	void PlayEquipMontage(const FName& SectionName);
	virtual void Die_Implementation() override;
	bool HasEnoughStamina();
	bool IsOccupied();

	UFUNCTION(BlueprintCallable)
	//Disarm function for attaching the weapon to the back
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	//Arm function for attaching the weapon to the hand
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	//Function to know where is the end of equipping
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

private:

	bool IsUnoccupied();	//Function to check if the character is unoccupied

	void InitializeSlashOverlay();	//Function to initialize the slash overlay

	void SetHUDHealth();	//Function to update health on HUD



	/* Rozwi�zanie dla zwyk�ej wersji c++
	//Variable to keep track of the character state
	CharacterState State = Unequipped;
	*/

	//Variable to keep track of the character state
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	//Variable to keep track of the action state
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true")/*Dodajemy AllowPrivateAccess aby mie� dost�p do statusu z prywatnej sekcji*/)
	EActionState ActionState = EActionState::EAS_Unoccupied;

	/** Character components */
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

	UPROPERTY()
	USlashOverlay* SlashOverlay;

public:

	//Function to get the character state and make it public. Const nie pozwala na zmiane niczego w funkcji poza zmian� state
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	//Function to get the action state and make it public
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
