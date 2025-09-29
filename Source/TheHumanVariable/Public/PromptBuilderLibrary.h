#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PromptBuilderLibrary.generated.h"

UCLASS()
class THEHUMANVARIABLE_API UPromptBuilderLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "Prompt")
    static FString BuildPromptFromInputs(
        const FString& Question,
        const FString& A,
        const FString& B,
        const FString& C,
        const FString& D,
        const FString& E,
        int32 Index
    );
};
