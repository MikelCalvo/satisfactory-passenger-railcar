#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedPlayerInput.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "TsabsPassengerAnchor.h"
#include "TsabsPassengerHUD.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsResolvedExitKeyTest,
    "Tsabs.Passenger.Interface.ExitReadsEffectiveLeaveBindingNotGenericIcon",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTsabsResolvedExitKeyTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::EditorPreview, false);
    GEngine->CreateNewWorldContext(EWorldType::EditorPreview).SetCurrentWorld(World);
    World->SetGameInstance(NewObject<UGameInstance>(GEngine));
    ON_SCOPE_EXIT { World->DestroyWorld(false); GEngine->DestroyWorldContext(World); };
    APlayerController* PC = World->SpawnActor<APlayerController>();
    PC->SetPlayer(NewObject<ULocalPlayer>(GEngine));
    World->AddController(PC);
    PC->SetPawn(World->SpawnActor<ATsabsPassengerAnchor>());
    UEnhancedPlayerInput* Input = NewObject<UEnhancedPlayerInput>(PC);
    PC->PlayerInput = Input;
    UTsabsPassengerHUD* HUD = CreateWidget<UTsabsPassengerHUD>(PC, UTsabsPassengerHUD::StaticClass());
    if (!TestNotNull(TEXT("Presentation instance"), HUD)) return false;
    UInputAction* Leave = LoadObject<UInputAction>(nullptr,
        TEXT("/Game/FactoryGame/Inputs/Vehicles/Actions/IA_Vehicles_Leave.IA_Vehicles_Leave"));
    UInputAction* Other = NewObject<UInputAction>();
    // Populate the non-const fixture's effective mapping array directly. This
    // verifies presentation reads; it does NOT claim to simulate a key event,
    // user-profile rebuild, or the game's input-processing pipeline.
    TArray<FEnhancedActionKeyMapping>& Mappings = const_cast<TArray<FEnhancedActionKeyMapping>&>(Input->GetEnhancedActionMappings());
    Mappings.Emplace(Other, EKeys::X);
    Mappings.Emplace(Leave, EKeys::Gamepad_FaceButton_Bottom);
    Mappings.Emplace(Leave, EKeys::F);
    TestEqual(TEXT("Keyboard shows its actual leave mapping, not another action or controller icon"),
        HUD->ResolveLeaveKey().ToString(), FString(TEXT("F")));
    Mappings.Last().Key = EKeys::E;
    TestEqual(TEXT("Changed effective binding is reflected without a stale F or fixed key"),
        HUD->ResolveLeaveKey().ToString(), FString(TEXT("E")));
    Mappings.Reset();
    TestTrue(TEXT("No binding never invents E or a generic X"), HUD->ResolveLeaveKey().IsEmpty());
    return true;
}
#endif
