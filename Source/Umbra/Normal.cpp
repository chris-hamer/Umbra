#include "Umbra.h"
#include "UmbraCharacter.h"


void AUmbraCharacter::NormalState::Tick(AUmbraCharacter * Player, float DeltaTime)
{
	PlayerState::Tick(Player, DeltaTime);
	PlayerState::Abilities(Player, DeltaTime);
	PlayerState::PhysicsStuff(Player, DeltaTime);
	PlayerState::CameraStuff(Player, DeltaTime);
	PlayerState::FaceTargetDirection(Player, DeltaTime);
	PlayerState::Tick2(Player, DeltaTime);
}