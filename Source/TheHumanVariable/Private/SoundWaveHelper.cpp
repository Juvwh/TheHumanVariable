#include "SoundWaveHelper.h"
#include "AudioDevice.h"
#include "Sound/SoundWaveProcedural.h"


// Utilitaire simple pour ignorer l'en-tête WAV et extraire les données PCM (WAV 16-bit mono ou stéréo)
static bool ParseWavHeader(const TArray<uint8>& Data, int32& SampleRate, int32& NumChannels, TArray<uint8>& PCMData)
{
	if (Data.Num() < 44) return false;

	// Extraction des métadonnées audio
	SampleRate = *(int32*)&Data[24];
	NumChannels = *(int16*)&Data[22];
	int32 BitsPerSample = *(int16*)&Data[34];

	if (BitsPerSample != 16)
	{
		UE_LOG(LogTemp, Error, TEXT("Unsupported BitsPerSample: %d"), BitsPerSample);
		return false;
	}

	// Données PCM
	int32 PCMOffset = 44;
	int32 PCMLength = Data.Num() - PCMOffset;
	if (PCMLength <= 0) return false;

	PCMData.Append(&Data[PCMOffset], PCMLength);
	return true;
}

USoundWaveProcedural* USoundWaveHelper::CreateProceduralSoundWaveFromWav(const TArray<uint8>& WavData)
{
	int32 SampleRate = 0;
	int32 NumChannels = 0;
	TArray<uint8> PCMData;

	if (!ParseWavHeader(WavData, SampleRate, NumChannels, PCMData))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse WAV header."));
		return nullptr;
	}

	// On ignore SampleRate et NumChannels car on ne peut pas les modifier directement
	// Ils seront gérés automatiquement par la lecture dynamique

	USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>();
	SoundWave->SetSampleRate(16000); // Optionnel : dépend du format Groq

	SoundWave->QueueAudio(PCMData.GetData(), PCMData.Num());

	return SoundWave;
}

