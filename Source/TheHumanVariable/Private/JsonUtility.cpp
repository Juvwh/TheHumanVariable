#include "JsonUtility.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
    // Fonction utilitaire pour nettoyer une cha�ne JSON entour�e de balises Markdown
    bool CleanJsonString(const FString& Input, FString& Cleaned)
    {
        int32 Start, End;
        if (Input.FindChar('{', Start) && Input.FindLastChar('}', End) && End >= Start)
        {
            Cleaned = Input.Mid(Start, End - Start + 1);
            return true;
        }
        return false;
    }
}

bool UJsonUtility::ExtractFieldsFromJson(
    const FString& JsonString,
    const TArray<FString>& FieldsToExtract,
    TArray<FString>& OutValues
)
{
    OutValues.Empty();

    FString CleanedJson;
    if (!CleanJsonString(JsonString, CleanedJson))
    {
        // JSON invalide ou mal form� (pas d'accolades d�tect�es)
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(CleanedJson);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        // �chec de la d�s�rialisation
        return false;
    }

    for (const FString& Field : FieldsToExtract)
    {
        if (!JsonObject->HasField(Field))
        {
            // Champ manquant
            return false;
        }

        const TSharedPtr<FJsonValue> Value = JsonObject->TryGetField(Field);
        if (!Value.IsValid())
        {
            return false;
        }

        FString Converted;
        switch (Value->Type)
        {
        case EJson::String:
            Converted = Value->AsString();
            break;
        case EJson::Boolean:
            Converted = Value->AsBool() ? TEXT("true") : TEXT("false");
            break;
        case EJson::Number:
            Converted = FString::SanitizeFloat(Value->AsNumber());
            break;
        default:
            // Types non g�r�s : objet, tableau, null
            return false;
        }

        OutValues.Add(Converted);
    }

    return true;
}
