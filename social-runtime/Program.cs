// Copyright (c) TahWorld. All rights reserved.

using System;

namespace TahWorld;

public static class Program
{
    public static int Main()
    {
        Console.WriteLine("[SOCIAL_SANDBOX] Starting deterministic social scoring demo.");

        var socialEngine = new SocialEngine();
        socialEngine.SetHooksForTarget(
            targetAgentId: 2,
            hooks:
            [
                new ProactiveHook(Threshold: 30.0f, ActionId: 200),   // e.g. OfferQuest
                new ProactiveHook(Threshold: -40.0f, ActionId: 500)   // e.g. CallGuards
            ]);

        socialEngine.HookTriggered += evt =>
        {
            Console.WriteLine(
                $"[HOOK] action={evt.ActionId} source={evt.SourceAgentId} target={evt.TargetAgentId} " +
                $"old={evt.OldScore:F2} new={evt.NewScore:F2} threshold={evt.Threshold:F2}");
        };

        SocialInteractionPayload[] interactions =
        [
            new(1, 2, 1, +18.0f),  // positive high impact
            new(1, 2, 0, +12.0f),  // standard positive
            new(1, 2, 0, -55.0f),  // strong negative
            new(1, 2, 1, -20.0f)   // extra negative
        ];

        Span<byte> packetBuffer = stackalloc byte[TahBinaryHeaderV2Constants.HeaderSize + SocialInteractionPayload.WireSize];
        foreach (SocialInteractionPayload interaction in interactions)
        {
            if (!TahNetworkIngress.TryBuildAgentInteractionPacket(interaction, packetBuffer, out int bytesWritten, out string buildError))
            {
                Console.WriteLine($"[PACKET_BUILD_ERROR] {buildError}");
                return 1;
            }

            ReadOnlySpan<byte> packet = packetBuffer.Slice(0, bytesWritten);
            if (!TahNetworkIngress.TryProcessRawPacket(packet, socialEngine, out string processError))
            {
                Console.WriteLine($"[PACKET_PROCESS_ERROR] {processError}");
                return 2;
            }

            float affinity = socialEngine.GetAffinity(interaction.SourceAgentId, interaction.TargetAgentId);
            Console.WriteLine(
                $"[INTERACTION] src={interaction.SourceAgentId} dst={interaction.TargetAgentId} " +
                $"type={interaction.InteractionType} intensity={interaction.Intensity:F2} affinity={affinity:F2}");
        }

        Console.WriteLine("[SOCIAL_SANDBOX] Complete.");
        return 0;
    }
}
