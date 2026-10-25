#pragma once

#include "CoreMinimal.h"
#include "FGRailroadVehicle.h"
#include "PassengerBoardingPolicy.h"
#include "PassengerBoardingReservation.h"
#include "TsabsPassengerRailcar.generated.h"

class UTsabsPassengerRailMovement;
class USkeletalMeshComponent;
class AFGFreightWagon;

UCLASS(Blueprintable)
class TSABSPASSENGERWAGON_API ATsabsPassengerRailcar : public AFGRailroadVehicle
{
    GENERATED_BODY()
public:
    ATsabsPassengerRailcar();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void PostLoadGame_Implementation(int32 SaveVersion, int32 GameVersion) override;
    virtual bool CanDismantle_Implementation() const override;
    virtual void Dismantle_Implementation() override;
    virtual void GetDismantleDisqualifiers_Implementation(TArray<TSubclassOf<UFGConstructDisqualifier>>& OutReasons, const TArray<AActor*>& SelectedActors) const override;
    /** Includes native driver/pending/controller state and our transient transaction. */
    bool HasProtectedPilot() const;
    bool ReleaseEmptyPilot();
    /** Idempotent authority-only creation/repair of the single technical pilot. */
    bool EnsurePilotAnchor();
    class ATsabsPassengerAnchor* GetPilotAnchor() const { return PilotAnchor; }
    virtual UPawnMovementComponent* GetMovementComponent() const override;
    virtual UFGRailroadVehicleMovementComponent* GetRailroadVehicleMovementComponent() const override;
    virtual bool CanDriverEnter(AFGCharacterPlayer* Character) override;
    virtual bool DriverEnter(AFGCharacterPlayer* Character) override;
    USkeletalMeshComponent* GetPassengerMesh() const;
    virtual void UpdateUseState_Implementation(AFGCharacterPlayer* Character, const FVector& AtLocation, UPrimitiveComponent* ComponentHit, FUseState& OutState) override;
    virtual void OnUse_Implementation(AFGCharacterPlayer* Character, const FUseState& State) override;
    virtual bool IsUseable_Implementation() const override;
    virtual FText GetLookAtDecription_Implementation(AFGCharacterPlayer* Character, const FUseState& State) const override;
    /** Uses the key resolved by the game's controller, never a hard-coded E. */
    UFUNCTION(BlueprintPure, Category="Passenger Interface")
    static FText FormatBoardingPrompt(const FText& UseKey);
    ETsabsBoardingFeedback GetBoardingFeedback(AFGCharacterPlayer* Character) const;
    /** Only the reserved synchronous server entry may mutate the anchor. */
    bool IsBoardingReservationValid(AFGCharacterPlayer* Character) const;
    bool IsBoardingTransactionActive() const { return BoardingReservation.IsReserved(); }
private:
    bool CanReachBoardingBody(AFGCharacterPlayer* Character) const;
    bool IsBoardingCharacterValid(AFGCharacterPlayer* Character) const;
    bool TryBoardPilot(AFGCharacterPlayer* Character);
    FPassengerBoardingReservation BoardingReservation;
    friend class FTsabsPilotProtectionTest;
    friend class FTsabsBoardingGuardsTest;
    UPROPERTY(SaveGame, Replicated)
    TObjectPtr<class ATsabsPassengerAnchor> PilotAnchor;
    UPROPERTY(VisibleDefaultsOnly, Category="Passenger Railcar")
    TObjectPtr<UTsabsPassengerRailMovement> PassengerMovement;
    UPROPERTY()
    TSubclassOf<AFGFreightWagon> FreightSetupReference;
};
