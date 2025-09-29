#include "SaveWavToDiskLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool USaveWavToDiskLibrary::SaveWavToSavedDir(const TArray<uint8>& WavData, const FString& FileName)
{
	if (WavData.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("WAV data is empty."));
		return false;
	}

	// Génère le chemin complet vers le fichier
	const FString FullPath = FPaths::Combine(FPaths::ProjectSavedDir(), FileName + TEXT(".wav"));

	// Sauvegarde le fichier
	if (FFileHelper::SaveArrayToFile(WavData, *FullPath))
	{
		UE_LOG(LogTemp, Log, TEXT("WAV saved to: %s"), *FullPath);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save WAV to: %s"), *FullPath);
		return false;
	}
}
