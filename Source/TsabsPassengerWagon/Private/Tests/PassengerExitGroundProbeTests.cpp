#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "PassengerExitGeometry.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsPassengerNativeGroundProbeTest,
    "Tsabs.Passenger.Boarding.NativeExitProbeReachesFlatGround",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTsabsPassengerNativeGroundProbeTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::EditorPreview, false);
    GEngine->CreateNewWorldContext(EWorldType::EditorPreview).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        World->DestroyWorld(false);
        GEngine->DestroyWorldContext(World);
    };
    const FVector Origin(30000.0, 40000.0, 5000.0);
    auto MakeBox = [&](const FVector& Location, const FVector& Extent)
    {
        AActor* Actor = World->SpawnActor<AActor>();
        UBoxComponent* Box = NewObject<UBoxComponent>(Actor);
        Actor->SetRootComponent(Box);
        Box->SetBoxExtent(Extent);
        Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        Box->SetCollisionResponseToAllChannels(ECR_Block);
        Box->RegisterComponent();
        Actor->SetActorLocation(Location);
        return Box;
    };
    // Chassis dimensions and capsule size observed in the failing 0.1.1 trial.
    const FBox LocalBody(FVector(-716.733, -245.405, 152.924), FVector(716.733, 245.408, 322.007));
    UBoxComponent* Chassis = MakeBox(Origin + LocalBody.GetCenter(), LocalBody.GetExtent());
    UBoxComponent* Floor = MakeBox(Origin + FVector(0, 0, -25), FVector(5000, 5000, 25));
    AActor* Anchor = World->SpawnActor<AActor>();
    AActor* Driver = World->SpawnActor<AActor>();
    UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>(Driver);
    Capsule->InitCapsuleSize(40.0f, 96.0f);
    const FCollisionShape Shape = Capsule->GetCollisionShape(0.1f);
    FCollisionQueryParams Query(SCENE_QUERY_STAT(TsabsExitGroundRegression), false);
    Query.AddIgnoredActor(Anchor);
    Query.AddIgnoredActor(Driver);

    // Replay of the COLLISION QUERY sequence observed in the installed native
    // GetSafeExitLocation (RVA 0x78A1C0): this is real engine physics, not a mock
    // of that SDK stub and not evidence of an actual player entering/leaving.
    auto NativeQuerySequenceAccepts = [&](FVector Offset)
    {
        for (int32 Side = 0; Side < 2; ++Side)
        {
            if (Side == 1) Offset.Y = -Offset.Y;
            const FVector Destination = Origin + Offset;
            FHitResult Lateral;
            World->SweepSingleByChannel(Lateral, Chassis->Bounds.Origin, Destination,
                FQuat::Identity, ECC_Pawn, Shape, Query);
            if (!Lateral.bBlockingHit) return true;
            FHitResult Down;
            World->SweepSingleByChannel(Down, Destination + FVector(0, 0, 300), Destination,
                FQuat::Identity, ECC_Pawn, Shape, Query);
            // The native fallback accepts a non-initial contact; NO hit is not
            // accepted by this branch. A target above the floor can stop too soon.
            if (Down.bBlockingHit && Down.Time > 0.0f) return true;
        }
        return false;
    };
    FVector Offset;
    if (!TestTrue(TEXT("Production geometry makes a lateral probe"),
        TsabsPassenger::TryMakeExitOffset(LocalBody, 40.0, 96.0, Offset))) return false;
    TestTrue(TEXT("Native-compatible query accepts a clear exit on flat ground"), NativeQuerySequenceAccepts(Offset));
    Floor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    TestFalse(TEXT("Missing ground still fails when the chassis blocks the lateral sweep"), NativeQuerySequenceAccepts(Offset));
    return true;
}
#endif
