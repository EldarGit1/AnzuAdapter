// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;

public class AnzuAdapter : ModuleRules
{
	public void Print(String p)
    {
        Console.WriteLine("AnzuAdapter :: " + p);
    }

	public AnzuAdapter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		if(File.Exists(Path.Combine(PluginDirectory, "Source", "anzuSDK", "libs", "windows", "anzu.dll")))
		{
			Print("Found Anzu DLL");
		}
		else
		{
			Print("Anzu DLL not found!");
		}
		String pathToLibs = Path.Combine(PluginDirectory, "Source", "anzuSDK", "libs", "windows");
        RuntimeDependencies.Add(Path.Combine(pathToLibs, "anzu.dll"));
        //Link against .lib
        PublicAdditionalLibraries.Add(Path.Combine(pathToLibs, "anzu.lib"));
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Private", "Core"),
				Path.Combine(ModuleDirectory, "Private", "Core", "Log"),
				Path.Combine(ModuleDirectory, "Private", "Core", "Render"),
				Path.Combine(ModuleDirectory, "Private", "Core", "Types"),
				Path.Combine(ModuleDirectory, "Private", "Core", "Utilities"),
				Path.Combine(ModuleDirectory, ".."),
				Path.Combine(ModuleDirectory, "Private", "Engine"),

				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"AnzuAdapterLibrary",
				"Projects",
				"CoreUObject",
				"Engine"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
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
