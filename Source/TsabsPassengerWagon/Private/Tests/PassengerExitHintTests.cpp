#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Blueprint/UserWidget.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsPassengerExitHintTest,
    "Tsabs.Passenger.Interface.ExitPromptUsesStandardKeyMarkup",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTsabsPassengerExitHintTest::RunTest(const FString& Parameters)
{
    UClass* HUD = FindObject<UClass>(nullptr, TEXT("/Script/TsabsPassengerWagon.TsabsPassengerHUD"));
    if (!TestNotNull(TEXT("Local passenger presentation class"), HUD)) return false;
    UFunction* Format = HUD->FindFunctionByName(TEXT("FormatExitPrompt"));
    if (!TestNotNull(TEXT("Exit presentation uses the normal interaction-text contract"), Format)) return false;
    struct FParams { FText LeaveKey; bool bWithinSpeedLimit; FText ReturnValue; } Args;
    for (const TCHAR* Key : {TEXT("E"), TEXT("F"), TEXT("Left Alt + Q")})
    {
        Args.LeaveKey = FText::FromString(Key);
        Args.bWithinSpeedLimit = true;
        Args.ReturnValue = FText::GetEmpty();
        HUD->GetDefaultObject()->ProcessEvent(Format, &Args);
        TestTrue(TEXT("Current binding uses the standard key capsule markup"),
            Args.ReturnValue.ToString().Contains(FString::Printf(TEXT("<Key>%s</>"), Key)));
        TestTrue(TEXT("Action describes dismounting"), Args.ReturnValue.ToString().Contains(TEXT("bajar del vagón")));
    }
    Args.bWithinSpeedLimit = false;
    HUD->GetDefaultObject()->ProcessEvent(Format, &Args);
    TestFalse(TEXT("At-or-above-limit text does not advertise an exit key"), Args.ReturnValue.ToString().Contains(TEXT("<Key>")));
    TestTrue(TEXT("30 km/h access limit is explained"), Args.ReturnValue.ToString().Contains(TEXT("30")) && Args.ReturnValue.ToString().Contains(TEXT("km/h")));
    Args.bWithinSpeedLimit = true;
    Args.LeaveKey = FText::GetEmpty();
    HUD->GetDefaultObject()->ProcessEvent(Format, &Args);
    TestFalse(TEXT("An unmapped action never invents an E binding or unknown-key icon"), Args.ReturnValue.ToString().Contains(TEXT("<Key>")));
    return true;
}
#endif
