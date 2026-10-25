#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UObject/UnrealType.h"
#include "FGRecipe.h"
#include "FGRecipeProducerInterface.h"
#include "Equipment/FGBuildGun.h"
#include "TsabsPassengerDescriptor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsFreeRecipeTest, "Tsabs.Passenger.Content.FreeBuildGunRecipe", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTsabsFreeRecipeTest::RunTest(const FString& Parameters)
{
    UClass* Class = LoadClass<UFGRecipe>(nullptr, TEXT("/Script/TsabsPassengerWagon.TsabsPassengerRecipe"));
    if (!TestNotNull(TEXT("Native passenger recipe exists"), Class)) return false;
    TestTrue(TEXT("Concrete native recipe"), Class->HasAnyClassFlags(CLASS_Native) && !Class->HasAnyClassFlags(CLASS_Abstract));
    const UFGRecipe* Recipe = Class->GetDefaultObject<UFGRecipe>();
    TestTrue(TEXT("Genuinely zero ingredients"), Recipe->GetIngredients().IsEmpty());
    const auto& Products = Recipe->GetProducts();
    if (TestEqual(TEXT("One product"), Products.Num(), 1)) {
        TestEqual(TEXT("Product is passenger descriptor"), Products[0].ItemClass.Get(), UTsabsPassengerDescriptor::StaticClass());
        TestEqual(TEXT("One wagon"), Products[0].Amount, 1);
    }
    TArray<TSubclassOf<UObject>> Producers;
    Recipe->GetProducedIn(Producers); // Real soft-class resolving implementation; never IsProducedIn stub.
    if (TestEqual(TEXT("One resolved producer"), Producers.Num(), 1)) {
        UClass* Producer = Producers[0].Get();
        TestNotNull(TEXT("Resolved producer"), Producer);
        TestTrue(TEXT("BuildGun subclass"), Producer && Producer->IsChildOf(AFGBuildGun::StaticClass()));
        TestTrue(TEXT("Recipe producer interface"), Producer && Producer->ImplementsInterface(UFGRecipeProducerInterface::StaticClass()));
        if (Producer) TestEqual(TEXT("Exact existing BuildGun"), Producer->GetPathName(), FString(TEXT("/Game/FactoryGame/Equipment/BuildGun/BP_BuildGun.BP_BuildGun_C")));
    }
    const FArrayProperty* ProducedIn = FindFProperty<FArrayProperty>(Class, TEXT("mProducedIn"));
    if (TestNotNull(TEXT("Soft class array"), ProducedIn)) {
        TestNotNull(TEXT("Array holds soft classes, not hard subclasses"), CastField<FSoftClassProperty>(ProducedIn->Inner));
        FScriptArrayHelper Entries(ProducedIn, ProducedIn->ContainerPtrToValuePtr<void>(Recipe));
        TestEqual(TEXT("Exactly one raw reference (no silently dropped invalid entries)"), Entries.Num(), 1);
    }
    TestEqual(TEXT("Explicit technical duration"), Recipe->GetManufacturingDuration(), 1.0f);
    TestEqual(TEXT("Explicit manual multiplier"), Recipe->GetManualManufacturingDuration(), 1.0f);
    const FArrayProperty* Events = FindFProperty<FArrayProperty>(Class, TEXT("mRelevantEvents"));
    if (TestNotNull(TEXT("Event array"), Events)) TestEqual(TEXT("No seasonal gate"), FScriptArrayHelper(Events, Events->ContainerPtrToValuePtr<void>(Recipe)).Num(), 0);
    const FObjectPropertyBase* Custom = FindFProperty<FObjectPropertyBase>(Class, TEXT("mMaterialCustomizationRecipe"));
    if (TestNotNull(TEXT("Customization field"), Custom)) TestNull(TEXT("No extra material cost recipe"), Custom->GetObjectPropertyValue_InContainer(Recipe));
    return true;
}
#endif
