#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TahBinaryTypes.h"
#include "TahNetworkSubsystem.generated.h"

// Delegate to broadcast valid payloads safely on the Game Thread
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSocialPayloadReceived, const FSocialInteractionPayload&, Payload);

UCLASS()
class KINETICMAGI_API UTahNetworkSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // USubsystem Interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Connects to the TAH C# Server socket */
    UFUNCTION(BlueprintCallable, Category = "TAH|Network")
    void ConnectToServer(const FString& IpAddress, int32 Port);

    /** Disconnects the socket and shuts down the listening thread */
    UFUNCTION(BlueprintCallable, Category = "TAH|Network")
    void Disconnect();

    /** Fired when a valid social payload is parsed and verified */
    UPROPERTY(BlueprintAssignable, Category = "TAH|Network")
    FOnSocialPayloadReceived OnSocialPayloadReceived;

private:
    /** The zero-allocation parser that validates raw bytes */
    void HandleReceivedRawBytes(const TArray<uint8>& RawBuffer);

    // Background Listening Task variables
    TFuture<void> ListenerTask;
    TAtomic<bool> bIsListening;
    class FSocket* ConnectionSocket;
};
