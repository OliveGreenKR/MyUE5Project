// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define LOG_LOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))
#define LOG_REMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))
#define LOG_NETMODEINFO ((GetNetMode() == ENetMode::NM_Client) ?  *FString::Printf(TEXT("CLIENT%d"), static_cast<int32>(GPlayInEditorID))  : \
						( (GetNetMode() == ENetMode::NM_Standalone) ? *FString::Printf(TEXT("STANDALONE")) : *FString::Printf(TEXT("SERVER")) ) )

#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)

#define AB_LOG(LogCategory, Verbosity, Format , ... ) \
				UE_LOG(LogCategory,Verbosity,TEXT("[%s][%s/%s] %40s %s"),LOG_NETMODEINFO, LOG_LOCALROLEINFO,LOG_REMOTEROLEINFO, \
																					 LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(LogABNetwork, Log, All);