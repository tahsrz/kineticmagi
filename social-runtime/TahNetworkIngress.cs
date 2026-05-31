// Copyright (c) TahWorld. All rights reserved.

using System;
using System.Buffers.Binary;
using System.Collections.Generic;

namespace TahWorld;

/// <summary>
/// Validates and routes incoming TAH social packets using the v2 header codec.
/// </summary>
public static class TahNetworkIngress
{
    public const uint DefaultMaxPayloadSize = 64 * 1024;

    private static readonly Dictionary<SocialPayloadType, int> ExpectedPayloadSizes = new()
    {
        [SocialPayloadType.AgentInteraction] = SocialInteractionPayload.WireSize
    };

    public static bool TryProcessRawPacket(
        ReadOnlySpan<byte> packet,
        SocialEngine socialEngine,
        out string error)
    {
        if (socialEngine is null)
        {
            error = "Social engine is null.";
            return false;
        }

        if (!TahBinaryHeaderV2Codec.TryValidatePacket(
            packet,
            DefaultMaxPayloadSize,
            out TahBinaryHeaderV2 header,
            out ReadOnlySpan<byte> payload,
            out error))
        {
            return false;
        }

        SocialPayloadType payloadType = (SocialPayloadType)header.PayloadType;
        if (!ExpectedPayloadSizes.TryGetValue(payloadType, out int expectedSize))
        {
            error = $"Unsupported payload type: {(ushort)payloadType}.";
            return false;
        }

        if (payload.Length != expectedSize)
        {
            error = $"Invalid payload size for {payloadType}. Expected {expectedSize}, got {payload.Length}.";
            return false;
        }

        switch (payloadType)
        {
            case SocialPayloadType.AgentInteraction:
                if (!TryReadAgentInteractionPayload(payload, out SocialInteractionPayload interaction, out error))
                {
                    return false;
                }

                if (!socialEngine.ProcessInteraction(interaction))
                {
                    error = "Social interaction rejected by semantic validation.";
                    return false;
                }

                error = string.Empty;
                return true;

            default:
                error = $"No handler registered for payload type {(ushort)payloadType}.";
                return false;
        }
    }

    public static bool TryBuildAgentInteractionPacket(
        in SocialInteractionPayload payload,
        Span<byte> destination,
        out int bytesWritten,
        out string error)
    {
        bytesWritten = 0;
        error = string.Empty;

        Span<byte> payloadBuffer = stackalloc byte[SocialInteractionPayload.WireSize];
        WriteAgentInteractionPayload(payloadBuffer, payload);
        TahBinaryHeaderV2 header = TahBinaryHeaderV2Codec.Create(SocialPayloadType.AgentInteraction, payloadBuffer);

        int totalBytes = TahBinaryHeaderV2Constants.HeaderSize + SocialInteractionPayload.WireSize;
        if (destination.Length < totalBytes)
        {
            error = $"Destination too small. Need {totalBytes}, got {destination.Length}.";
            return false;
        }

        TahBinaryHeaderV2Codec.WriteHeader(destination.Slice(0, TahBinaryHeaderV2Constants.HeaderSize), header);
        payloadBuffer.CopyTo(destination.Slice(TahBinaryHeaderV2Constants.HeaderSize, SocialInteractionPayload.WireSize));
        bytesWritten = totalBytes;
        return true;
    }

    private static bool TryReadAgentInteractionPayload(
        ReadOnlySpan<byte> bytes,
        out SocialInteractionPayload payload,
        out string error)
    {
        payload = default;
        error = string.Empty;

        if (bytes.Length != SocialInteractionPayload.WireSize)
        {
            error = $"AgentInteraction payload must be {SocialInteractionPayload.WireSize} bytes.";
            return false;
        }

        uint sourceAgentId = BinaryPrimitives.ReadUInt32LittleEndian(bytes.Slice(0, 4));
        uint targetAgentId = BinaryPrimitives.ReadUInt32LittleEndian(bytes.Slice(4, 4));
        ushort interactionType = BinaryPrimitives.ReadUInt16LittleEndian(bytes.Slice(8, 2));

        int intensityBits = BinaryPrimitives.ReadInt32LittleEndian(bytes.Slice(10, 4));
        float intensity = BitConverter.Int32BitsToSingle(intensityBits);
        if (!float.IsFinite(intensity))
        {
            error = "AgentInteraction intensity is not finite.";
            return false;
        }

        payload = new SocialInteractionPayload(sourceAgentId, targetAgentId, interactionType, intensity);
        return true;
    }

    private static void WriteAgentInteractionPayload(Span<byte> destination, in SocialInteractionPayload payload)
    {
        BinaryPrimitives.WriteUInt32LittleEndian(destination.Slice(0, 4), payload.SourceAgentId);
        BinaryPrimitives.WriteUInt32LittleEndian(destination.Slice(4, 4), payload.TargetAgentId);
        BinaryPrimitives.WriteUInt16LittleEndian(destination.Slice(8, 2), payload.InteractionType);
        BinaryPrimitives.WriteInt32LittleEndian(destination.Slice(10, 4), BitConverter.SingleToInt32Bits(payload.Intensity));
    }
}
