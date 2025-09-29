// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WavSaverLibrary.generated.h"

UCLASS()
class THEHUMANVARIABLE_API UWavSaverLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "Audio")
    static bool SaveWav(const FString& FilePath, const TArray<float>& Samples, int32 SampleRate = 44100, int32 NumChannels = 1);
};
