// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFileScanner;

class FExpandNodeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
#if WITH_EDITOR
private:
	//~ File scanner class module singleton
	static FFileScanner* FileScanner;

public:
	//~ Get Reference to this singleton class
	static FFileScanner* GetFileScanner();

private:
	/**
	 * Triggers when a file has new changes
	 *
	 * @param: File - The file containing the new changes
	*/
	void OnFileUpdated(const FString& File);
#endif
};
