// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Eclipse : ModuleRules
{
	public Eclipse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
            "MotionWarping",
            "GameplayTags",
            "Chooser",
            "StructUtils",
            "AnimGraphRuntime"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Eclipse",
			"Eclipse/Variant_Platforming",
			"Eclipse/Variant_Platforming/Animation",
			"Eclipse/Variant_Combat",
			"Eclipse/Variant_Combat/AI",
			"Eclipse/Variant_Combat/Animation",
			"Eclipse/Variant_Combat/Gameplay",
			"Eclipse/Variant_Combat/Interfaces",
			"Eclipse/Variant_Combat/UI",
			"Eclipse/Variant_SideScrolling",
			"Eclipse/Variant_SideScrolling/AI",
			"Eclipse/Variant_SideScrolling/Gameplay",
			"Eclipse/Variant_SideScrolling/Interfaces",
			"Eclipse/Variant_SideScrolling/UI",
            "Eclipse/Character",
            "Eclipse/Character/Enemy",
            "Eclipse/Character/Player",
            "Eclipse/Interface",
            "Eclipse/AI"
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
