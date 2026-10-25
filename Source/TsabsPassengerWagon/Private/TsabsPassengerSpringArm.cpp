#include "TsabsPassengerSpringArm.h"
#include "TsabsPassengerAnchor.h"
#include "TsabsPassengerRailcar.h"
#include "FGCharacterPlayer.h"
#include "Engine/World.h"

bool UTsabsPassengerSpringArm::CopyLocomotiveFraming(const USpringArmComponent* Source)
{
    if (!IsValid(Source) || !FMath::IsFinite(Source->TargetArmLength) || Source->TargetArmLength <= 0.0f ||
        Source->GetRelativeLocation().ContainsNaN() || Source->GetRelativeRotation().ContainsNaN() ||
        Source->TargetOffset.ContainsNaN() || Source->SocketOffset.ContainsNaN() ||
        !FMath::IsFinite(Source->CameraLagSpeed) || Source->CameraLagSpeed < 0.0f ||
        !FMath::IsFinite(Source->CameraRotationLagSpeed) || Source->CameraRotationLagSpeed < 0.0f ||
        !FMath::IsFinite(Source->CameraLagMaxDistance) || Source->CameraLagMaxDistance < 0.0f ||
        !FMath::IsFinite(Source->CameraLagMaxTimeStep) || Source->CameraLagMaxTimeStep <= 0.0f) return false;
    TargetArmLength = Source->TargetArmLength;
    SetRelativeLocation(Source->GetRelativeLocation());
    SetRelativeRotation(Source->GetRelativeRotation());
    TargetOffset = Source->TargetOffset;
    SocketOffset = Source->SocketOffset;
    bEnableCameraLag = Source->bEnableCameraLag;
    bEnableCameraRotationLag = Source->bEnableCameraRotationLag;
    bUseCameraLagSubstepping = Source->bUseCameraLagSubstepping;
    CameraLagSpeed = Source->CameraLagSpeed;
    CameraRotationLagSpeed = Source->CameraRotationLagSpeed;
    CameraLagMaxDistance = Source->CameraLagMaxDistance;
    CameraLagMaxTimeStep = Source->CameraLagMaxTimeStep;
    return true;
}

FVector UTsabsPassengerSpringArm::BlendLocations(const FVector& DesiredArmLocation,
    const FVector& TraceHitLocation, bool bHitSomething, float DeltaTime)
{
    if (!bHitSomething) return DesiredArmLocation;
    ATsabsPassengerAnchor* Anchor = Cast<ATsabsPassengerAnchor>(GetOwner());
    ATsabsPassengerRailcar* Car = IsValid(Anchor) ? Cast<ATsabsPassengerRailcar>(Anchor->GetOuterVehicle()) : nullptr;
    if (!IsValid(Car) || !GetWorld() || Car->GetWorld() != GetWorld() ||
        Anchor->GetAttachParentActor() != Car)
        return Super::BlendLocations(DesiredArmLocation, TraceHitLocation, bHitSomething, DeltaTime);

    // The stock spring arm ignores only its own actor. Here that is an invisible
    // anchor, NOT the actual wagon: a pivot inside the wagon retracts to zero.
    // Keep the engine's rotation, lag, socket and obstruction policy, but recheck
    // a hit while ignoring our own wagon and its passenger as one camera rig.
    FCollisionQueryParams Query(SCENE_QUERY_STAT(TsabsPassengerCamera), false, Anchor);
    Query.AddIgnoredActor(Car);
    if (IsValid(Anchor->GetDriver())) Query.AddIgnoredActor(Anchor->GetDriver());
    FHitResult Hit;
    // Engine updates PreviousDesiredLoc to the current (possibly lagged) pivot
    // immediately before invoking BlendLocations. Do not substitute actor origin.
    GetWorld()->SweepSingleByChannel(Hit, PreviousDesiredLoc, DesiredArmLocation,
        FQuat::Identity, ProbeChannel, FCollisionShape::MakeSphere(ProbeSize), Query);
    return Super::BlendLocations(DesiredArmLocation, Hit.Location, Hit.bBlockingHit, DeltaTime);
}
