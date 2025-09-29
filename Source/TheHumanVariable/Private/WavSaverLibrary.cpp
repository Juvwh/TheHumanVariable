// Fill out your copyright notice in the Description page of Project Settings.


#include "WavSaverLibrary.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"

bool UWavSaverLibrary::SaveWav(const FString& FilePath, const TArray<float>& Samples, int32 SampleRate, int32 NumChannels)
{
    TArray<uint8> WavData;

    // Convert float samples [-1, 1] to int16
    TArray<int16> Int16Samples;
    Int16Samples.Reserve(Samples.Num());
    for (float Sample : Samples)
    {
        Int16Samples.Add(FMath::Clamp(Sample * 32767.0f, -32768.0f, 32767.0f));
    }

    int32 DataSize = Int16Samples.Num() * sizeof(int16);
    int32 FileSize = 44 + DataSize;

    // Write WAV header
    WavData.Append((uint8*)"RIFF", 4);
    int32 ChunkSize = FileSize - 8;
    WavData.Append((uint8*)&ChunkSize, 4);
    WavData.Append((uint8*)"WAVE", 4);
    WavData.Append((uint8*)"fmt ", 4);

    int32 Subchunk1Size = 16;
    int16 AudioFormat = 1;
    WavData.Append((uint8*)&Subchunk1Size, 4);
    WavData.Append((uint8*)&AudioFormat, 2);
    WavData.Append((uint8*)&NumChannels, 2);
    WavData.Append((uint8*)&SampleRate, 4);

    int32 ByteRate = SampleRate * NumChannels * sizeof(int16);
    int16 BlockAlign = NumChannels * sizeof(int16);
    int16 BitsPerSample = 16;
    WavData.Append((uint8*)&ByteRate, 4);
    WavData.Append((uint8*)&BlockAlign, 2);
    WavData.Append((uint8*)&BitsPerSample, 2);

    WavData.Append((uint8*)"data", 4);
    WavData.Append((uint8*)&DataSize, 4);
    WavData.Append((uint8*)Int16Samples.GetData(), DataSize);

    return FFileHelper::SaveArrayToFile(WavData, *FilePath);
}
