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
			}
			);
		string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);
		
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
				"CoreUObject",
				"Engine",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
		
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd"
				}
				);
		}
	}
}
