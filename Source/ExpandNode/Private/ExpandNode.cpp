// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExpandNode.h"

#include "FileScanner.h"
#include "MaterialExpressionCustomReader.h"

#define LOCTEXT_NAMESPACE "FExpandNodeModule"

void FExpandNodeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	#if WITH_EDITOR
	FOnFileUpdated OnFileUpdated = FOnFileUpdated::CreateRaw(this, &FExpandNodeModule::OnFileUpdated);
	FileScanner = new FFileScanner(OnFileUpdated);	
	#endif
	/*FString PluginShaderDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
	if(!FPaths::DirectoryExists(PluginShaderDir))
	{
		IFileManager::Get().MakeDirectory(*PluginShaderDir);
	}
	AddShaderSourceDirectoryMapping(TEXT("/Shaders"), PluginShaderDir);*/
}

void FExpandNodeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	#if WITH_EDITOR
	if (FileScanner)
	{
		FileScanner->Stop();
	}
#endif
}
#if WITH_EDITOR
FFileScanner* FExpandNodeModule::FileScanner = nullptr;

FFileScanner*  FExpandNodeModule::GetFileScanner()
{
	return FileScanner;
}

void FExpandNodeModule::OnFileUpdated(const FString& File)
{
	FString Code;
	TArray<FString> Variables;
	if (UMaterialExpressionCustomReader::ParseFile(File, Code, Variables))
	{
		for (TObjectIterator<UMaterialExpressionCustomReader> It; It; ++It)
		{
			if (!It->IsPendingKill() && File == It->File.FilePath)
			{
				It->UpdateNode(Code, Variables);
			}
		}
	}	
}
#endif

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExpandNodeModule, ExpandNode)