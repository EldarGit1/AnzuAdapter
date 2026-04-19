#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnzuAdMetricsWidget.generated.h"

UCLASS()
class ANZUADAPTER_API UAnzuAdMetricsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UAnzuAdMetricsWidget(const FObjectInitializer& ObjectInitializer);

    virtual bool Initialize() override;

    UFUNCTION(BlueprintCallable, Category = "Anzu|UI")
    void UpdateMetrics(int32 InEmpties, int32 InImpressions, int32 InCompleted, float InAngle, float InVisibility, float InViewability);

private:
    void DrawWidget();

    UPROPERTY()
    class UTextBlock* EmptiesText;

    UPROPERTY()
    class UTextBlock* ImpressionsText;

    UPROPERTY()
    class UTextBlock* CompletedText;

    UPROPERTY()
    class UTextBlock* AngleText;

    UPROPERTY()
    class UTextBlock* VisibilityText;

    UPROPERTY()
    class UTextBlock* ViewabilityText;
};
