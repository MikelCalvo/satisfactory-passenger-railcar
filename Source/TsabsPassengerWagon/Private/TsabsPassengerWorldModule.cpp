#include "TsabsPassengerWorldModule.h"
#include "PassengerContentPolicy.h"
#include "TsabsPassengerRecipe.h"
#include "TsabsPassengerWagonLog.h"
#include "Engine/World.h"
#include "FGRecipeManager.h"
#include "Registry/ModContentRegistry.h"

UTsabsPassengerWorldModule::UTsabsPassengerWorldModule()
{
    bRootModule = true;
}

void UTsabsPassengerWorldModule::DispatchLifecycleEvent(ELifecyclePhase Phase)
{
    UWorld* World = GetWorld();
    const FName Owner = GetOwnerModReference();
    const bool bValidWorld = IsValid(World) && World->IsGameWorld();
    const bool bValidOwner = Owner == FName(TEXT("TsabsPassengerWagon"));
    // The base implementation dereferences the world. Validate before Super.
    if (!bValidWorld || !bValidOwner)
    {
        UE_LOG(LogTsabsPassengerWagon, Error, TEXT("TSABS_CONTENT invalid-context phase=%d world=%d owner=%s"),
            static_cast<int32>(Phase), bValidWorld, *Owner.ToString());
        return;
    }
    Super::DispatchLifecycleEvent(Phase);
    const TSubclassOf<UFGRecipe> Recipe = UTsabsPassengerRecipe::StaticClass();
    if (TsabsPassenger::ShouldRegisterPassengerRecipe(Phase, bValidWorld, bValidOwner, ContentState.bRegistrationAttempted))
    {
        UModContentRegistry* Registry = UModContentRegistry::Get(World);
        if (!IsValid(Registry))
        {
            UE_LOG(LogTsabsPassengerWagon, Error, TEXT("TSABS_CONTENT registration missing-registry"));
            return;
        }
        ContentState.bRegistrationAttempted = true;
        Registry->RegisterRecipe(Owner, Recipe);
        FGameObjectRegistration Registration;
        ContentState.bRegistrationVerified = Registry->GetRecipeInfo(Recipe, Registration) &&
            Registration.RegisteredObject == Recipe.Get() && Registration.OwnedByModReference == Owner &&
            !Registration.HasAnyFlags(EGameObjectRegistrationFlags::Removed | EGameObjectRegistrationFlags::Unregistered);
        UE_LOG(LogTsabsPassengerWagon, Display, TEXT("TSABS_CONTENT registration verified=%d owner=%s recipe=%s netmode=%d"),
            ContentState.bRegistrationVerified, *Owner.ToString(), *GetNameSafe(Recipe.Get()), static_cast<int32>(World->GetNetMode()));
        if (!ContentState.bRegistrationVerified)
        {
            UE_LOG(LogTsabsPassengerWagon, Error, TEXT("TSABS_CONTENT registration readback-failed"));
        }
    }
    if (Phase != ELifecyclePhase::POST_INITIALIZATION || World->GetNetMode() == NM_Client || ContentState.bAvailabilityAttempted)
    {
        return;
    }
    AFGRecipeManager* Manager = AFGRecipeManager::Get(World);
    const bool bAuthority = IsValid(Manager) && Manager->HasAuthority();
    if (!TsabsPassenger::ShouldGrantPassengerRecipe(Phase, bValidWorld, false, bAuthority,
        ContentState.bRegistrationVerified, ContentState.bAvailabilityAttempted))
    {
        UE_LOG(LogTsabsPassengerWagon, Error, TEXT("TSABS_CONTENT availability blocked registered=%d manager-authority=%d"),
            ContentState.bRegistrationVerified, bAuthority);
        return;
    }
    ContentState.bAvailabilityAttempted = true;
    if (!Manager->IsRecipeAvailable(Recipe))
    {
        Manager->AddAvailableRecipe(Recipe);
    }
    // Verify the effect instead of treating a void API call as success.
    // This readback still requires exercising in a real game, not a CDO.
    ContentState.bAvailabilityVerified = Manager->IsRecipeAvailable(Recipe);
    UE_LOG(LogTsabsPassengerWagon, Display, TEXT("TSABS_CONTENT availability verified=%d recipe=%s netmode=%d"),
        ContentState.bAvailabilityVerified, *GetNameSafe(Recipe.Get()), static_cast<int32>(World->GetNetMode()));
    if (!ContentState.bAvailabilityVerified)
    {
        UE_LOG(LogTsabsPassengerWagon, Error, TEXT("TSABS_CONTENT availability readback-failed"));
    }
}

