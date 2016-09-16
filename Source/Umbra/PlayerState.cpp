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
	} else {
		Player->FlattenVelocity();
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
	FQuat TargetRotation = Player->TargetFacingDirection.Rotation().Quaternion();
	FQuat CurrentRotation = Player->PlayerModel->RelativeRotation.Quaternion();

	int8 reflect = (Player->MovementInput.X >= 0 ? 1 : -1);
	FQuat Between = FQuat::FindBetween(Player->PlayerModel->GetForwardVector(), Player->TargetFacingDirection);
	FQuat Inc = FQuat::FindBetween(Player->PlayerModel->GetForwardVector(), (Player->PlayerModel->RelativeRotation + FRotator(0.0f, Player->TurningSpeed*DeltaTime, 0.0f)).Vector());

	FVector dummy1;
	FVector dummy2;
	float angle_between, angle_inc;
	Between.ToAxisAndAngle(dummy1, angle_between);
	Inc.ToAxisAndAngle(dummy2, angle_inc);

	angle_between = (dummy1.Z >= 0 ? 1 : -1)*FMath::RadiansToDegrees(angle_between);
	angle_inc = FMath::RadiansToDegrees(angle_inc)*(angle_between >= 0 ? 1 : -1);

	if (FMath::Abs(angle_inc) > FMath::Abs(angle_between)) {
		Player->PlayerModel->SetWorldRotation(TargetRotation);
	} else {
		Player->PlayerModel->AddLocalRotation(FRotator(0.0f, angle_inc, 0.0));
	}
}
