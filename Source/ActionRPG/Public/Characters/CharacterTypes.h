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