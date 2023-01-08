// Copyright 2020 Jorge CR. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <HAL/Runnable.h>
#include <HAL/RunnableThread.h>

DECLARE_DELEGATE_OneParam(FOnFileUpdated, const FString&);

/**
 * 
 */
class EXPANDNODE_API FFileScanner : public FRunnable
{

public:
	FFileScanner(FOnFileUpdated InOnFileUpdated);
	~FFileScanner();

public:
	//~ FRunnable
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;


private:

	//~ Hold the files to be scanned
	TMap<FString, FDateTime> files;

	/** Holds the thread object. */
	FRunnableThread* Thread;


	bool Stopping;

private:
	//~ Delegate that triggers when the file has changes
	FOnFileUpdated OnFileUpdated;

public:
	/**
	 * Register a file to see if is having changes once in a while
	 *
	 * @param: File - File to be registered by the file scanner
	*/
	void RegisterFile(const FString& File);
};
