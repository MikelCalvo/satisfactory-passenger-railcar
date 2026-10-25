#pragma once
#include "CoreMinimal.h"
namespace TsabsPassenger
{
/** Provisional LOCAL lateral candidate only. Never a substitute for native safe-exit validation. */
inline bool TryMakeExitOffset(const FBox& BodyBounds, double CapsuleRadius, double CapsuleHalfHeight, FVector& OutOffset)
{
    if (!BodyBounds.IsValid || BodyBounds.Min.ContainsNaN() || BodyBounds.Max.ContainsNaN() ||
        !FMath::IsFinite(CapsuleRadius) || !FMath::IsFinite(CapsuleHalfHeight) ||
        CapsuleRadius <= 0.0 || CapsuleHalfHeight < CapsuleRadius) return false;
    const FVector Size = BodyBounds.GetSize();
    if (Size.ContainsNaN() || Size.X <= 0.0 || Size.Y <= 0.0 || Size.Z <= 0.0) return false;
    // GetSafeExitLocation's downward fallback sweeps TO this target and
    // accepts a non-initial contact. This is a ground-probe endpoint, not
    // the final capsule centre: adding half-height can leave the entire
    // sweep above a clear floor when the chassis is elevated over its pivot.
    const FVector Candidate(BodyBounds.GetCenter().X, BodyBounds.Max.Y + 2.0 * CapsuleRadius,
        FMath::Min(0.0, BodyBounds.Min.Z));
    if (Candidate.ContainsNaN()) return false;
    OutOffset = Candidate;
    return true;
}
}
