#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "TsabsPassengerRailcar.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsPassengerBoardingKeyPromptTest,
    "Tsabs.Passenger.Interface.BoardingPromptUsesSuppliedKeyMarkup",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTsabsPassengerBoardingKeyPromptTest::RunTest(const FString& Parameters)
{
    UFunction* Format = ATsabsPassengerRailcar::StaticClass()->FindFunctionByName(TEXT("FormatBoardingPrompt"));
    if (!TestNotNull(TEXT("Boarding description has a key-aware formatter"), Format)) return false;
    struct FParams { FText UseKey; FText ReturnValue; } Args;
    for (const TCHAR* Key : {TEXT("E"), TEXT("F"), TEXT("Left Alt + Q")})
    {
        Args.UseKey = FText::FromString(Key);
        Args.ReturnValue = FText::GetEmpty();
        GetMutableDefault<ATsabsPassengerRailcar>()->ProcessEvent(Format, &Args);
        TestTrue(TEXT("Actual supplied binding is wrapped in original game key markup"),
            Args.ReturnValue.ToString().Contains(FString::Printf(TEXT("<Key>%s</>"), Key)));
        TestTrue(TEXT("Description still identifies the passenger action"),
            Args.ReturnValue.ToString().Contains(TEXT("montar como pasajero")));
    }
    return true;
}
#endif
