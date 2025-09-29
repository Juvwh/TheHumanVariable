#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SendPromptToLLM.generated.h"

UENUM(BlueprintType)
enum class ESendGroqResultText : uint8
{
    Success     UMETA(DisplayName = "Success"),
    Failure     UMETA(DisplayName = "Failure")
};

UENUM(BlueprintType)
enum class ELLMProvider : uint8
{
    Groq_Llama      UMETA(DisplayName = "Groq - LLaMA3"),
    Groq_Gemma      UMETA(DisplayName = "Groq - Gemma"),
    Mistral         UMETA(DisplayName = "Mistral"),
    OpenAI_GPT      UMETA(DisplayName = "OpenAI - GPT-4")
};

UCLASS()
class THEHUMANVARIABLE_API USendPromptToLLM : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", Latent, LatentInfo = "LatentInfo", ExpandEnumAsExecs = "Result"), Category = "LLM")
    static void SendPromptToLLM(
        UObject* WorldContextObject,
        FLatentActionInfo LatentInfo,
        const FString& Prompt,
        const FString& Job,
        ELLMProvider Provider,
        FString& ResponseOut,
        ESendGroqResultText& Result
    );
};
