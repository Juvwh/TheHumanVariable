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

    // Compter les fr�quences
    for (int32 Vote : Votes)
    {
        FrequencyMap.FindOrAdd(Vote)++;
    }

    // Trouver la fr�quence maximale
    for (const auto& Elem : FrequencyMap)
    {
        if (Elem.Value > VoteCount)
        {
            VoteCount = Elem.Value;
        }
    }

    // R�cup�rer toutes les valeurs ayant cette fr�quence max
    for (const auto& Elem : FrequencyMap)
    {
        if (Elem.Value == VoteCount)
        {
            Modes.Add(Elem.Key);
        }
    }

    OnlyOne = (Modes.Num() == 1);
}
