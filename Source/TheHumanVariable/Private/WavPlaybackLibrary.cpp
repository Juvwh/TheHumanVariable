#include "WavPlaybackLibrary.h"
#include "LatentActions.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundSubmix.h"
#include "Components/AudioComponent.h"

class FPlayWavLatentAction : public FPendingLatentAction
{
public:
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;
    UAudioComponent* AudioComponent;

    FPlayWavLatentAction(UAudioComponent* InAudioComponent, const FLatentActionInfo& LatentInfo)
        : ExecutionFunction(LatentInfo.ExecutionFunction),
        OutputLink(LatentInfo.Linkage),
        CallbackTarget(LatentInfo.CallbackTarget),
        AudioComponent(InAudioComponent)
    {
    }

    virtual void UpdateOperation(FLatentResponse& Response) override
    {
        const bool bFinished = !AudioComponent || !AudioComponent->IsPlaying();
        Response.FinishAndTriggerIf(bFinished, ExecutionFunction, OutputLink, CallbackTarget);
    }
};
void UWavPlaybackLibrary::PlayWavFromBuffer(UObject* WorldContextObject,
    const TArray<uint8>& WavData,
    float& OutDuration,
    USoundWaveProcedural*& OutSoundWave,
    FLatentActionInfo LatentInfo)
{
    OutDuration = -1.f;
    OutSoundWave = nullptr;

    if (!WorldContextObject || WavData.Num() < 44)
    {
        UE_LOG(LogTemp, Warning, TEXT("WavData invalide ou trop court."));
        return;
    }

    // === PARSING DU HEADER WAV PCM 16 bits ===
    struct FWavHeader
    {
        uint8 ChunkID[4];      // "RIFF"
        uint32 ChunkSize;
        uint8 Format[4];       // "WAVE"
        uint8 Subchunk1ID[4];  // "fmt "
        uint32 Subchunk1Size;
        uint16 AudioFormat;
        uint16 NumChannels;
        uint32 SampleRate;
        uint32 ByteRate;
        uint16 BlockAlign;
        uint16 BitsPerSample;
        uint8 Subchunk2ID[4];  // "data"
        uint32 Subchunk2Size;
    };

    if (WavData.Num() < sizeof(FWavHeader))
    {
        UE_LOG(LogTemp, Warning, TEXT("Fichier WAV trop petit pour contenir un header valide."));
        return;
    }

    const FWavHeader* Header = reinterpret_cast<const FWavHeader*>(WavData.GetData());

    if (Header->AudioFormat != 1 || Header->BitsPerSample != 16)
    {
        UE_LOG(LogTemp, Warning, TEXT("Format WAV non pris en charge (PCM 16 bits requis)."));
        return;
    }

    const int32 SampleRate = static_cast<int32>(Header->SampleRate);
    const int32 NumChannels = static_cast<int32>(Header->NumChannels);
    const int32 BitsPerSample = static_cast<int32>(Header->BitsPerSample);
    const int32 DataOffset = 44; // Simplifié : on suppose header standard
    const int32 AudioDataSize = WavData.Num() - DataOffset;

    if (AudioDataSize <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Pas de données audio dans le fichier WAV."));
        return;
    }

    const uint8* PCMData = WavData.GetData() + DataOffset;

    const int32 BytesPerSample = BitsPerSample / 8;
    const int32 BytesPerFrame = NumChannels * BytesPerSample;
    const float DurationSeconds = static_cast<float>(AudioDataSize) / (SampleRate * BytesPerFrame);
    OutDuration = DurationSeconds;

    // === CRÉATION DU SOUNDWAVE PROCEDURAL ===
    USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>();
    SoundWave->NumChannels = NumChannels;
    SoundWave->Duration = INDEFINITELY_LOOPING_DURATION;
    SoundWave->SetSampleRate(SampleRate);
    SoundWave->QueueAudio(PCMData, AudioDataSize);
    OutSoundWave = SoundWave;

    // === LECTURE DU SON ===
    UAudioComponent* AudioComponent = UGameplayStatics::SpawnSound2D(WorldContextObject, SoundWave);
    if (!AudioComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Échec lors du SpawnSound2D."));
        return;
    }

    // === AJOUT DE LA REVERB ===
    USoundSubmix* ReverbSubmix = LoadObject<USoundSubmix>(nullptr, TEXT("/Game/Sound/Reverb/RE_LargeHall.RE_LargeHall"));
    if (ReverbSubmix)
    {
        if (USoundSubmixBase* SubmixBase = Cast<USoundSubmixBase>(ReverbSubmix))
        {
            AudioComponent->SetSubmixSend(SubmixBase, 1.0f);
        }
    }

    // === LANCEMENT ===
    AudioComponent->Play();

    // === ACTION LATENTE : FIN DE LECTURE ===
    if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
    {
        FLatentActionManager& LatentManager = World->GetLatentActionManager();
        LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID,
            new FPlayWavLatentAction(AudioComponent, LatentInfo));
    }
}

