#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TsabsPassengerHUD.generated.h"

class ATsabsPassengerAnchor;
class UInputAction;
class UCanvasPanelSlot;

/** Local standard interaction prompt; never binds or consumes controls. */
UCLASS(NotBlueprintable)
class TSABSPASSENGERWAGON_API UTsabsPassengerHUD : public UUserWidget
{
    GENERATED_BODY()
public:
    UTsabsPassengerHUD(const FObjectInitializer& Initializer);
    UFUNCTION(BlueprintPure, Category="Passenger Interface")
    static FText FormatExitPrompt(const FText& LeaveKey, bool bWithinSpeedLimit);
    ATsabsPassengerAnchor* GetPassengerAnchor() const;
protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& Geometry, float DeltaTime) override;
private:
    friend class FTsabsResolvedExitKeyTest;
    void RefreshExitHint(bool bForce = false);
    FText ResolveLeaveKey() const;
    bool ApplyInteractionText(const FText& Text);
    bool CopyNativeInteractionLayout(UCanvasPanelSlot* Destination) const;
    UPROPERTY()
    TSubclassOf<UUserWidget> InteractionWidgetClass;
    UPROPERTY()
    TObjectPtr<UInputAction> LeaveAction;
    UPROPERTY(Transient)
    TObjectPtr<UUserWidget> InteractionWidget;
    TWeakObjectPtr<ATsabsPassengerAnchor> PassengerAnchor;
    FText LastPrompt;
    bool bHasPrompt = false;
    bool bReportedMissingContract = false;
};

