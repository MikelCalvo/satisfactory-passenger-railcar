#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "ModLoading/PluginModuleLoader.h"
#include "Module/GameWorldModule.h"
#include "Engine/World.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsRootDiscoveryTest, "Tsabs.Passenger.Content.SingleNativeWorldRoot", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTsabsRootDiscoveryTest::RunTest(const FString& Parameters)
{
    UClass* Class = LoadClass<UGameWorldModule>(nullptr, TEXT("/Script/TsabsPassengerWagon.TsabsPassengerWorldModule"));
    if (!TestNotNull(TEXT("Native game-world root exists"), Class)) return false;
    TestTrue(TEXT("Native concrete current class"), Class->HasAnyClassFlags(CLASS_Native) && !Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists));
    const UGameWorldModule* CDO = Class->GetDefaultObject<UGameWorldModule>();
    TestTrue(TEXT("Root flag"), CDO->bRootModule);
    TestTrue(TEXT("No alternate schematic progression"), CDO->mSchematics.IsEmpty());
    TestTrue(TEXT("No changes to vanilla research"), CDO->mResearchTrees.IsEmpty());
    TestNull(TEXT("CDO has no runtime world"), CDO->GetWorld());
    TestTrue(TEXT("CDO does not claim runtime owner"), CDO->GetOwnerModReference().IsNone());
    int32 Count = 0;
    for (const FDiscoveredModule& Module : FPluginModuleLoader::FindRootModulesOfType(UGameWorldModule::StaticClass())) {
        if (Module.OwnerPluginName == TEXT("TsabsPassengerWagon")) {
            ++Count;
            TestEqual(TEXT("SML resolves exact native class to this plugin"), Module.ModuleClass.Get(), Class);
        }
    }
    TestEqual(TEXT("Exactly one GameWorld root for plugin"), Count, 1);
    UWorld* World = UWorld::CreateWorld(EWorldType::EditorPreview, false);
    TestFalse(TEXT("Editor discovery is NOT automatic lifecycle execution"), FPluginModuleLoader::ShouldLoadModulesForWorld(World));
    World->DestroyWorld(false);
    return true;
}
#endif
