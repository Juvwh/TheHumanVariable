#include "SubtitleUtils.h"

void USubtitleUtils::DivideTranscriptionByWords(
    float Duration,
    const FString& Transcription,
    int32 NmbWord,
    TArray<FString>& DividedTranscript,
    float& TimeByInterval
)
{
    DividedTranscript.Empty();
    TimeByInterval = 0.0f;

    if (Duration <= 0.f || NmbWord <= 0 || Transcription.IsEmpty())
    {
        return;
    }

    // Séparer les mots (en utilisant les espaces)
    TArray<FString> Words;
    Transcription.ParseIntoArray(Words, TEXT(" "), true);

    int32 TotalWords = Words.Num();
    int32 NumChunks = FMath::CeilToInt((float)TotalWords / (float)NmbWord);

    if (NumChunks <= 0)
    {
        return;
    }

    TimeByInterval = Duration / (float)NumChunks;

    for (int32 ChunkIndex = 0; ChunkIndex < NumChunks; ++ChunkIndex)
    {
        FString Chunk;
        for (int32 WordIndex = 0; WordIndex < NmbWord; ++WordIndex)
        {
            int32 GlobalIndex = ChunkIndex * NmbWord + WordIndex;
            if (GlobalIndex >= TotalWords)
            {
                break;
            }

            if (!Chunk.IsEmpty())
            {
                Chunk += TEXT(" ");
            }
            Chunk += Words[GlobalIndex];
        }
        DividedTranscript.Add(Chunk);
    }
}
