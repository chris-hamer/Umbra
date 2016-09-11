#include "Umbra.h"
#include "UmbraCharacter.h"


void AUmbraCharacter::PlayerState::Tick(AUmbraCharacter * Player, float DeltaTime)
{
	Player->MovementInput = Player->MovementInput.GetClampedToMaxSize(1.0f);
	Player->Right = FVector::VectorPlaneProject(Player->Camera->GetRightVector(), FVector::UpVector).GetSafeNormal();
	Player->Forward = FVector::VectorPlaneProject(Player->Camera->GetForwardVector(), FVector::UpVector).GetSafeNormal();

	Player->OnTheGround = Player->MovementComponent->onground;
	Player->MovementComponent->PlayerVelocity = Player->CapsuleComponent->GetPhysicsLinearVelocity();
}

void AUmbraCharacter::PlayerState::Tick2(AUmbraCharacter * Player, float DeltaTime)
{
	Player->JumpPressed = false;
	Player->UsePressed = false;
	Player->CameraFaceForwardPressed = false;
	Player->CameraModeTogglePressed = false;

	Player->MovementComponent->onground = Player->OnTheGround;
}

void AUmbraCharacter::PlayerState::Abilities(AUmbraCharacter * Player, float DeltaTime)
{
	if (Player->JumpPressed && (Player->OnTheGround || Player->MovementComponent->offGroundTime < Player->CoyoteJumpTime)) {
		Player->OnTheGround = false;
		Player->MovementComponent->justjumped = true;
		Player->CapsuleComponent->AddImpulse(Player->JumpPower*FVector::UpVector, NAME_None, true);
		Player->MovementComponent->PlayerVelocity = Player->CapsuleComponent->GetPhysicsLinearVelocity();
		Player->TargetFacingDirection = Player->PlayerModel->GetForwardVector();
	}

	Player->EyesValue = FMath::FInterpTo(Player->EyesValue, Player->EyesPressed, DeltaTime, Player->EyesCloseSpeed);

	if (Player->UsePressed) {
		Player->MarkedLocation = Player->GetActorLocation();
	}
}

void AUmbraCharacter::PlayerState::PhysicsStuff(AUmbraCharacter * Player, float DeltaTime)
{
	FVector AdjustedMovementInput = Player->MovementInput.X*Player->Right + Player->MovementInput.Y*Player->Forward;
	FVector Velocity2D = FVector::VectorPlaneProject(Player->CapsuleComponent->GetPhysicsLinearVelocity(), FVector::UpVector);
	float AccelRate = (Player->OnTheGround ? Player->GroundAccelRate : Player->AirAccelRate);
	float VelocityRatio = Velocity2D.Size() / Player->MaxSpeed;
	if (!Player->MovementInput.IsNearlyZero() && Player->OnTheGround) { Player->TargetFacingDirection = AdjustedMovementInput.GetSafeNormal(); }

	Player->CapsuleComponent->AddForce(AdjustedMovementInput*AccelRate, NAME_None, true);

	Player->CapsuleComponent->AddForce(-Velocity2D.GetSafeNormal()*AccelRate*VelocityRatio, NAME_None, true);

	if (!Player->OnTheGround) {
		Player->CapsuleComponent->AddForce(Player->Gravity, NAME_None, true);
	}
}

void AUmbraCharacter::PlayerState::CameraStuff(AUmbraCharacter * Player, float DeltaTime)
{
	Player->SpringArm->AddRelativeRotation(FRotator(0.0, Player->CameraInput.X, 0.0f));
	Player->SpringArm->AddRelativeRotation(FRotator(Player->CameraInput.Y, 0.0, 0.0f));
	if (FMath::Abs(Player->SpringArm->RelativeRotation.Pitch) >= 80.0f) {
		Player->SpringArm->RelativeRotation.Pitch = 80.0f*FMath::Sign(Player->SpringArm->RelativeRotation.Pitch);
	}
}

void AUmbraCharacter::PlayerState::FaceTargetDirection(AUmbraCharacter * Player, float DeltaTime)
{
	FRotator TargetRotation = Player->TargetFacingDirection.Rotation();
	FQuat NewRotation = FQuat::Slerp(Player->PlayerModel->GetComponentRotation().Quaternion(), Player->TargetFacingDirection.Rotation().Quaternion(), Player->TurningSpeed*DeltaTime);
	Player->PlayerModel->SetWorldRotation(NewRotation);
}
