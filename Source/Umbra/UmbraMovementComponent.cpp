// Fill out your copyright notice in the Description page of Project Settings.

#include "Umbra.h"
#include "UmbraMovementComponent.h"




void UUmbraMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult HitResult;
	FHitResult ShapeTraceResult;
	//FCollisionShape shape = FCollisionShape::MakeBox(FVector(0.62f*playerradius, 0.62f*playerradius, 10.0f));
	FCollisionShape shape = FCollisionShape::MakeBox(FVector(0.82f*playerradius, 0.82f*playerradius, 10.0f));

	FCollisionQueryParams Params;
	Params.bFindInitialOverlaps = true;
	Params.AddIgnoredActor(UpdatedComponent->GetAttachmentRootActor());

	TArray<FHitResult> results;
	if (forceregiondirection.Z == 0.0f) {
		GetWorld()->SweepMultiByChannel(results, UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentLocation() - 1000.0f*FVector::UpVector, FQuat::Identity, ECC_Visibility, shape, Params); //100
		for (FHitResult r : results) {
			if (r.Normal.Z > 0.6f) {
				ShapeTraceResult = r;
				break;
			}
		}
		if (!ShapeTraceResult.IsValidBlockingHit()) {
			GetWorld()->LineTraceSingleByChannel(ShapeTraceResult, UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentLocation() - 1000.0f*FVector::UpVector, ECC_Visibility, Params);
		}
	}


	FVector PlayerCapsuleBottom = UpdatedComponent->GetComponentLocation() - playerhalfheight / 2.0f * FVector::UpVector; // 50
	float RequiredDistance = (onground ? 50.0f*grounddetecttfudgefactor : 10.0f) + playerhalfheight / 2.0f; //50,1
	DistanceFromImpact = (PlayerCapsuleBottom - ShapeTraceResult.ImpactPoint).Z;
	overground = ShapeTraceResult.IsValidBlockingHit();

	FHitResult groundhitresult;
	GetWorld()->LineTraceSingleByChannel(groundhitresult, UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentLocation() - 10000.0f*FVector::UpVector, ECC_Visibility, Params);
	groundtracehit = groundhitresult.ImpactPoint;

	if (!onground) {
		offGroundTime += DeltaTime;
	}

	toosteep = false;
	if (enforcementtimer >= 0.0f) {
		enforcementtimer += DeltaTime;
		toosteep = true;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 464568, FColor::Green, FString::SanitizeFloat(onground) + "   zVel: " + FString::SanitizeFloat(PlayerVelocity.Z <= groundvelocity.Z) + "   off: " + FString::SanitizeFloat(offGroundTime > 0.5f) + "   was: " + FString::SanitizeFloat(wasonground));
	wasonground = onground;
	onground = false;
	prevgroundvelocity = groundvelocity;
	groundvelocity = FVector::ZeroVector;
	platformangularfrequency = 0.0f;
	platformspindir = 1;
	FloorNormal = FVector::ZeroVector;

	if ((enforcementtimer < timerlimit && ShapeTraceResult.Normal.Z>0.6f) && DistanceFromImpact < RequiredDistance && !justjumped) { // (PlayerVelocity.Z <= 0.0f || wasonground)
		if (ShapeTraceResult.Normal.Z < minnormalz) {
			if (enforcementtimer == -1.0f) {
				enforcementtimer = 0.0f;
			}
		} else {
			enforcementtimer = -1.0f;
		}

		FVector pvel;
		// Handle moving platforms.
		if (ShapeTraceResult.GetActor() != nullptr && ShapeTraceResult.GetComponent() != nullptr && ShapeTraceResult.GetComponent()->IsA(UStaticMeshComponent::StaticClass())) {

			// The motion of a point on a rigid body is the combination of its motion about the center of mass...
			FVector angvel = FMath::DegreesToRadians((((UStaticMeshComponent*)ShapeTraceResult.GetComponent())->GetPhysicsAngularVelocity()));
			FVector rr = GetActorLocation() - (((UStaticMeshComponent*)ShapeTraceResult.GetComponent())->GetComponentLocation());
			FVector rvel = FVector::CrossProduct(angvel, rr);

			// ...and the motion of the center of mass itself.
			FVector cmvel = (((UStaticMeshComponent*)ShapeTraceResult.GetComponent())->GetPhysicsLinearVelocity());

			groundvelocity = rvel + cmvel;
			platformangularfrequency = -angvel.Z;
		}

		//if ((PlayerVelocity.Z <= groundvelocity.Z || wasonground)) {
		if ((PlayerVelocity.Z <= groundvelocity.Z || offGroundTime > 0.5f || wasonground)) {
			onground = true;
			offGroundTime = 0.0f;
		}
	}

	justjumped = false;

	bool TraceBlocked;
	FVector newlocation = UpdatedComponent->GetComponentLocation();

	FHitResult TraceHitResult;
	TraceBlocked = GetWorld()->LineTraceSingleByChannel(TraceHitResult, ShapeTraceResult.ImpactPoint + 1.0f*FVector::UpVector, ShapeTraceResult.ImpactPoint - 10.0f*FVector::UpVector, ECC_Visibility, Params);
	if (TraceHitResult.Normal.Z > minnormalz) {
		enforcementtimer = -1.0f;
	}

	if (onground) {

		if (TraceBlocked) {
			newlocation.Z = TraceHitResult.ImpactPoint.Z + playerhalfheight; // 50
			GetWorld()->LineTraceSingleByChannel(TraceHitResult, ShapeTraceResult.ImpactPoint + 1.0f*FVector::UpVector, ShapeTraceResult.ImpactPoint - 10.0f*FVector::UpVector, ECC_Visibility, Params);
			FloorNormal = TraceHitResult.ImpactNormal;
		}

		SafeMoveUpdatedComponent(newlocation - UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentRotation(), true, HitResult);
		SlideAlongSurface(newlocation - UpdatedComponent->GetComponentLocation(), 1.0 - HitResult.Time, HitResult.Normal, HitResult);
	}
}