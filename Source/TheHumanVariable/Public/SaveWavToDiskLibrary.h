#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveWavToDiskLibrary.generated.h"

UCLASS()
class THEHUMANVARIABLE_API USaveWavToDiskLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Audio|Save")
	static bool SaveWavToSavedDir(const TArray<uint8>& WavData, const FString& FileName);
};
