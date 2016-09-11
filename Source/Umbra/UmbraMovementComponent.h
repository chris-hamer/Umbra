// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PawnMovementComponent.h"
#include "UmbraMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class UMBRA_API UUmbraMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
public:
	// Variables to store the floor and wall collision normals.
	FHitResult Floor;
	FHitResult Wall;

	// Temporary variable to tell the actual character class if we're on the ground or not.
	bool onground;
	float offGroundTime;
	float MaxOffGroundTime;
	FVector PlatformVelocity;

	// Temporary variable to tell the actual character class if we WERE on the ground or not.
	bool wasonground;

	// Stores the maximum slope as set by the character class.
	float maxslope;

	// Velocity of the platform you're standing on.
	FVector groundvelocity;
	FVector prevgroundvelocity;
	FVector groundaccel;

	FVector groundtracehit;

	// Vertical velocity of the platform you're standing on.
	// Stored separatly because the z component of groundvelocity
	// is set to zero if it is positive to compensate for collision
	// detection, but its value is still useful.
	float groundverticalvelocity;

	// Angular frequency of the rotating platform that the player is standing on.
	float platformangularfrequency;

	// Is 1 if the platform is spinning clockwise, and -1 if it isn't.
	int8 platformspindir;

	FVector FloorNormal;
	FVector WallNormal;

	FVector PlayerVelocity;

	FVector forceregiondirection;
	bool justjumped;

	bool isclimbing;

	float enforcementtimer;
	float minnormalz;
	float timerlimit;

	float DistanceFromImpact;
	bool overground;

	float grounddetecttfudgefactor;

	float playerradius;
	float playerhalfheight;

	bool toosteep;
	

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};
