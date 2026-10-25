#pragma once
#include "Module/GameWorldModule.h"
#include "TsabsPassengerWorldModule.generated.h"

struct FTsabsPassengerContentState
{
    bool bRegistrationAttempted = false;
    bool bRegistrationVerified = false;
    bool bAvailabilityAttempted = false;
    bool bAvailabilityVerified = false;
};

/** One native SML root per game world. Runtime owner is supplied by SML. */
UCLASS()
class TSABSPASSENGERWAGON_API UTsabsPassengerWorldModule : public UGameWorldModule
{
    GENERATED_BODY()
public:
    UTsabsPassengerWorldModule();
    virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;
    const FTsabsPassengerContentState& GetContentState() const { return ContentState; }
private:
    // Per-world instance only; never static or saved as proof of a future session.
    FTsabsPassengerContentState ContentState;
};
