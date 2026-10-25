#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UObject/UnrealType.h"
#include "TsabsPassengerHUD.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsPassengerHudDeferredAssetsTest,
    "Tsabs.Passenger.Interface.NoGameAssetsLoadedByNativeHUDConstructor",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTsabsPassengerHudDeferredAssetsTest::RunTest(const FString& Parameters)
{
    const UTsabsPassengerHUD* Defaults = GetDefault<UTsabsPassengerHUD>();
    for (const FName Name : {FName(TEXT("InteractionWidgetClass")), FName(TEXT("LeaveAction"))})
    {
        FObjectPropertyBase* Property = FindFProperty<FObjectPropertyBase>(UTsabsPassengerHUD::StaticClass(), Name);
        if (!TestNotNull(TEXT("Existing runtime asset field remains available"), Property)) return false;
        TestNull(FString::Printf(TEXT("CDO must not synchronously resolve %s before engine module initialization"), *Name.ToString()),
            Property->GetObjectPropertyValue_InContainer(Defaults));
    }
    // Real-widget composition and effective-key tests separately require both
    // assets to resolve on a possessed runtime instance. No rendering bypass.
    return true;
}
#endif
