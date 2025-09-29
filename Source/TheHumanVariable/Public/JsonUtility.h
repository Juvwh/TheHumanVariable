// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonUtility.generated.h"

/**
 * 
 */
UCLASS()
class THEHUMANVARIABLE_API UJsonUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

    UFUNCTION(BlueprintCallable, Category = "JSON")
    static bool ExtractFieldsFromJson(
        const FString& JsonString,
        const TArray<FString>& FieldsToExtract,
        TArray<FString>& OutValues
    );
};