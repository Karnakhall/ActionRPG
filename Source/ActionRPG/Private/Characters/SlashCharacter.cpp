// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"		//Potrzebujemy tego nag³ówka aby móc podnieœæ broñ
#include "Items/Weapons/Weapon.h"	//Potrzebujemy tego nag³ówka aby móc podnieœæ broñ
#include "Animation/AnimMontage.h"	//Potrzebujemy tego nag³ówka aby móc u¿yæ AnimMontage

// Sets default values
ASlashCharacter::ASlashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

//Set this character to be controlled by the lowest-numbered player
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

//Set the character to move in the direction it is facing
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUp);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &ASlashCharacter::EKeyPresed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);
}

void ASlashCharacter::MoveForward(float Value)
{
	if (Controller && (Value != 0.f))
	{
		//Moving only in one direction
		/*FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);
		*/
		//find out which way is forward
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::MoveRight(float Value)
{
	if (Controller && (Value != 0.f))
	{
		//Moving only in the left and right	direction
		/*FVector Right = GetActorRightVector();
		AddMovementInput(Right, Value);
		*/

		//find out which way is right
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

	}
}

void ASlashCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ASlashCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ASlashCharacter::EKeyPresed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		//Jeœli klikniemy przycisk E, to podnosimy broñ i doczepiamy do socketu w d³oni
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	}
}
//Funkcja ataku
void ASlashCharacter::Attack()
{
	//Sprawdzamy action state postaci
	if (ActionState == EActionState::EAS_Unoccupied)
	{
		//Wywo³ujemy funkcjê odtwarzaj¹c¹ animacjê ataku
		PlayAttackMontage();
		//Jeœli postaæ jest w stanie unoccupied, to zmieniamy jej stan na occupied
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	//Sprawdzamy czy to nie jest nullpointer
	if (AnimInstance && AttackMontage)	//Jeœli AnimInstance i AttackMontage nie s¹ nullpointerami, to odtwarzamy animacjê ataku
	{
		AnimInstance->Montage_Play(AttackMontage);
		//Mamy dwie sekcje w animacji ataku, wiêc losujemy która z nich zostanie odtworzona, wiêc dodajemy liczbê losow¹ 0 albo 1
		const int32 Selection = FMath::RandRange(0, 1);	//Trochê jak rzut monet¹, generuje nam 0 albo 1
		//Tworzymy zmienn¹, która bêdzie przechowywaæ nazwê sekcji animacji - pozostawiamy j¹ pust¹ poniewa¿ sekcja zostanie wybrana przez switch.
		FName SectionName = FName();
		//Wybieramy sekcjê animacji ataku i zmieniamy siê pomiêdzy nimi
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			//Break jest potrzebny, ¿eby wyjœæ z pêtli switch
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		default:
			break;
		}
		//Po wyborze sekcji animacji, odtwarzamy j¹
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}
//Funkcja koñcz¹ca atak
void ASlashCharacter::AttackEnd()
{
	//Jeœli postaæ jest w stanie ataku, to zmieniamy jej stan na unoccupied w momencie gdy animacja ataku siê skoñczy
	ActionState = EActionState::EAS_Unoccupied;
}
