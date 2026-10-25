using UnrealBuildTool;

public class TsabsPassengerWagon : ModuleRules
{
    public TsabsPassengerWagon(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "DeveloperSettings", "PhysicsCore",
            "InputCore", "EnhancedInput", "GeometryCollectionEngine", "AnimGraphRuntime",
            "AssetRegistry", "NavigationSystem", "AIModule", "GameplayTasks", "SlateCore",
            "Slate", "UMG", "RenderCore", "CinematicCamera", "Foliage", "NetCore",
            "GameplayTags", "Json", "JsonUtilities", "DummyHeaders", "FactoryGame", "SML"
        });
        // Base train assets transitively reference the server-manager widgets.
        // Retain their native module before any mod CDO can load those assets.
        PrivateDependencyModuleNames.Add("FactoryDedicatedClient");
        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PublicDependencyModuleNames.Add("AnimGraph");
        }
    }
}
