#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TsabsPassengerAnchor.h"
#include "TsabsPassengerRailcar.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsPassengerCameraOwnChassisTest,
    "Tsabs.Passenger.Anchor.CameraClearsOwnChassisButBlocksWorld",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTsabsPassengerCameraOwnChassisTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::EditorPreview, false);
    GEngine->CreateNewWorldContext(EWorldType::EditorPreview).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        World->DestroyWorld(false);
        GEngine->DestroyWorldContext(World);
    };
    const FVector Origin(30000.0, 40000.0, 5000.0);
    ATsabsPassengerRailcar* Car = World->SpawnActor<ATsabsPassengerRailcar>();
    ATsabsPassengerAnchor* Anchor = World->SpawnActor<ATsabsPassengerAnchor>();
    if (!TestNotNull(TEXT("Own car fixture"), Car) | !TestNotNull(TEXT("Own camera anchor"), Anchor)) return false;
    Car->SetActorLocation(Origin);
    auto MakeBox = [&](AActor* Owner, const FVector& Location, const FVector& Extent)
    {
        UBoxComponent* Box = NewObject<UBoxComponent>(Owner);
        Owner->AddInstanceComponent(Box);
        Box->SetBoxExtent(Extent);
        Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        Box->SetCollisionResponseToAllChannels(ECR_Block);
        Box->RegisterComponent();
        Box->SetWorldLocation(Location);
        return Box;
    };
    // The real 0.1.2 chassis straddles the provisional arm origin Z=300.
    const FBox Body(FVector(-716.733, -245.405, 152.924), FVector(716.733, 245.408, 322.007));
    MakeBox(Car, Origin + Body.GetCenter(), Body.GetExtent());
    if (!TestTrue(TEXT("Attach real production anchor to own railcar"),
        Anchor->InitializePassengerAnchor(Car, FTransform::Identity))) return false;
    USpringArmComponent* Arm = Anchor->FindComponentByClass<USpringArmComponent>();
    if (!TestNotNull(TEXT("Production orbit arm"), Arm)) return false;
    // Isolate collision from whichever real locomotive framing/smoothing is loaded.
    Arm->TargetArmLength = 1600.0f;
    Arm->SetRelativeLocation(FVector(0.0, 0.0, 300.0));
    Arm->SetRelativeRotation(FRotator(-15.0, 0.0, 0.0));
    Arm->TargetOffset = FVector::ZeroVector;
    Arm->SocketOffset = FVector::ZeroVector;
    Arm->bEnableCameraLag = false;
    Arm->bEnableCameraRotationLag = false;
    auto ViewLocation = [&]()
    {
        Arm->TickComponent(1.0f / 60.0f, LEVELTICK_All, &Arm->PrimaryComponentTick);
        FMinimalViewInfo View;
        Anchor->CalcCamera(1.0f / 60.0f, View);
        return View.Location;
    };
    const FVector Pivot = Arm->GetComponentLocation() + Arm->TargetOffset;
    const FVector ClearLocation = ViewLocation();
    const double ClearDistance = FVector::Distance(ClearLocation, Pivot);
    AddInfo(FString::Printf(TEXT("Own chassis camera distance %.3f, desired arm %.3f"), ClearDistance, Arm->TargetArmLength));
    TestTrue(TEXT("Own railcar must not retract its passenger camera into the chassis"),
        ClearDistance > Arm->TargetArmLength * 0.9);
    AActor* Wall = World->SpawnActor<AActor>();
    const FVector Desired = Arm->GetUnfixedCameraPosition();
    MakeBox(Wall, (Desired + Pivot) * 0.5, FVector(100.0, 500.0, 500.0));
    const double BlockedDistance = FVector::Distance(ViewLocation(), Pivot);
    TestTrue(TEXT("An external wall still retracts the exterior camera"),
        BlockedDistance < Arm->TargetArmLength * 0.75);
    return true;
}
#endif
