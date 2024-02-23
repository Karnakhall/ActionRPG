// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SlashCharacter.h"
#include "SlashAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime)	override;
	//Our character
	UPROPERTY(BlueprintReadOnly)
	class ASlashCharacter* SlashCharacter;
	//Our character movement
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* SlashCharacterMovement;
	//Our character speed
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;
	//Is Our character falling?
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	ECharacterState CharacterState;
};
