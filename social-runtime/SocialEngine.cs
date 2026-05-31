// Copyright (c) TahWorld. All rights reserved.

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;

namespace TahWorld;

/// <summary>
/// Fixed-size social interaction payload used by SocialPayloadType.AgentInteraction.
/// Wire size: 14 bytes.
/// </summary>
public readonly record struct SocialInteractionPayload(
    uint SourceAgentId,
    uint TargetAgentId,
    ushort InteractionType,
    float Intensity)
{
    public const int WireSize = 14;
}

public readonly record struct ProactiveHook(float Threshold, ushort ActionId);

public readonly record struct HookExecutionEvent(
    ushort ActionId,
    uint SourceAgentId,
    uint TargetAgentId,
    float OldScore,
    float NewScore,
    float Threshold);

public sealed class SocialEngine
{
    // Directional key: source -> target.
    // This preserves asymmetric relationships such as trust or fear.
    private readonly ConcurrentDictionary<ulong, float> _affinityByDirectedRelation = new();

    // Target-agent hook sets; arrays are replaced atomically to avoid lock contention on reads.
    private readonly ConcurrentDictionary<uint, ProactiveHook[]> _hooksByTarget = new();

    private const float MaxAffinity = 100.0f;
    private const float MinAffinity = -100.0f;

    /// <summary>
    /// Optional observer invoked when a hook threshold crossing is detected.
    /// </summary>
    public event Action<HookExecutionEvent>? HookTriggered;

    public bool ProcessInteraction(in SocialInteractionPayload payload)
    {
        uint sourceAgentId = payload.SourceAgentId;
        uint targetAgentId = payload.TargetAgentId;
        ushort interactionType = payload.InteractionType;
        float intensity = payload.Intensity;

        if (sourceAgentId == 0 || targetAgentId == 0)
        {
            return false;
        }

        if (intensity is < -100f or > 100f)
        {
            return false;
        }

        float multiplier = GetSituationalMultiplier(interactionType);
        ulong relationKey = GetDirectedRelationKey(sourceAgentId, targetAgentId);

        float oldAffinity = 0.0f;
        float newAffinity = _affinityByDirectedRelation.AddOrUpdate(
            relationKey,
            addValueFactory: _ =>
            {
                oldAffinity = 0.0f;
                return ComputeNextAffinity(0.0f, intensity, multiplier);
            },
            updateValueFactory: (_, current) =>
            {
                oldAffinity = current;
                return ComputeNextAffinity(current, intensity, multiplier);
            });

        EvaluateProactiveHooks(sourceAgentId, targetAgentId, oldAffinity, newAffinity);
        return true;
    }

    public float GetAffinity(uint sourceAgentId, uint targetAgentId)
    {
        ulong key = GetDirectedRelationKey(sourceAgentId, targetAgentId);
        return _affinityByDirectedRelation.TryGetValue(key, out float value) ? value : 0.0f;
    }

    public void SetHooksForTarget(uint targetAgentId, IReadOnlyList<ProactiveHook> hooks)
    {
        if (targetAgentId == 0)
        {
            return;
        }

        if (hooks == null || hooks.Count == 0)
        {
            _hooksByTarget.TryRemove(targetAgentId, out _);
            return;
        }

        ProactiveHook[] snapshot = new ProactiveHook[hooks.Count];
        for (int i = 0; i < hooks.Count; i++)
        {
            snapshot[i] = hooks[i];
        }

        _hooksByTarget[targetAgentId] = snapshot;
    }

    private void EvaluateProactiveHooks(uint sourceId, uint targetId, float oldScore, float newScore)
    {
        if (!_hooksByTarget.TryGetValue(targetId, out ProactiveHook[]? hooks) || hooks.Length == 0)
        {
            return;
        }

        foreach (ProactiveHook hook in hooks)
        {
            bool crossedUpward = oldScore < hook.Threshold && newScore >= hook.Threshold;
            bool crossedDownward = oldScore > hook.Threshold && newScore <= hook.Threshold;
            if (!crossedUpward && !crossedDownward)
            {
                continue;
            }

            HookTriggered?.Invoke(new HookExecutionEvent(
                ActionId: hook.ActionId,
                SourceAgentId: sourceId,
                TargetAgentId: targetId,
                OldScore: oldScore,
                NewScore: newScore,
                Threshold: hook.Threshold));
        }
    }

    private static float ComputeNextAffinity(float currentAffinity, float intensity, float multiplier)
    {
        float headroom = intensity >= 0
            ? (MaxAffinity - currentAffinity) / MaxAffinity
            : (currentAffinity - MinAffinity) / MaxAffinity;

        // Allow exact saturation without forcing artificial movement near the cap.
        headroom = Math.Clamp(headroom, 0.0f, 1.0f);
        float delta = intensity * multiplier * headroom;
        return Math.Clamp(currentAffinity + delta, MinAffinity, MaxAffinity);
    }

    private static ulong GetDirectedRelationKey(uint sourceId, uint targetId)
    {
        return ((ulong)sourceId << 32) | targetId;
    }

    private static float GetSituationalMultiplier(ushort interactionType)
    {
        return interactionType switch
        {
            1 => 1.5f, // high-impact positive/negative action
            2 => 0.5f, // low-impact interaction
            _ => 1.0f
        };
    }
}
