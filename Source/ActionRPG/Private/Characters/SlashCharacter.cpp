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
	//Jeœli postaæ jest w stanie ataku, to nie mo¿e siê poruszaæ
	if (ActionState == EActionState::EAS_Attacking) return;
	//Jeœli postaæ nie atakuje, to mo¿e siê poruszaæ
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
	//Jeœli postaæ jest w stanie ataku, to nie mo¿e siê poruszaæ
	if (ActionState == EActionState::EAS_Attacking) return;
	//Jeœli postaæ nie atakuje, to mo¿e siê poruszaæ
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
//Funkcja podnoszenia broni po naciœnieciu przycisku
void ASlashCharacter::EKeyPresed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		//Jeœli klikniemy przycisk E, to podnosimy broñ i doczepiamy do socketu w d³oni
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		//I need to set up OverlappingItem to nullptr, because we don't want to still store the address of the weapon we pick up. Otherwise we will try do this whole process again
		OverlappingItem = nullptr;
		//I have a variable storing the weapon we have equipped now
		EquippedWeapon = OverlappingWeapon;
	}
	else
	{
		//Wrzucamy zwrot z boola aby zobaczyæ czy mo¿emy zdj¹æ/za³o¿yæ broñ 
		if (CanDisarm())
		{
			PlayEquipMontage(FName("Unequip"));	//Odtwarzamy animacjê EquipMontage we should set our state right after that
			CharacterState = ECharacterState::ECS_Unequipped;
		}
		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));	//Odtwarzamy animacjê EquipMontage we should set our state right after that
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		}
	}
}
//Funkcja ataku
void ASlashCharacter::Attack()
{	
	//Jeœli postaæ ma odpowiednie statusy akcji oraz postaci, to atakujemy
	if (CanAttack())
	{
		//Wywo³ujemy funkcjê odtwarzaj¹c¹ animacjê ataku
		PlayAttackMontage();
		//Jeœli postaæ jest w stanie unoccupied, to zmieniamy jej stan na occupied
		ActionState = EActionState::EAS_Attacking;
	}
}
//Funkcja sprawdzaj¹ca czy postaæ mo¿e zaatakowaæ
bool ASlashCharacter::CanAttack()
{
	//Sprawdzamy czy postaæ ma odpowiednie statusy akcji oraz postaci aby móc zaatakowaæ
	return ActionState == EActionState::EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unequipped;
}

//Funkcja sprawdzaj¹ca czy postaæ mo¿e zdj¹æ broñ lub j¹ za³o¿yæ
bool ASlashCharacter::CanDisarm()
{
	//Sprawdzamy action state oraz character state, jeœli s¹ odpowiednie, to mo¿emy odpaliæ animacjê EquipMontage 
	return ActionState == EActionState::EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void ASlashCharacter::Disarm()
{
	//Sprawdzamy, czy broñ jest wyekwipowana i nie jest nullptr
	if (EquippedWeapon)
	{
		//Wywo³ujemy funkcjê, która "doczepia" broñ do socketu. W tym przypadku do socketu na plecach
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
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
//Funkcja odtwarzaj¹ca animacjê EquipMontage
void ASlashCharacter::PlayEquipMontage(FName SectionName)
{	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		//Po wyborze sekcji animacji, odtwarzamy j¹
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

//Funkcja koñcz¹ca atak
void ASlashCharacter::AttackEnd()
{
	//Jeœli postaæ jest w stanie ataku, to zmieniamy jej stan na unoccupied w momencie gdy animacja ataku siê skoñczy
	ActionState = EActionState::EAS_Unoccupied;
}


