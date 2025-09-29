// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SubtitleUtils.generated.h"

/**
 * 
 */
UCLASS()
class THEHUMANVARIABLE_API USubtitleUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

    UFUNCTION(BlueprintCallable, Category = "Subtitles")
    static void DivideTranscriptionByWords(
        float Duration,
        const FString& Transcription,
        int32 NmbWord,
        TArray<FString>& DividedTranscript,
        float& TimeByInterval
    );
};
