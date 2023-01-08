// Copyright 2020 Jorge CR. All Rights Reserved.

#include "FileScanner.h"
#include <HAL/FileManager.h>
#include <Async/Async.h>
#include <Misc/Paths.h>

FFileScanner::FFileScanner(FOnFileUpdated InOnFileUpdated)
{
	OnFileUpdated = InOnFileUpdated;
	Stopping = false;
	Thread = FRunnableThread::Create(this, TEXT("FFileScanner"), 8 * 1024, TPri_Normal);
}

FFileScanner::~FFileScanner()
{
	if (Thread != nullptr)
	{
		Thread->Kill(true);
		delete Thread;
	}
}

bool FFileScanner::Init()
{	
	return true;
}

uint32 FFileScanner::Run()
{
	while (!Stopping)
	{
		for (auto& map : files)
		{
			if (FPaths::FileExists(map.Key))
			{
				FDateTime  TimeStamp = IFileManager::Get().GetStatData(*map.Key).ModificationTime;
				if (TimeStamp > map.Value)
				{
					map.Value = TimeStamp;
					FString File = map.Key;
					AsyncTask(ENamedThreads::GameThread, [this, File]()
					{
						OnFileUpdated.ExecuteIfBound(File);
						UE_LOG(LogTemp, Verbose, TEXT("File updated"));
					});
				}
			}
		}
		FPlatformProcess::Sleep(3.0f);
	}
	return 1;
}

void FFileScanner::Stop()
{
	Stopping = true;
}

void FFileScanner::Exit()
{

}

void FFileScanner::RegisterFile(const FString& File)
{
	if (!files.Contains(File))
	{
		files.Add(File, FDateTime());
	}
}
