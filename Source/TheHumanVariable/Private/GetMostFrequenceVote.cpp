#include "GetMostFrequenceVote.h"

void UGetMostFrequenceVote::GetIntArrayMode(
    const TArray<int32>& Votes,
    bool& OnlyOne,
    TArray<int32>& Modes,
    int32& VoteCount)
{
    TMap<int32, int32> FrequencyMap;
    Modes.Empty();
    VoteCount = 0;

    // Compter les fréquences
    for (int32 Vote : Votes)
    {
        FrequencyMap.FindOrAdd(Vote)++;
    }

    // Trouver la fréquence maximale
    for (const auto& Elem : FrequencyMap)
    {
        if (Elem.Value > VoteCount)
        {
            VoteCount = Elem.Value;
        }
    }

    // Récupérer toutes les valeurs ayant cette fréquence max
    for (const auto& Elem : FrequencyMap)
    {
        if (Elem.Value == VoteCount)
        {
            Modes.Add(Elem.Key);
        }
    }

    OnlyOne = (Modes.Num() == 1);
}
