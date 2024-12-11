// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)

#define AB_LOG(LogCategory, Verbosity, Format , ... ) \
				UE_LOG(LogCategory,Verbosity,TEXT("%-40s %s"),LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(LogABNetwork, Log, All);