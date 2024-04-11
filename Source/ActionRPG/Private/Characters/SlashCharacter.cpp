// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"		//Potrzebujemy tego nag³ówka aby móc podnieœæ broñ
#include "Items/Weapons/Weapon.h"	//Potrzebujemy tego nag³ówka aby móc podnieœæ broñ
#include "Animation/AnimMontage.h"	//Potrzebujemy tego nag³ówka aby móc u¿yæ AnimMontage
//#include "Components/BoxComponent.h"	//Potrzebujemy tego nag³ówka aby móc u¿yæ BoxComponent


// Sets default values
ASlashCharacter::ASlashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

//Set this character to be controlled by the lowest-numbered player
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

//Set the character to move in the direction it is facing
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);	// Set the mesh to generate overlap events

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

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));	//Dodajemy tag do postaci, ¿eby móc j¹ zidentyfikowaæ albo ¿eby nasi przeciwnicy mogli ja zidentyfikowaæ
	
}

void ASlashCharacter::MoveForward(float Value)
{
	//Jeœli postaæ jest w stanie ataku, to nie mo¿e siê poruszaæ
	if (ActionState != EActionState::EAS_Unoccupied) return;
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
	if (ActionState != EActionState::EAS_Unoccupied) return;
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
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		//Wrzucamy zwrot z boola aby zobaczyæ czy mo¿emy zdj¹æ/za³o¿yæ broñ 
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

//Funkcja ataku
void ASlashCharacter::Attack()
{	
	Super::Attack();
	//Jeœli postaæ ma odpowiednie statusy akcji oraz postaci, to atakujemy
	if (CanAttack())
	{
		//Wywo³ujemy funkcjê odtwarzaj¹c¹ animacjê ataku
		PlayAttackMontage();
		//Jeœli postaæ jest w stanie unoccupied, to zmieniamy jej stan na occupied
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	//Jeœli klikniemy przycisk E, to podnosimy broñ i doczepiamy do socketu w d³oni
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);

	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	//I need to set up OverlappingItem to nullptr, because we don't want to still store the address of the weapon we pick up. Otherwise we will try do this whole process again
	OverlappingItem = nullptr;
	//I have a variable storing the weapon we have equipped now
	EquippedWeapon = Weapon;
}

//Funkcja koñcz¹ca atak
void ASlashCharacter::AttackEnd()
{
	//Jeœli postaæ jest w stanie ataku, to zmieniamy jej stan na unoccupied w momencie gdy animacja ataku siê skoñczy
	ActionState = EActionState::EAS_Unoccupied;
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
	PlayEquipMontage(FName("Unequip"));	//Odtwarzamy animacjê EquipMontage we should set our state right after that
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));	//Odtwarzamy animacjê EquipMontage we should set our state right after that
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::AttachWeaponToBack()
{
	//Sprawdzamy, czy broñ jest wyekwipowana i nie jest nullptr
	if (EquippedWeapon)
	{
		//Wywo³ujemy funkcjê, która "doczepia" broñ do socketu. W tym przypadku do socketu na plecach
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	
	if (EquippedWeapon)
	{
		//Wywo³ujemy funkcjê, która "doczepia" broñ do socketu. W tym przypadku do socketu w prawej d³oni
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

//Funkcja odtwarzaj¹ca animacjê EquipMontage
void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)	//Jeœli AnimInstance i EquipMontage nie s¹ nullpointerami, to odtwarzamy animacjê equip
	{
		AnimInstance->Montage_Play(EquipMontage);
		//Po wyborze sekcji animacji, odtwarzamy j¹
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::FinishEquipping()
{
	//All we neeed to do is to change our action state to unoccupied
	ActionState = EActionState::EAS_Unoccupied;
}






