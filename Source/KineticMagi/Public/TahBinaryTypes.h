#pragma once

#include "CoreMinimal.h"
#include "TahBinaryTypes.generated.h"

USTRUCT(BlueprintType)
struct FSocialInteractionPayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TAH|Network")
	uint32 SourceAgentId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TAH|Network")
	uint32 TargetAgentId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TAH|Network")
	uint16 InteractionType = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TAH|Network")
	float Intensity = 0.0f;
};

struct FTahHeader
{
	uint16 Magic = 0;
	uint16 PayloadType = 0;
	uint32 Length = 0;
	uint32 Checksum = 0;
};

namespace TahBinary
{
	static constexpr uint16 MagicTA = 0x5441;
	static constexpr uint16 PayloadTypeSocialInteraction = 101;
	static constexpr int32 HeaderSizeBytes = 12;
	static constexpr int32 SocialPayloadSizeBytes = 14;

	inline uint16 ReadU16LE(const uint8* Data)
	{
		return static_cast<uint16>(Data[0]) | (static_cast<uint16>(Data[1]) << 8);
	}

	inline uint32 ReadU32LE(const uint8* Data)
	{
		return static_cast<uint32>(Data[0])
			| (static_cast<uint32>(Data[1]) << 8)
			| (static_cast<uint32>(Data[2]) << 16)
			| (static_cast<uint32>(Data[3]) << 24);
	}

	inline float ReadF32LE(const uint8* Data)
	{
		const uint32 Raw = ReadU32LE(Data);
		float Out = 0.0f;
		FMemory::Memcpy(&Out, &Raw, sizeof(float));
		return Out;
	}

	inline uint32 ComputeFnv1a32(const uint8* Data, const int32 NumBytes)
	{
		uint32 Hash = 2166136261u;
		for (int32 i = 0; i < NumBytes; ++i)
		{
			Hash ^= Data[i];
			Hash *= 16777619u;
		}
		return Hash;
	}

	inline bool TryParseHeader(const TArray<uint8>& RawBuffer, FTahHeader& OutHeader)
	{
		if (RawBuffer.Num() < HeaderSizeBytes)
		{
			return false;
		}

		const uint8* Base = RawBuffer.GetData();
		OutHeader.Magic = ReadU16LE(Base + 0);
		OutHeader.PayloadType = ReadU16LE(Base + 2);
		OutHeader.Length = ReadU32LE(Base + 4);
		OutHeader.Checksum = ReadU32LE(Base + 8);
		return true;
	}

	inline bool TryParseSocialPayload(const TArray<uint8>& RawBuffer, FSocialInteractionPayload& OutPayload)
	{
		FTahHeader Header;
		if (!TryParseHeader(RawBuffer, Header))
		{
			return false;
		}

		if (Header.Magic != MagicTA)
		{
			return false;
		}

		if (Header.PayloadType != PayloadTypeSocialInteraction)
		{
			return false;
		}

		if (Header.Length != SocialPayloadSizeBytes)
		{
			return false;
		}

		if (Header.Length > static_cast<uint32>(MAX_int32))
		{
			return false;
		}

		const int32 PayloadLength = static_cast<int32>(Header.Length);
		if (HeaderSizeBytes > RawBuffer.Num() - PayloadLength)
		{
			return false;
		}

		const uint8* PayloadPtr = RawBuffer.GetData() + HeaderSizeBytes;
		const uint32 ComputedChecksum = ComputeFnv1a32(PayloadPtr, PayloadLength);
		if (ComputedChecksum != Header.Checksum)
		{
			return false;
		}

		OutPayload.SourceAgentId = ReadU32LE(PayloadPtr + 0);
		OutPayload.TargetAgentId = ReadU32LE(PayloadPtr + 4);
		OutPayload.InteractionType = ReadU16LE(PayloadPtr + 8);
		OutPayload.Intensity = ReadF32LE(PayloadPtr + 10);
		return true;
	}
}
