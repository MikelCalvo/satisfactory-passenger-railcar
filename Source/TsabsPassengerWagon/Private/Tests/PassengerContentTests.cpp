#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UObject/UnrealType.h"
#include "Resources/FGVehicleDescriptor.h"
#include "FGBuildCategory.h"
#include "Engine/Texture2D.h"
#include "TsabsPassengerRailcar.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTsabsDescriptorTest, "Tsabs.Passenger.Content.NativeDescriptor", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FTsabsDescriptorTest::RunTest(const FString& Parameters)
{
    UClass* Class = LoadClass<UFGVehicleDescriptor>(nullptr, TEXT("/Script/TsabsPassengerWagon.TsabsPassengerDescriptor"));
    if (!TestNotNull(TEXT("Concrete native passenger vehicle descriptor exists"), Class)) return false;
    TestTrue(TEXT("Native concrete descriptor"), Class->HasAnyClassFlags(CLASS_Native) && !Class->HasAnyClassFlags(CLASS_Abstract));
    const UFGVehicleDescriptor* Descriptor = Class->GetDefaultObject<UFGVehicleDescriptor>();
    TestEqual(TEXT("Describes existing passenger car, not freight"), Descriptor->mVehicleClass.Get(), ATsabsPassengerRailcar::StaticClass());
    TestEqual(TEXT("Name uses real virtual instance accessor"), Descriptor->GetItemNameFromInstance().ToString(), FString(TEXT("Vagón de pasajeros (prueba)")));
    TestFalse(TEXT("Description is explicit"), Descriptor->GetItemDescriptionFromInstance().IsEmpty());
    UClass* Category = Descriptor->GetCategoryFromInstance();
    TestNotNull(TEXT("Category resolves"), Category);
    if (Category) {
        TestTrue(TEXT("Build category type"), Category->IsChildOf(UFGBuildCategory::StaticClass()));
        TestEqual(TEXT("Existing vehicles category"), Category->GetPathName(), FString(TEXT("/Game/FactoryGame/Interface/UI/InGame/BuildMenu/BuildCategories/BC_Vehicles.BC_Vehicles_C")));
    }
    for (const FName Name : {FName(TEXT("mSmallIcon")), FName(TEXT("mPersistentBigIcon"))}) {
        const FObjectPropertyBase* Property = FindFProperty<FObjectPropertyBase>(Class, Name);
        if (TestNotNull(TEXT("Icon property exists"), Property)) {
            UObject* Icon = Property->GetObjectPropertyValue_InContainer(Descriptor);
            TestNotNull(TEXT("Icon resolves"), Icon);
            TestTrue(TEXT("Icon is texture, not placeholder default copy"), Icon && Icon->IsA<UTexture2D>());
        }
    }
    return true;
}
#endif
