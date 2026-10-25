#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "TsabsPassengerRailcar.h"
#include "TsabsPassengerAnchor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsPassengerNativeExitBoundsTest,
    "Tsabs.Passenger.Boarding.NativeExitBoundsFollowRailcar",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTsabsPassengerNativeExitBoundsTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::EditorPreview, false);
    GEngine->CreateNewWorldContext(EWorldType::EditorPreview).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        World->DestroyWorld(false);
        GEngine->DestroyWorldContext(World);
    };

    const FVector RailcarLocation(40000.0, 70000.0, 9000.0);
    ATsabsPassengerRailcar* Railcar = World->SpawnActor<ATsabsPassengerRailcar>();
    ATsabsPassengerAnchor* Anchor = World->SpawnActor<ATsabsPassengerAnchor>();
    if (!TestNotNull(TEXT("Railcar fixture exists"), Railcar) ||
        !TestNotNull(TEXT("Anchor fixture exists"), Anchor)) return false;

    // A real registered collision primitive, NOT placeholder vanilla geometry.
    UBoxComponent* Chassis = NewObject<UBoxComponent>(Railcar);
    Railcar->AddInstanceComponent(Chassis);
    Chassis->SetupAttachment(Railcar->GetRootComponent());
    Chassis->SetBoxExtent(FVector(2000.0, 150.0, 200.0));
    Chassis->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Chassis->RegisterComponent();
    Chassis->SetRelativeLocation(FVector(0.0, 0.0, 200.0));
    Railcar->SetActorLocation(RailcarLocation);
    if (!TestTrue(TEXT("Empty pilot is attached to its own railcar"),
        Anchor->InitializePassengerAnchor(Railcar, FTransform::Identity))) return false;

    const FBox VehicleBounds = Railcar->GetComponentsBoundingBox(false, false);
    if (!TestTrue(TEXT("Railcar has actual collision bounds"), VehicleBounds.IsValid != 0)) return false;
    const FBox AnchorBounds = Anchor->GetComponentsBoundingBox(false, false);
    TestTrue(TEXT("Invisible pilot supplies valid chassis bounds to the native exit query"), AnchorBounds.IsValid != 0);

    FVector NativeOrigin, NativeExtent;
    // Exact engine route used by the installed GetSafeExitLocation implementation.
    Anchor->GetActorBounds(true, NativeOrigin, NativeExtent, false);
    TestTrue(TEXT("Native exit sweep begins at the vehicle, never world origin"),
        NativeOrigin.Equals(VehicleBounds.GetCenter(), 0.01));
    TestTrue(TEXT("Native exit query sees the actual chassis extents"),
        NativeExtent.Equals(VehicleBounds.GetExtent(), 0.01));
    TestFalse(TEXT("Bounds adaptation must not enable physical collision on the pilot"), Anchor->GetActorEnableCollision());
    return true;
}
#endif
