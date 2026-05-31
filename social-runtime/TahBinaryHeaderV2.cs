// Copyright (c) TahWorld. All rights reserved.

using System;
using System.Buffers.Binary;
using System.Runtime.InteropServices;

namespace TahWorld;

/// <summary>
/// Wire header for binary TAH payload packets.
/// Keep this layout stable; parse with TahBinaryHeaderV2Codec for safety.
/// </summary>
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct TahBinaryHeaderV2
{
    /// <summary>
    /// 0x54414821 = ASCII "TAH!" in little-endian byte order.
    /// </summary>
    public uint Magic;

    /// <summary>
    /// Schema version of this header. Current: 2.
    /// </summary>
    public ushort Version;

    /// <summary>
    /// Size of this header in bytes. Used for forward compatibility.
    /// </summary>
    public ushort HeaderSize;

    /// <summary>
    /// Bit flags. Bit 0 indicates little-endian wire format.
    /// </summary>
    public ushort Flags;

    /// <summary>
    /// Social payload type discriminator.
    /// </summary>
    public ushort PayloadType;

    /// <summary>
    /// Checksum algorithm discriminator.
    /// </summary>
    public ushort ChecksumType;

    /// <summary>
    /// Reserved for future use.
    /// </summary>
    public ushort Reserved;

    /// <summary>
    /// Length of payload bytes that follow this header.
    /// </summary>
    public uint DataLength;

    /// <summary>
    /// 32-bit checksum of payload bytes using ChecksumType.
    /// </summary>
    public uint Checksum;
}

public enum SocialPayloadType : ushort
{
    AgentInteraction = 101,
    FactionScoreUpdate = 102,
    ProactiveHookTrigger = 103
}

public enum TahChecksumType : ushort
{
    None = 0,
    CityHash64Truncated32 = 1,
    MurmurHash3_x86_32 = 2
}

public static class TahBinaryHeaderV2Constants
{
    public const uint MagicTahExclamation = 0x54414821;
    public const ushort Version = 2;
    public const ushort FlagLittleEndian = 1 << 0;
    public const int HeaderSize = 24;
}

/// <summary>
/// Safe parser/validator and writer for TahBinaryHeaderV2.
/// Avoid direct struct casts for portability and validation control.
/// </summary>
public static class TahBinaryHeaderV2Codec
{
    public static TahBinaryHeaderV2 Create(
        SocialPayloadType payloadType,
        ReadOnlySpan<byte> payload,
        TahChecksumType checksumType = TahChecksumType.CityHash64Truncated32)
    {
        return new TahBinaryHeaderV2
        {
            Magic = TahBinaryHeaderV2Constants.MagicTahExclamation,
            Version = TahBinaryHeaderV2Constants.Version,
            HeaderSize = TahBinaryHeaderV2Constants.HeaderSize,
            Flags = TahBinaryHeaderV2Constants.FlagLittleEndian,
            PayloadType = (ushort)payloadType,
            ChecksumType = (ushort)checksumType,
            Reserved = 0,
            DataLength = (uint)payload.Length,
            Checksum = ComputeChecksum(payload, checksumType)
        };
    }

    public static bool TryReadHeader(ReadOnlySpan<byte> packet, out TahBinaryHeaderV2 header, out string error)
    {
        header = default;
        error = string.Empty;

        if (packet.Length < TahBinaryHeaderV2Constants.HeaderSize)
        {
            error = $"Packet too small for header. Need at least {TahBinaryHeaderV2Constants.HeaderSize}, got {packet.Length}.";
            return false;
        }

        // Header is always encoded little-endian.
        header.Magic = BinaryPrimitives.ReadUInt32LittleEndian(packet.Slice(0, 4));
        header.Version = BinaryPrimitives.ReadUInt16LittleEndian(packet.Slice(4, 2));
        header.HeaderSize = BinaryPrimitives.ReadUInt16LittleEndian(packet.Slice(6, 2));
        header.Flags = BinaryPrimitives.ReadUInt16LittleEndian(packet.Slice(8, 2));
        header.PayloadType = BinaryPrimitives.ReadUInt16LittleEndian(packet.Slice(10, 2));
        header.ChecksumType = BinaryPrimitives.ReadUInt16LittleEndian(packet.Slice(12, 2));
        header.Reserved = BinaryPrimitives.ReadUInt16LittleEndian(packet.Slice(14, 2));
        header.DataLength = BinaryPrimitives.ReadUInt32LittleEndian(packet.Slice(16, 4));
        header.Checksum = BinaryPrimitives.ReadUInt32LittleEndian(packet.Slice(20, 4));

        if (header.Magic != TahBinaryHeaderV2Constants.MagicTahExclamation)
        {
            error = $"Invalid magic: 0x{header.Magic:X8}.";
            return false;
        }

        if ((header.Flags & TahBinaryHeaderV2Constants.FlagLittleEndian) == 0)
        {
            error = "Unsupported endianness flag. Only little-endian packets are supported.";
            return false;
        }

        if (header.HeaderSize < TahBinaryHeaderV2Constants.HeaderSize)
        {
            error = $"Invalid header size: {header.HeaderSize}.";
            return false;
        }

        if (packet.Length < header.HeaderSize)
        {
            error = $"Packet truncated before full header. Need {header.HeaderSize}, got {packet.Length}.";
            return false;
        }

        return true;
    }

    public static bool TryValidatePacket(
        ReadOnlySpan<byte> packet,
        uint maxPayloadBytes,
        out TahBinaryHeaderV2 header,
        out ReadOnlySpan<byte> payload,
        out string error)
    {
        payload = default;
        if (!TryReadHeader(packet, out header, out error))
        {
            return false;
        }

        if (header.DataLength > maxPayloadBytes)
        {
            error = $"Payload length {header.DataLength} exceeds configured max {maxPayloadBytes}.";
            return false;
        }

        ulong requiredBytes = (ulong)header.HeaderSize + header.DataLength;
        if ((ulong)packet.Length < requiredBytes)
        {
            error = $"Packet truncated for payload. Need {requiredBytes}, got {packet.Length}.";
            return false;
        }

        payload = packet.Slice(header.HeaderSize, (int)header.DataLength);
        TahChecksumType checksumType = (TahChecksumType)header.ChecksumType;
        uint computed = ComputeChecksum(payload, checksumType);

        if (computed != header.Checksum)
        {
            error = $"Checksum mismatch. Expected 0x{header.Checksum:X8}, computed 0x{computed:X8}.";
            return false;
        }

        error = string.Empty;
        return true;
    }

    public static void WriteHeader(Span<byte> destination, in TahBinaryHeaderV2 header)
    {
        if (destination.Length < TahBinaryHeaderV2Constants.HeaderSize)
        {
            throw new ArgumentException($"Destination too small. Need {TahBinaryHeaderV2Constants.HeaderSize} bytes.");
        }

        BinaryPrimitives.WriteUInt32LittleEndian(destination.Slice(0, 4), header.Magic);
        BinaryPrimitives.WriteUInt16LittleEndian(destination.Slice(4, 2), header.Version);
        BinaryPrimitives.WriteUInt16LittleEndian(destination.Slice(6, 2), header.HeaderSize);
        BinaryPrimitives.WriteUInt16LittleEndian(destination.Slice(8, 2), header.Flags);
        BinaryPrimitives.WriteUInt16LittleEndian(destination.Slice(10, 2), header.PayloadType);
        BinaryPrimitives.WriteUInt16LittleEndian(destination.Slice(12, 2), header.ChecksumType);
        BinaryPrimitives.WriteUInt16LittleEndian(destination.Slice(14, 2), header.Reserved);
        BinaryPrimitives.WriteUInt32LittleEndian(destination.Slice(16, 4), header.DataLength);
        BinaryPrimitives.WriteUInt32LittleEndian(destination.Slice(20, 4), header.Checksum);
    }

    private static uint ComputeChecksum(ReadOnlySpan<byte> payload, TahChecksumType checksumType)
    {
        return checksumType switch
        {
            TahChecksumType.None => 0,
            TahChecksumType.CityHash64Truncated32 => (uint)(CityHash.CityHash64(payload) & 0xFFFFFFFF),
            TahChecksumType.MurmurHash3_x86_32 => MurmurHash3_x86_32(payload),
            _ => throw new NotSupportedException($"Unsupported checksum type: {(ushort)checksumType}.")
        };
    }

    // Standard MurmurHash3 x86 32-bit.
    private static uint MurmurHash3_x86_32(ReadOnlySpan<byte> data, uint seed = 0)
    {
        const uint c1 = 0xcc9e2d51;
        const uint c2 = 0x1b873593;

        uint h1 = seed;
        int length = data.Length;
        int roundedEnd = length & ~0x3;

        for (int i = 0; i < roundedEnd; i += 4)
        {
            uint k1 = BinaryPrimitives.ReadUInt32LittleEndian(data.Slice(i, 4));
            k1 *= c1;
            k1 = RotateLeft(k1, 15);
            k1 *= c2;

            h1 ^= k1;
            h1 = RotateLeft(h1, 13);
            h1 = h1 * 5 + 0xe6546b64;
        }

        uint tail = 0;
        switch (length & 3)
        {
            case 3:
                tail ^= (uint)data[roundedEnd + 2] << 16;
                goto case 2;
            case 2:
                tail ^= (uint)data[roundedEnd + 1] << 8;
                goto case 1;
            case 1:
                tail ^= data[roundedEnd];
                tail *= c1;
                tail = RotateLeft(tail, 15);
                tail *= c2;
                h1 ^= tail;
                break;
        }

        h1 ^= (uint)length;
        h1 = Fmix32(h1);
        return h1;
    }

    private static uint RotateLeft(uint x, int r)
    {
        return (x << r) | (x >> (32 - r));
    }

    private static uint Fmix32(uint h)
    {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }
}
