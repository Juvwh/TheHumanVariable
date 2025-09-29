#include "WavUtilsLibrary.h"

float UWavUtilsLibrary::GetWavDurationFromBuffer(const TArray<uint8>& WavData)
{
    if (WavData.Num() < 44)
        return -1.0f;

    const uint8* Data = WavData.GetData();

    if (FMemory::Memcmp(Data, "RIFF", 4) != 0 || FMemory::Memcmp(Data + 8, "WAVE", 4) != 0)
        return -1.0f;

    int32 NumChannels = 0;
    int32 SampleRate = 0;
    int32 DataSize = 0;

    int32 Cursor = 12;

    while (Cursor + 8 <= WavData.Num())
    {
        FName ChunkId = FName((const ANSICHAR*)(Data + Cursor), 4);
        uint32 ChunkSize = *(const uint32*)(Data + Cursor + 4);
        Cursor += 8;

        if (ChunkId == "fmt ")
        {
            if (ChunkSize < 16 || Cursor + static_cast<int32>(ChunkSize) > WavData.Num())
                return -1.0f;


            NumChannels = *(const uint16*)(Data + Cursor + 2);
            SampleRate = *(const uint32*)(Data + Cursor + 4);
        }
        else if (ChunkId == "data")
        {
            DataSize = ChunkSize;
            break;
        }

        Cursor += ChunkSize;
    }

    if (SampleRate == 0 || NumChannels == 0 || DataSize == 0)
        return -1.0f;

    int32 NumSamples = DataSize / (NumChannels * 2); // 16-bit PCM = 2 bytes per sample
    return static_cast<float>(NumSamples) / static_cast<float>(SampleRate);
}
