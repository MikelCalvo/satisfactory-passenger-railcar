#pragma once

#include "CoreMinimal.h"
#include "FGRailroadVehicleMovementComponent.h"
#include "TsabsPassengerRailMovement.generated.h"

UCLASS()
class TSABSPASSENGERWAGON_API UTsabsPassengerRailMovement : public UFGRailroadVehicleMovementComponent
{
    GENERATED_BODY()
public:
    bool CopyEditableSetupFrom(const UFGRailroadVehicleMovementComponent* Source);
    bool HasCopiedFreightSetup() const { return bCopiedSetup; }
    int32 GetConfiguredWheelsetCount() const { return mWheelsetSetups.Num(); }
    int32 GetConfiguredCouplerCount() const { return mCouplerSetups.Num(); }
private:
    bool bCopiedSetup = false;
};
