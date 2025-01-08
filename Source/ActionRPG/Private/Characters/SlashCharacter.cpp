// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GroomComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Item.h"		//Potrzebujemy tego nag��wka aby m�c podnie�� bro�
#include "Items/Weapons/Weapon.h"	//Potrzebujemy tego nag��wka aby m�c podnie�� bro�
#include "Animation/AnimMontage.h"	//Potrzebujemy tego nag��wka aby m�c u�y� AnimMontage
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Soul.h"	
#include "Items/Treasure.h"	

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
//#include "Components/BoxComponent.h"	//Potrzebujemy tego nag��wka aby m�c u�y� BoxComponent


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

void ASlashCharacter::Tick(float DeltaTime)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->RegenStamina(DeltaTime);	//Funkcja do regeneracji staminy
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());	//Ustawiamy procent staminy w StaminaProgressBar
	}
}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
	}

	//PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForward);
	//PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRight);
	//PlayerInputComponent->BindAxis(FName("Turn"), this, &ASlashCharacter::Turn);
	//PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUp);

	//PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ASlashCharacter::Jump);
	//PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	//PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);
	//PlayerInputComponent->BindAction(FName("Dodge"), IE_Pressed, this, &ASlashCharacter::Dodge);

}

void ASlashCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
	
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);	//Wywo�ujemy funkcj� obs�ugi obra�e� dla naszej postaci
	SetHUDHealth();
	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSoulsText(Attributes->GetSouls());
	}
	UE_LOG(LogTemp, Warning, TEXT("Souls added"));
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGoldText(Attributes->GetGold());
	}
	UE_LOG(LogTemp, Warning, TEXT("Gold added"));
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashContext, 0);
		}
	}

	Tags.Add(FName("EngageableTarget"));	//Dodajemy tag do postaci, �eby m�c j� zidentyfikowa� albo �eby nasi przeciwnicy mogli ja zidentyfikowa�
	
	InitializeSlashOverlay();	//Funkcja inicjalizuj�ca SlashOverlay i wy�wietlaj�ca informacje o postaci na HUD
	
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	/*const FVector Forward = GetActorForwardVector();
	AddMovementInput(Forward, MovementVector.Y);
	const FVector Right = GetActorRightVector();
	AddMovementInput(Right, MovementVector.X);*/
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void ASlashCharacter::MoveForward(float Value)
{
	//Je�li posta� jest w stanie ataku, to nie mo�e si� porusza�
	if (ActionState != EActionState::EAS_Unoccupied) return;
	//Je�li posta� nie atakuje, to mo�e si� porusza�
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
	//Je�li posta� jest w stanie ataku, to nie mo�e si� porusza�
	if (ActionState != EActionState::EAS_Unoccupied) return;
	//Je�li posta� nie atakuje, to mo�e si� porusza�
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

//Funkcja podnoszenia broni po naci�nieciu przycisku
void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		//Wrzucamy zwrot z boola aby zobaczy� czy mo�emy zdj��/za�o�y� bro� 
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
	//Je�li posta� ma odpowiednie statusy akcji oraz postaci, to atakujemy
	if (CanAttack())
	{
		//Wywo�ujemy funkcj� odtwarzaj�c� animacj� ataku
		PlayAttackMontage();
		//Je�li posta� jest w stanie unoccupied, to zmieniamy jej stan na occupied
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina()) return;
	
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && SlashOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());	//Ustawiamy procent staminy w StaminaProgressBar
	}
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	//Je�li klikniemy przycisk E, to podnosimy bro� i doczepiamy do socketu w d�oni
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);

	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	//I need to set up OverlappingItem to nullptr, because we don't want to still store the address of the weapon we pick up. Otherwise we will try do this whole process again
	OverlappingItem = nullptr;
	//I have a variable storing the weapon we have equipped now
	EquippedWeapon = Weapon;
}

//Funkcja ko�cz�ca atak
void ASlashCharacter::AttackEnd()
{
	//Je�li posta� jest w stanie ataku, to zmieniamy jej stan na unoccupied w momencie gdy animacja ataku si� sko�czy
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

//Funkcja sprawdzaj�ca czy posta� mo�e zaatakowa�
bool ASlashCharacter::CanAttack()
{
	//Sprawdzamy czy posta� ma odpowiednie statusy akcji oraz postaci aby m�c zaatakowa�
	return ActionState == EActionState::EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unequipped;
}

//Funkcja sprawdzaj�ca czy posta� mo�e zdj�� bro� lub j� za�o�y�
bool ASlashCharacter::CanDisarm()
{
	//Sprawdzamy action state oraz character state, je�li s� odpowiednie, to mo�emy odpali� animacj� EquipMontage 
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
	PlayEquipMontage(FName("Unequip"));	//Odtwarzamy animacj� EquipMontage we should set our state right after that
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));	//Odtwarzamy animacj� EquipMontage we should set our state right after that
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::AttachWeaponToBack()
{
	//Sprawdzamy, czy bro� jest wyekwipowana i nie jest nullptr
	if (EquippedWeapon)
	{
		//Wywo�ujemy funkcj�, kt�ra "doczepia" bro� do socketu. W tym przypadku do socketu na plecach
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	
	if (EquippedWeapon)
	{
		//Wywo�ujemy funkcj�, kt�ra "doczepia" bro� do socketu. W tym przypadku do socketu w prawej d�oni
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

//Funkcja odtwarzaj�ca animacj� EquipMontage
void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)	//Je�li AnimInstance i EquipMontage nie s� nullpointerami, to odtwarzamy animacj� equip
	{
		AnimInstance->Montage_Play(EquipMontage);
		//Po wyborze sekcji animacji, odtwarzamy j�
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;	//Zmieniamy stan postaci na dead
	DisableMeshCollision();	//Funkcja do wy��czenia kolizji na meshe aby nie mo�na by�a zaatakowa� martwego przeciwnika
}

bool ASlashCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();	//Sprawdzamy czy posta� ma wystarczaj�co du�o staminy aby m�c unikn�� ciosu
}

bool ASlashCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void ASlashCharacter::FinishEquipping()
{
	//All we neeed to do is to change our action state to unoccupied
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;	//Resetujemy stan postaci
}

bool ASlashCharacter::IsUnoccupied()	//Funkcja sprawdzaj�ca czy posta� jest w stanie unoccupied
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ASlashCharacter::InitializeSlashOverlay()	//Funkcja inicjalizuj�ca SlashOverlay i wy�wietlaj�ca informacje o postaci na HUD
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());	//
	if (PlayerController)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());	//Pobieramy HUD
		if (SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();	//Pobieramy SlashOverlay
			if (SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());	//Ustawiamy procent zdrowia w HealthProgressBar
				SlashOverlay->SetStaminaBarPercent(1.f);	//Ustawiamy procent staminy w StaminaProgressBar
				SlashOverlay->SetGoldText(0.f);	//Ustawiamy ilo�� z�ota w GoldText
				SlashOverlay->SetSoulsText(0.f);	//Ustawiamy ilo�� dusz w SoulsText
			}
		}
	}
}

void ASlashCharacter::SetHUDHealth()	//Function to update health on HUD
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());	//Ustawiamy procent zdrowia w HealthProgressBar aby sie aktualizowa� po otrzymaniu obra�e�
	}
}


