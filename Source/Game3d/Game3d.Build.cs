// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Game3d : ModuleRules
{
	public Game3d(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Game3d",
			"Game3d/Variant_Platforming",
			"Game3d/Variant_Platforming/Animation",
			"Game3d/Variant_Combat",
			"Game3d/Variant_Combat/AI",
			"Game3d/Variant_Combat/Animation",
			"Game3d/Variant_Combat/Gameplay",
			"Game3d/Variant_Combat/Interfaces",
			"Game3d/Variant_Combat/UI",
			"Game3d/Variant_SideScrolling",
			"Game3d/Variant_SideScrolling/AI",
			"Game3d/Variant_SideScrolling/Gameplay",
			"Game3d/Variant_SideScrolling/Interfaces",
			"Game3d/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
