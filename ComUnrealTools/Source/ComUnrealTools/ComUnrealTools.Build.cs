// Copyright com04 All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class ComUnrealTools : ModuleRules
{
	public ComUnrealTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"ComUnrealTools/Private",
				// ... add other private include paths required here ...
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
				// ... add other public dependencies that you statically link with here ...
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
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
