#include "PromptBuilderLibrary.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

FString UPromptBuilderLibrary::BuildPromptFromInputs(
    const FString& Question,
    const FString& A,
    const FString& B,
    const FString& C,
    const FString& D,
    const FString& E,
    int32 Index)
{
    TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();

    if (!Question.IsEmpty())
    {
        JsonObject->SetStringField(TEXT("Question"), Question);
    }

    // Tableau temporaire pour simplifier l'accès
    TArray<FString> Keys = { TEXT("A"), TEXT("B"), TEXT("C"), TEXT("D"), TEXT("E") };
    TArray<FString> Values = { A, B, C, D, E };

    for (int32 i = 0; i < 5; ++i)
    {
        if (i == Index)
        {
            continue; // On saute l'entrée correspondant à l'index exclu
        }

        if (!Values[i].IsEmpty())
        {
            JsonObject->SetStringField(Keys[i], Values[i]);
        }
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject, Writer);

    return OutputString;
}
