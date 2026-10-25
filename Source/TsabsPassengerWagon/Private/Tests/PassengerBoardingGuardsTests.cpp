#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "TsabsPassengerRailcar.h"
#include "TsabsPassengerAnchor.h"
#include "FGCharacterPlayer.h"
#include "FGPlayerController.h"
#include "FGHealthComponent.h"
#include "FGRailroadVehicleMovementComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/UnrealType.h"
#include "Components/SkeletalMeshComponent.h"
#include <bit>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsBoardingGuardsTest,
    "Tsabs.Passenger.Boarding.ServerIdentityAndFeedback",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTsabsBoardingGuardsTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::EditorPreview, false);
    GEngine->CreateNewWorldContext(EWorldType::EditorPreview).SetCurrentWorld(World);
    ATsabsPassengerRailcar* Car = World->SpawnActor<ATsabsPassengerRailcar>();
    Car->EnsurePilotAnchor();
    AFGCharacterPlayer* Character = World->SpawnActor<AFGCharacterPlayer>();
    AFGPlayerController* PC = World->SpawnActor<AFGPlayerController>();
    // Reflected ENGINE pointer data only, NOT Possess(), a connected player,
    // a native CanDriverEnter success, or a mock of any Super implementation.
    FObjectProperty* ControllerProperty = FindFProperty<FObjectProperty>(APawn::StaticClass(), TEXT("Controller"));
    FObjectProperty* PawnProperty = FindFProperty<FObjectProperty>(AController::StaticClass(), TEXT("Pawn"));
    FObjectProperty* DriverProperty = FindFProperty<FObjectProperty>(AFGDriveablePawn::StaticClass(), TEXT("mDriver"));
    if (!TestNotNull(TEXT("Character fixture"), Character) | !TestNotNull(TEXT("Controller fixture"), PC) |
        !TestNotNull(TEXT("Engine controller reference"), ControllerProperty) | !TestNotNull(TEXT("Engine pawn reference"), PawnProperty))
    {
        World->DestroyWorld(false);
        GEngine->DestroyWorldContext(World);
        return false;
    }
    TestFalse(TEXT("Character without controller is not eligible"), Car->IsBoardingCharacterValid(Character));
    ControllerProperty->SetObjectPropertyValue_InContainer(Character, PC);
    TestFalse(TEXT("One-way controller relation is not eligible"), Car->IsBoardingCharacterValid(Character));
    PawnProperty->SetObjectPropertyValue_InContainer(PC, Character);
    TestTrue(TEXT("Consistent authority-side identity DATA satisfies own identity predicate only"), Car->IsBoardingCharacterValid(Character));
    // Reflected selection DATA only. The installed game's server Tick computes
    // this non-replicated field; this test does not fake native trace execution.
    FObjectProperty* BestActorProperty = FindFProperty<FObjectProperty>(AFGCharacterPlayer::StaticClass(), TEXT("mBestUsableActor"));
    if (TestNotNull(TEXT("Native best-usable actor selection field"), BestActorProperty))
    {
        BestActorProperty->SetObjectPropertyValue_InContainer(Character, Car);
        TestTrue(TEXT("Native selected railcar is not rejected by a second geometry policy"), Car->CanReachBoardingBody(Character));
        BestActorProperty->SetObjectPropertyValue_InContainer(Character, Character);
        TestFalse(TEXT("Native selection of another actor does not authorize this railcar"), Car->CanReachBoardingBody(Character));
        BestActorProperty->SetObjectPropertyValue_InContainer(Character, nullptr);
        TestFalse(TEXT("Missing native selection cannot authorize this railcar"), Car->CanReachBoardingBody(Character));
    }
    Character->SetRole(ROLE_SimulatedProxy);
    TestFalse(TEXT("Authority rejects non-authority character data"), Car->IsBoardingCharacterValid(Character));
    Character->SetRole(ROLE_Authority);
    PC->SetRole(ROLE_SimulatedProxy);
    TestFalse(TEXT("Authority rejects non-authority controller data"), Car->IsBoardingCharacterValid(Character));
    PC->SetRole(ROLE_Authority);
    auto* Movement = Car->GetRailroadVehicleMovementComponent();
    const uint32 MovingBits[]{0x44505556u, 0xc4505556u, 0x44610000u, 0xc4610000u, 0x7fc00000u, 0x7f800000u, 0xff800000u};
    for (uint32 Bits : MovingBits)
    {
        Movement->SetForwardSpeed(std::bit_cast<float>(Bits));
        TestTrue(FString::Printf(TEXT("Server feedback rejects speed bits %08x"), Bits), Car->GetBoardingFeedback(Character) == ETsabsBoardingFeedback::Moving);
    }
    for (uint32 Bits : {0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x3f800000u, 0xbf800000u, 0x437a0000u, 0xc37a0000u, 0x43960000u, 0xc3960000u, 0x443b8000u, 0xc43b8000u, 0x44480000u, 0xc4480000u, 0x44505555u, 0xc4505555u})
    {
        Movement->SetForwardSpeed(std::bit_cast<float>(Bits));
        // No physical base-game body in EditorPreview: no invented native success.
        TestTrue(TEXT("Below-limit value is not rejected as too fast"), Car->GetBoardingFeedback(Character) != ETsabsBoardingFeedback::Moving);
    }
    Movement->SetForwardSpeed(0.0f);
    {
        FScopedPassengerBoardingReservation Scope(Car->BoardingReservation, Character);
        TestTrue(TEXT("Actual reserved car reports busy"), Car->GetBoardingFeedback(Character) == ETsabsBoardingFeedback::Busy);
        TestFalse(TEXT("Reserved second use cannot enter"), Car->TryBoardPilot(Character));
        TestNull(TEXT("Reentrant use does not fabricate a driver"), Car->GetPilotAnchor()->GetDriver());
    }
    if (TestNotNull(TEXT("Native driver data"), DriverProperty))
    {
        DriverProperty->SetObjectPropertyValue_InContainer(Car->GetPilotAnchor(), Character);
        TestTrue(TEXT("Native occupant reports busy"), Car->GetBoardingFeedback(Character) == ETsabsBoardingFeedback::Busy);
        DriverProperty->SetObjectPropertyValue_InContainer(Car->GetPilotAnchor(), nullptr);
    }
    FUseState Forged;
    Forged.bIsTraceHit = true;
    Forged.UseComponent = Car->GetPassengerMesh();
    Forged.UseLocation = Character->GetActorLocation();
    Car->SetRole(ROLE_SimulatedProxy);
    Car->OnUse_Implementation(Character, Forged);
    TestFalse(TEXT("Client OnUse cannot acquire a transaction"), Car->BoardingReservation.IsReserved());
    TestNull(TEXT("Client OnUse does not create a driver"), Car->GetPilotAnchor()->GetDriver());
    Car->SetRole(ROLE_Authority);
    ControllerProperty->SetObjectPropertyValue_InContainer(Character, nullptr);
    Car->OnUse_Implementation(Character, Forged);
    TestNull(TEXT("Forged hit never replaces server controller validation"), Car->GetPilotAnchor()->GetDriver());
    PawnProperty->SetObjectPropertyValue_InContainer(PC, nullptr);
    TestFalse(TEXT("Own blocked-state constructor is not usable"), GetDefault<UTsabsPassengerUseState_Blocked>()->IsUsableState());
    World->DestroyWorld(false);
    GEngine->DestroyWorldContext(World);
    return true;
}
#endif
