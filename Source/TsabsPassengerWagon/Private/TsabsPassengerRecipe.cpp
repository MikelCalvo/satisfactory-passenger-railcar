#include "TsabsPassengerRecipe.h"
#include "TsabsPassengerDescriptor.h"

UTsabsPassengerRecipe::UTsabsPassengerRecipe()
{
    mIngredients.Empty();
    mProduct = { FItemAmount(UTsabsPassengerDescriptor::StaticClass(), 1) };
    mProducedIn = { TSoftClassPtr<UObject>(FSoftObjectPath(TEXT("/Game/FactoryGame/Equipment/BuildGun/BP_BuildGun.BP_BuildGun_C"))) };
    mManufactoringDuration = 1.0f;
    mManualManufacturingMultiplier = 1.0f;
    mRelevantEvents.Empty();
    mMaterialCustomizationRecipe = nullptr;
    mDisplayNameOverride = true;
    mDisplayName = NSLOCTEXT("TsabsPassengerWagon", "TestWagonName", "Vagón de pasajeros (prueba)");
}
