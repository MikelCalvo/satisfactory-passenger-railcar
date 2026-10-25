#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "TsabsPassengerSpringArm.generated.h"

/** Stock orbit/lag with own outer-railcar excluded from camera obstruction. */
UCLASS()
class TSABSPASSENGERWAGON_API UTsabsPassengerSpringArm : public USpringArmComponent
{
    GENERATED_BODY()
public:
    /** Framing/smoothing only: no ownership, input mode, or collision-policy copy. */
    UFUNCTION(BlueprintCallable, Category="Passenger Camera")
    bool CopyLocomotiveFraming(const USpringArmComponent* Source);
protected:
    virtual FVector BlendLocations(const FVector& DesiredArmLocation,
        const FVector& TraceHitLocation, bool bHitSomething, float DeltaTime) override;
};
