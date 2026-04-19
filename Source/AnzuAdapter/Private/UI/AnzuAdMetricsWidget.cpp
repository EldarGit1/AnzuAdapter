#include "UI/AnzuAdMetricsWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

UAnzuAdMetricsWidget::UAnzuAdMetricsWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    EmptiesText = nullptr;
    ImpressionsText = nullptr;
    CompletedText = nullptr;
    AngleText = nullptr;
    VisibilityText = nullptr;
    ViewabilityText = nullptr;
}

bool UAnzuAdMetricsWidget::Initialize()
{
    WidgetTree = NewObject<UWidgetTree>();
    DrawWidget();
    return Super::Initialize();
}

void UAnzuAdMetricsWidget::DrawWidget()
{
    // Root overlay provides the drawing surface
    UOverlay* root = WidgetTree->ConstructWidget<UOverlay>();

    // Dark background image
    UImage* bg = WidgetTree->ConstructWidget<UImage>();
    FLinearColor bgColor(0.0f, 0.0f, 0.0f, 0.85f);
    bg->SetColorAndOpacity(bgColor);
    UOverlaySlot* bgSlot = root->AddChildToOverlay(bg);
    bgSlot->SetHorizontalAlignment(HAlign_Fill);
    bgSlot->SetVerticalAlignment(VAlign_Fill);

    // Vertical box for rows
    UVerticalBox* col = WidgetTree->ConstructWidget<UVerticalBox>();
    UOverlaySlot* colSlot = root->AddChildToOverlay(col);
    colSlot->SetPadding(FMargin(8.0f));

    // Helper lambda: create one labeled row
    auto MakeRow = [&](const FString& label, UTextBlock*& valueOut)
    {
        UHorizontalBox* row = WidgetTree->ConstructWidget<UHorizontalBox>();

        UTextBlock* lbl = WidgetTree->ConstructWidget<UTextBlock>();
        lbl->SetText(FText::FromString(label));
        lbl->SetColorAndOpacity(FSlateColor(FLinearColor::White));
        row->AddChildToHorizontalBox(lbl);

        valueOut = WidgetTree->ConstructWidget<UTextBlock>();
        valueOut->SetText(FText::FromString(TEXT("0")));
        valueOut->SetColorAndOpacity(FSlateColor(FLinearColor::White));
        row->AddChildToHorizontalBox(valueOut);

        col->AddChildToVerticalBox(row);
    };

    MakeRow(TEXT("Empties: "),     EmptiesText);
    MakeRow(TEXT("Impressions: "), ImpressionsText);
    MakeRow(TEXT("Completed: "),   CompletedText);
    MakeRow(TEXT("Angle: "),       AngleText);
    MakeRow(TEXT("Visibility: "),  VisibilityText);
    MakeRow(TEXT("Viewability: "), ViewabilityText);

    WidgetTree->RootWidget = root;
}

void UAnzuAdMetricsWidget::UpdateMetrics(int32 InEmpties, int32 InImpressions, int32 InCompleted, float InAngle, float InVisibility, float InViewability)
{
    if (EmptiesText)
    {
        EmptiesText->SetText(FText::FromString(FString::Printf(TEXT("Empties: %d"), InEmpties)));
    }

    if (ImpressionsText)
    {
        ImpressionsText->SetText(FText::FromString(FString::Printf(TEXT("Impressions: %d"), InImpressions)));
    }

    if (CompletedText)
    {
        CompletedText->SetText(FText::FromString(FString::Printf(TEXT("Completed: %d"), InCompleted)));
    }

    if (AngleText)
    {
        AngleText->SetText(FText::FromString(FString::Printf(TEXT("Angle: %.2f"), InAngle)));
    }

    if (VisibilityText)
    {
        VisibilityText->SetText(FText::FromString(FString::Printf(TEXT("Visibility: %.3f"), InVisibility)));
    }

    if (ViewabilityText)
    {
        ViewabilityText->SetText(FText::FromString(FString::Printf(TEXT("Viewability: %.3f"), InViewability)));
    }
}
