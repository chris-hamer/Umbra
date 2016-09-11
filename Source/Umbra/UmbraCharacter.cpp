// Fill out your copyright notice in the Description page of Project Settings.

#include "Umbra.h"
#include "UmbraCharacter.h"


// Sets default values
AUmbraCharacter::AUmbraCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->InitCapsuleSize(45.0f, 80.0f);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	CapsuleComponent->SetSimulatePhysics(true);
	CapsuleComponent->SetEnableGravity(false);
	CapsuleComponent->SetLinearDamping(0.0f);
	CapsuleComponent->BodyInstance.PositionSolverIterationCount = 16;
	CapsuleComponent->BodyInstance.VelocitySolverIterationCount = 16;
	CapsuleComponent->BodyInstance.bLockXRotation = true;
	CapsuleComponent->BodyInstance.bLockYRotation = true;
	CapsuleComponent->BodyInstance.bLockZRotation = true;
	RootComponent = CapsuleComponent;

	PlayerModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Model"));
	PlayerModel->SetupAttachment(CapsuleComponent);
	const ConstructorHelpers::FObjectFinder<UStaticMesh> PlayerMeshObj(TEXT("/Game/StarterContent/Props/SM_Chair"));
	PlayerModel->SetStaticMesh(PlayerMeshObj.Object);
	PlayerModel->SetRelativeLocation(FVector(0.0f, 0.0f, -70.0f));
	PlayerModel->bRenderCustomDepth = true;
	PlayerModel->SetSimulatePhysics(false);
	PlayerModel->bReceivesDecals = false;
	PlayerModel->bCastInsetShadow = false;
	PlayerModel->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlayerModel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayerModel->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PlayerModel->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Block);
	PlayerModel->SetCollisionObjectType(ECC_PhysicsBody);
	PlayerModel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayerModel->SetSimulatePhysics(false);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(-15.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 1000.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagMaxDistance = 1000.0f;
	SpringArm->ProbeSize = 20.0f;
	SpringArm->SetupAttachment(CapsuleComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	const ConstructorHelpers::FObjectFinder<UMaterialInterface> EyesCloseEffectRef(TEXT("/Game/Materials/EyesCloseEffect"));
	Camera->AddOrUpdateBlendable(EyesCloseEffectRef.Object);
	//const ConstructorHelpers::FObjectFinder<UMaterialInterface> WarningEffectRef(TEXT("/Game/Materials/BadDudeWarning"));
	//Camera->AddOrUpdateBlendable(WarningEffectRef.Object);


	MovementComponent = CreateDefaultSubobject<UUmbraMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = CapsuleComponent;
	MovementComponent->MaxOffGroundTime = CoyoteJumpTime;
	MovementComponent->playerradius = CapsuleComponent->GetUnscaledCapsuleRadius();
	MovementComponent->playerhalfheight = CapsuleComponent->GetUnscaledCapsuleHalfHeight();

}

// Called when the game starts or when spawned
void AUmbraCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AUmbraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentState != nullptr) {
		CurrentState->Tick(this, DeltaTime);
	}
}

// Called to bind functionality to input
void AUmbraCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->SetTickableWhenPaused(true);
	InputComponent->Priority = 1000;
	InputComponent->BindAxis("MoveX", this, &AUmbraCharacter::MoveRight);
	InputComponent->BindAxis("MoveY", this, &AUmbraCharacter::MoveForward);
	InputComponent->BindAxis("CameraPitch", this, &AUmbraCharacter::PitchCamera);
	InputComponent->BindAxis("CameraYaw", this, &AUmbraCharacter::YawCamera);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AUmbraCharacter::Jump);
	InputComponent->BindAction("CloseEyes", IE_Pressed, this, &AUmbraCharacter::Eyes);
	InputComponent->BindAction("CloseEyes", IE_Released, this, &AUmbraCharacter::UnEyes);
	InputComponent->BindAction("Use", IE_Pressed, this, &AUmbraCharacter::Use);
	InputComponent->BindAction("CameraFaceForward", IE_Pressed, this, &AUmbraCharacter::CameraFaceForward);
	InputComponent->BindAction("CameraMode", IE_Pressed, this, &AUmbraCharacter::CameraModeToggle);
}

void AUmbraCharacter::MoveRight(float AxisValue)
{
	MovementInput.X = AxisValue;
}

void AUmbraCharacter::MoveForward(float AxisValue)
{
	MovementInput.Y = AxisValue;
}

void AUmbraCharacter::YawCamera(float AxisValue)
{
	CameraInput.X = AxisValue;
}

void AUmbraCharacter::PitchCamera(float AxisValue)
{
	CameraInput.Y = AxisValue;
}

void AUmbraCharacter::Jump()
{
	JumpPressed = true;
}

void AUmbraCharacter::Eyes()
{
	EyesPressed = true;
}

void AUmbraCharacter::UnEyes()
{
	EyesPressed = false;
}

void AUmbraCharacter::Use()
{
	UsePressed = true;
}

void AUmbraCharacter::CameraFaceForward()
{
	CameraFaceForwardPressed = true;
}

void AUmbraCharacter::CameraModeToggle()
{
	CameraModeTogglePressed = true;
}