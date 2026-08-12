// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Opia_P : ModuleRules
{
	public Opia_P(ReadOnlyTargetRules Target) : base(Target)
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
			"Opia_P",
			"Opia_P/Variant_Platforming",
			"Opia_P/Variant_Platforming/Animation",
			"Opia_P/Variant_Combat",
			"Opia_P/Variant_Combat/AI",
			"Opia_P/Variant_Combat/Animation",
			"Opia_P/Variant_Combat/Gameplay",
			"Opia_P/Variant_Combat/Interfaces",
			"Opia_P/Variant_Combat/UI",
			"Opia_P/Variant_SideScrolling",
			"Opia_P/Variant_SideScrolling/AI",
			"Opia_P/Variant_SideScrolling/Gameplay",
			"Opia_P/Variant_SideScrolling/Interfaces",
			"Opia_P/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
