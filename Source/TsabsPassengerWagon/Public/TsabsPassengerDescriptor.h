#pragma once
#include "Resources/FGVehicleDescriptor.h"
#include "TsabsPassengerDescriptor.generated.h"

/** Native test content only; does not change vehicle or vanilla defaults. */
UCLASS()
class TSABSPASSENGERWAGON_API UTsabsPassengerDescriptor : public UFGVehicleDescriptor
{
    GENERATED_BODY()
public:
    UTsabsPassengerDescriptor();
protected:
    virtual FText GetItemNameInternal() const override;
    virtual FString GetItemNameInternalAsString() const override;
    virtual FText GetItemDescriptionInternal() const override;
};
