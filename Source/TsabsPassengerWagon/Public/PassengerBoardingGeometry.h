#pragma once
#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

namespace TsabsPassenger
{
/** Shared production geometry. Range must be Character->GetUseDistance(), never a client hit. */
inline bool CanReachBoardingBody(UPrimitiveComponent* Body, const AActor* IgnoredCharacter,
    float UseDistance, const FVector& CharacterOrigin, const FVector& ViewOrigin, const FRotator& ViewRotation)
{
    if (!IsValid(Body) || !Body->IsRegistered() || !Body->GetWorld() || !Body->IsCollisionEnabled() ||
        !FMath::IsFinite(UseDistance) || UseDistance <= 0.0f || CharacterOrigin.ContainsNaN() ||
        ViewOrigin.ContainsNaN() || ViewRotation.ContainsNaN()) return false;
    FVector ClosestBodyPoint;
    const float DistanceToBody = Body->GetClosestPointOnCollision(CharacterOrigin, ClosestBodyPoint);
    // Missing collision is unavailable; never fall back to a long vehicle's centre/bounds.
    if (!FMath::IsFinite(DistanceToBody) || DistanceToBody < 0.0f || ClosestBodyPoint.ContainsNaN() ||
        FVector::DistSquared(CharacterOrigin, ClosestBodyPoint) > FMath::Square(double(UseDistance))) return false;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(TsabsPassengerBoarding), true, IgnoredCharacter);
    FHitResult BodyHit;
    const FVector End = ViewOrigin + ViewRotation.Vector() * UseDistance;
    // Component geometry traces return a hit without necessarily setting the
    // scene-query bBlockingHit flag (verified against this engine's box/BodyInstance).
    if (End.ContainsNaN() || !Body->LineTraceComponent(BodyHit, ViewOrigin, End, Params) ||
        BodyHit.bStartPenetrating || BodyHit.ImpactPoint.ContainsNaN() ||
        FVector::DistSquared(ViewOrigin, BodyHit.ImpactPoint) > FMath::Square(double(UseDistance))) return false;
    FHitResult Obstruction;
    if (Body->GetWorld()->LineTraceSingleByChannel(Obstruction, ViewOrigin, BodyHit.ImpactPoint, ECC_Visibility, Params) &&
        Obstruction.GetComponent() != Body) return false;
    return true;
}
}
