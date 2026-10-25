#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "TsabsPassengerRailcar.h"
#include "TsabsPassengerRailMovement.h"
#include "FGFreightWagon.h"
#include "FGInventoryComponent.h"
#include "Components/SkeletalMeshComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsIndependentRailcarTest,
    "Tsabs.Passenger.IndependentRailcar",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTsabsIndependentRailcarTest::RunTest(const FString& Parameters)
{
    ATsabsPassengerRailcar* Car = GetMutableDefault<ATsabsPassengerRailcar>();
    TestFalse(TEXT("Passenger car must not inherit cargo behavior"),
        Car->IsA(AFGFreightWagon::StaticClass()));
    TestNotNull(TEXT("Passenger car has its own rail movement"), Car->GetRailroadVehicleMovementComponent());
    TestTrue(TEXT("Pawn and rail getters refer to the same movement"),
        Car->GetMovementComponent() == Car->GetRailroadVehicleMovementComponent());
    TestNotNull(TEXT("Vehicle chassis component exists"), Car->GetPassengerMesh());
    TestNull(TEXT("No cargo inventory component is created"), Car->FindComponentByClass<UFGInventoryComponent>());
    TestFalse(TEXT("The car itself never accepts a driver"), Car->CanDriverEnter(nullptr));
    TestFalse(TEXT("The car itself cannot be possessed by boarding"), Car->DriverEnter(nullptr));
    TestTrue(TEXT("Railcar length is configured"), Car->GetLength() > 0.0f);
    const UTsabsPassengerRailMovement* Movement = Cast<UTsabsPassengerRailMovement>(Car->GetRailroadVehicleMovementComponent());
    TestNotNull(TEXT("Uses the passenger movement setup adapter"), Movement);
    if (Movement)
    {
        TestTrue(TEXT("Editable setup was obtained by reference"), Movement->HasCopiedFreightSetup());
        TestEqual(TEXT("Front and rear wheelsets exist"), Movement->GetConfiguredWheelsetCount(), 2);
        TestEqual(TEXT("Front and rear couplers exist"), Movement->GetConfiguredCouplerCount(), 2);
    }
    return true;
}
#endif
