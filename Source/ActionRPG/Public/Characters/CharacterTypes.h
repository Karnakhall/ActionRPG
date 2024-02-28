#pragma once

//Allows us to use the enum in blueprint
UENUM(BlueprintType)
//Enum to keep track of the character state
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped")/*UMETA makro pozwala zmieniæ wyœwietlan¹ nazwê w blueprintach */,
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "EquippedOneHandedWeapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "EquippedTwoHandedWeapon"),
};
//Allows us to use the enum in blueprint
UENUM(BlueprintType)
//Enum to keep track of the action state
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon")
};