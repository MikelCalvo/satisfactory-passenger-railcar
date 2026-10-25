#include "TsabsPassengerRailMovement.h"
#include "TsabsPassengerWagonLog.h"
#include "UObject/UnrealType.h"

bool UTsabsPassengerRailMovement::CopyEditableSetupFrom(const UFGRailroadVehicleMovementComponent* Source)
{
    bCopiedSetup = false;
    if (!IsValid(Source))
    {
        return false;
    }
    // Only copy the documented editable setup, never owner, track, simulation
    // state, replication state, or arbitrary component properties.
    const TCHAR* Fields[] = {
        TEXT("mWheelsetSetups"), TEXT("mWheelRadius"), TEXT("mCouplerSetups"),
        TEXT("mMass"), TEXT("mMaxVelocity"), TEXT("mMaxPayloadMass"),
        TEXT("mDragCoefficient"), TEXT("mChassisWidth"), TEXT("mChassisHeight"),
        TEXT("mRollingResistanceCoefficient"), TEXT("mCurvatureResistanceCoefficient"),
        TEXT("mMaxAirBrakingEffort")
    };
    for (const TCHAR* Name : Fields)
    {
        const FProperty* Property = FindFProperty<FProperty>(UFGRailroadVehicleMovementComponent::StaticClass(), Name);
        if (!Property)
        {
            UE_LOG(LogTsabsPassengerWagon, Error, TEXT("Missing rail setup property: %s"), Name);
            return false;
        }
        Property->CopyCompleteValue(Property->ContainerPtrToValuePtr<void>(this),
            Property->ContainerPtrToValuePtr<const void>(Source));
    }
    // Runtime ComputeConstants calculates these from the actual game mesh.
    for (FWheelsetSetup& Setup : mWheelsetSetups) { Setup.Offset = 0.0f; }
    for (FCouplerSetup& Setup : mCouplerSetups) { Setup.Offset = 0.0f; }
    SetPayloadMass(0.0f);
    SetForwardSpeed(0.0f);
    mOrientation = 1.0f;
    bCopiedSetup = mWheelsetSetups.Num() == 2 && mCouplerSetups.Num() == 2 &&
        FMath::IsFinite(mMass) && mMass > 0.0f && mWheelRadius > 0.0f && mMaxVelocity > 0.0f;
    return bCopiedSetup;
}
