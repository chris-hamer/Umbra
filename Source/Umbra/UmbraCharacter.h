// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "UmbraMovementComponent.h"
#include "UmbraCharacter.generated.h"

UCLASS()
class UMBRA_API AUmbraCharacter : public APawn
{
	GENERATED_BODY()

public:

	class PlayerState
	{
	public:
		PlayerState() {};
		virtual FString GetName() { return "None"; };
		virtual void Tick(AUmbraCharacter* Player, float DeltaTime);
		virtual void Tick2(AUmbraCharacter* Player, float DeltaTime);
		virtual void Abilities(AUmbraCharacter* Player, float DeltaTime);
		virtual void PhysicsStuff(AUmbraCharacter* Player, float DeltaTime);
		virtual void CameraStuff(AUmbraCharacter* Player, float DeltaTime);
		virtual void FaceTargetDirection(AUmbraCharacter* Player, float DeltaTime);
	};
	class NormalState : public PlayerState
	{
	public:
		NormalState() {};
		virtual FString GetName() { return "Normal"; };
		virtual void Tick(AUmbraCharacter* Player, float DeltaTime);
	};

	//class DialogueState : public PlayerState
	//{
	//public:
	//	DialogueState() {};
	//	virtual FString GetName() { return "Dialogue"; };
	//	virtual void Tick(AUmbraPlayerCharacter* Player, float DeltaTime);
	//};

	// Sets default values for this pawn's properties
	AUmbraCharacter();

	void MoveRight(float AxisValue);
	void MoveForward(float AxisValue);
	void YawCamera(float AxisVlue);
	void PitchCamera(float AxisValue);
	void Jump();
	void Use();
	void Eyes(); void UnEyes();
	void CameraFaceForward();
	void CameraModeToggle();

	void FlattenVelocity();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") UCapsuleComponent* CapsuleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") UStaticMeshComponent* PlayerModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") UUmbraMovementComponent* MovementComponent;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") USkeletalMeshComponent* PlayerModel;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float GroundAccelRate = 5000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float AirAccelRate = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float MaxSpeed = 450.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float TurningSpeed = 270.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float JumpPower = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float CoyoteJumpTime = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float EyesCloseSpeed = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") FVector Gravity = FVector(0.0, 0.0, -1600.0f);

	// State
	class NormalState Normal;
	class PlayerState* CurrentState = &Normal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") bool OnTheGround;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") float EyesValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") FVector TargetFacingDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") bool EyesClosed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") FVector GroundVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") FVector GroundNormal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") FVector Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") FVector Forward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") FVector MarkedLocation=FVector(0.0f,0.0f,-10000.0f);

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") FVector MovementInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") FVector CameraInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") bool JumpPressed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") bool EyesPressed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") bool UsePressed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") bool CameraFaceForwardPressed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") bool CameraModeTogglePressed;

};
