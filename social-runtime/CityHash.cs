// Copyright (c) TahWorld. All rights reserved.

using System;

namespace TahWorld;

/// <summary>
/// A high-performance 64-bit CityHash implementation in C# with strict 64-bit parity.
/// </summary>
public static class CityHash
{
    private const ulong K0 = 0xc3a5c85c97cb3127UL;
    private const ulong K1 = 0xb492b66fbe98f273UL;
    private const ulong K2 = 0x9ae16a3b2f90404fUL;
    private const ulong KMul = 0x9ddfea08eb382d69UL;

    private static ulong Rotate(ulong val, int shift)
    {
        return shift == 0 ? val : (val >> shift) | (val << (64 - shift));
    }

    private static ulong ShiftMix(ulong val)
    {
        return val ^ (val >> 47);
    }

    private static ulong HashLen16(ulong u, ulong v)
    {
        ulong a = (u ^ v) * KMul;
        a ^= (a >> 47);
        ulong b = (v ^ a) * KMul;
        b ^= (b >> 47);
        b *= KMul;
        return b;
    }

    private static (ulong, ulong) WeakHashLen32WithSeeds(ulong w, ulong x, ulong y, ulong z, ulong a, ulong b)
    {
        a += w;
        b = Rotate(b + a + z, 21);
        ulong c = a;
        a += x;
        a += y;
        b += Rotate(a, 44);
        return (a + z, b + c);
    }

    private static ulong Fetch64(ReadOnlySpan<byte> data, int offset)
    {
        return BitConverter.ToUInt64(data.Slice(offset, 8));
    }

    private static ulong Fetch32(ReadOnlySpan<byte> data, int offset)
    {
        return BitConverter.ToUInt32(data.Slice(offset, 4));
    }

    private static ulong HashLen0to16(ReadOnlySpan<byte> data)
    {
        int len = data.Length;
        if (len > 8)
        {
            ulong a = Fetch64(data, 0);
            ulong b = Fetch64(data, len - 8);
            return HashLen16(a, Rotate(b + (ulong)len, len)) ^ b;
        }
        if (len >= 4)
        {
            ulong a = Fetch32(data, 0);
            return HashLen16((ulong)len + (a << 3), Fetch32(data, len - 4));
        }
        if (len > 0)
        {
            ulong a = data[0];
            ulong b = data[len / 2];
            ulong c = data[len - 1];
            ulong y = a + (b << 8);
            ulong z = (ulong)len + (c << 2);
            return ShiftMix(y * K2 ^ z * K0) * K2;
        }
        return K0;
    }

    public static ulong CityHash64(ReadOnlySpan<byte> data)
    {
        int len = data.Length;
        if (len <= 32)
        {
            if (len <= 16)
            {
                return HashLen0to16(data);
            }
            else
            {
                ulong a = Fetch64(data, 0);
                ulong b = Fetch64(data, 8);
                ulong c = Fetch64(data, len - 8);
                ulong d = Fetch64(data, len - 16);
                return HashLen16(Rotate(a - b, 43) + Rotate(c, 30) + d,
                                 a + Rotate(b ^ K2, 18) + c);
            }
        }
        else if (len <= 64)
        {
            if (len >= 40)
            {
                ulong x = Fetch64(data, len - 40);
                ulong y = Fetch64(data, len - 16) ^ Fetch64(data, len - 24);
                ulong z = Fetch64(data, len - 8);
                ulong v0 = Rotate(y, 33) * K1;
                ulong v1 = Rotate(y + x, 33) * K1;
                ulong w0 = Rotate(z + v0, 35) * K1 + v1;
                ulong w1 = Rotate(x + y, 33) * K1;
                return HashLen16(v0 + v1, w0 + w1);
            }
            else
            {
                ulong a = Fetch64(data, 0);
                ulong b = Fetch64(data, 8);
                ulong c = Fetch64(data, len - 8);
                ulong d = Fetch64(data, len - 16);
                return HashLen16(Rotate(a - b, 43) + Rotate(c, 30) + d,
                                 a + Rotate(b ^ K2, 18) + c);
            }
        }

        ulong x_val = Fetch64(data, 0);
        ulong y_val = Fetch64(data, len - 16) ^ Fetch64(data, len - 32);
        ulong z_val = Fetch64(data, len - 8);
        (ulong v0_val, ulong v1_val) = (Rotate(y_val, 33) * K1, Rotate(y_val + x_val, 33) * K1);
        (ulong w0_val, ulong w1_val) = (Rotate(z_val + v0_val, 35) * K1 + v1_val, Rotate(x_val + y_val, 33) * K1);
        x_val = Rotate(x_val + y_val, 42) * K1;

        int offset = 0;
        while (len - offset > 64)
        {
            x_val = Rotate(x_val + y_val + v0_val + Fetch64(data, offset + 8), 37) * K1;
            y_val = Rotate(y_val + v1_val + Fetch64(data, offset + 48), 42) * K1;
            x_val ^= w1_val;
            y_val += v0_val + Fetch64(data, offset + 40);
            z_val = Rotate(z_val + w0_val, 33) * K1;
            (v0_val, v1_val) = WeakHashLen32WithSeeds(Fetch64(data, offset), Fetch64(data, offset + 16), Fetch64(data, offset + 24), Fetch64(data, offset + 32), v0_val, v1_val);
            (w0_val, w1_val) = WeakHashLen32WithSeeds(Fetch64(data, offset + 32), Fetch64(data, offset + 40), Fetch64(data, offset + 48), Fetch64(data, offset + 56), w0_val, w1_val);
            ulong temp = z_val;
            z_val = x_val;
            x_val = temp;
            offset += 64;
        }

        return HashLen16(HashLen16(v0_val, v1_val) + ShiftMix(y_val) * K1 + z_val,
                         HashLen16(w0_val, w1_val) + x_val);
    }
}
