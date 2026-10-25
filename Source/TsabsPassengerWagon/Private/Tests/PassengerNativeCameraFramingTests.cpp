#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "GameFramework/SpringArmComponent.h"
#include "TsabsPassengerSpringArm.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsNativeCameraFramingTest,
    "Tsabs.Passenger.Anchor.ReusesLocomotiveCameraFramingOnly",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTsabsNativeCameraFramingTest::RunTest(const FString& Parameters)
{
    UFunction* Copy = UTsabsPassengerSpringArm::StaticClass()->FindFunctionByName(TEXT("CopyLocomotiveFraming"));
    if (!TestNotNull(TEXT("Framing has an explicit native-template copy seam"), Copy)) return false;
    USpringArmComponent* Source = NewObject<USpringArmComponent>();
    UTsabsPassengerSpringArm* Target = NewObject<UTsabsPassengerSpringArm>();
    Source->TargetArmLength = 2345.0f;
    Source->SetRelativeLocation(FVector(10.0, 20.0, 350.0));
    Source->SetRelativeRotation(FRotator(-22.0, 15.0, 0.0));
    Source->TargetOffset = FVector(1, 2, 3);
    Source->SocketOffset = FVector(4, 5, 6);
    Source->bEnableCameraLag = true;
    Source->CameraLagSpeed = 9.0f;
    Source->bUsePawnControlRotation = true;
    Source->bDoCollisionTest = false;
    Target->bUsePawnControlRotation = false;
    Target->bDoCollisionTest = true;
    struct FParams { USpringArmComponent* Source; bool ReturnValue; } Args{Source, false};
    Target->ProcessEvent(Copy, &Args);
    TestTrue(TEXT("Valid source framing is accepted"), Args.ReturnValue);
    TestEqual(TEXT("Original camera distance is reused"), Target->TargetArmLength, Source->TargetArmLength);
    TestTrue(TEXT("Original pivot location is reused"), Target->GetRelativeLocation().Equals(Source->GetRelativeLocation()));
    TestTrue(TEXT("Original default view angle is reused"), Target->GetRelativeRotation().Equals(Source->GetRelativeRotation()));
    TestTrue(TEXT("Original target offset is reused"), Target->TargetOffset.Equals(Source->TargetOffset));
    TestTrue(TEXT("Original socket offset is reused"), Target->SocketOffset.Equals(Source->SocketOffset));
    TestTrue(TEXT("Original camera smoothing is reused"), Target->bEnableCameraLag && Target->CameraLagSpeed == 9.0f);
    TestTrue(TEXT("Own collision policy is not disabled by copying framing"), Target->bDoCollisionTest);
    TestFalse(TEXT("Copying framing does not switch passenger input/control routes"), Target->bUsePawnControlRotation);
    Source->TargetArmLength = 0.0f;
    Args.ReturnValue = true;
    Target->ProcessEvent(Copy, &Args);
    TestFalse(TEXT("An empty placeholder cannot erase valid camera reach"), Args.ReturnValue);
    TestEqual(TEXT("Invalid copy has no partial mutation"), Target->TargetArmLength, 2345.0f);
    return true;
}
#endif
