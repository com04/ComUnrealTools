﻿// Copyright com04 All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class ComUnrealToolsEd : ModuleRules
{
	public ComUnrealToolsEd(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"ComUnrealToolsEd/Private",
			}
			);
		string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);
		PrivateIncludePaths.Add(EnginePath + "Source/Editor/WorkspaceMenuStructure/Public");
		PrivateIncludePaths.Add(EnginePath + "Source/Editor/MaterialEditor/Public");
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"ApplicationCore",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"MaterialEditor",
				"TextureEditor",
				"PropertyEditor",
				"EditorStyle",
				"DesktopPlatform",
				"MainFrame",
				"RenderCore",
				"AppFramework",
				"DeveloperSettings",
				"LevelSequence",
				"MovieScene",
				"LevelSequenceEditor",
				"ComUnrealTools",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
