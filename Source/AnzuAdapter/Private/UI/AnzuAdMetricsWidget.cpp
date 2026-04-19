#include "UI/AnzuAdMetricsWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/VerticalBoxSlot.h"

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

    // Semi-transparent dark-blue background
    UImage* bg = WidgetTree->ConstructWidget<UImage>();
    bg->SetColorAndOpacity(FLinearColor(0.04f, 0.04f, 0.10f, 0.60f));
    UOverlaySlot* bgSlot = root->AddChildToOverlay(bg);
    bgSlot->SetHorizontalAlignment(HAlign_Fill);
    bgSlot->SetVerticalAlignment(VAlign_Fill);

    // Content column
    UVerticalBox* col = WidgetTree->ConstructWidget<UVerticalBox>();
    UOverlaySlot* colSlot = root->AddChildToOverlay(col);
    colSlot->SetPadding(FMargin(12.0f, 8.0f));

    const FLinearColor AccentColor(0.3f, 0.8f, 1.0f, 1.0f);    // cyan
    const FLinearColor LabelColor (0.60f, 0.60f, 0.72f, 1.0f); // muted lavender-gray
    const FLinearColor ValueColor (FLinearColor::White);

    // Title
    {
        UTextBlock* title = WidgetTree->ConstructWidget<UTextBlock>();
        title->SetText(FText::FromString(TEXT("ANZU METRICS")));
        title->SetColorAndOpacity(FSlateColor(AccentColor));
        UVerticalBoxSlot* s = col->AddChildToVerticalBox(title);
        s->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
    }

    // Adds a small dimmed section header
    auto AddSection = [&](const FString& text)
    {
        UTextBlock* sec = WidgetTree->ConstructWidget<UTextBlock>();
        sec->SetText(FText::FromString(text));
        sec->SetColorAndOpacity(FSlateColor(FLinearColor(0.42f, 0.42f, 0.52f, 1.0f)));
        UVerticalBoxSlot* s = col->AddChildToVerticalBox(sec);
        s->SetPadding(FMargin(0.0f, 4.0f, 0.0f, 2.0f));
    };

    // One labeled row: label in muted gray on the left, live value in white on the right
    auto MakeRow = [&](const FString& label, UTextBlock*& valueOut)
    {
        UHorizontalBox* row = WidgetTree->ConstructWidget<UHorizontalBox>();

        UTextBlock* lbl = WidgetTree->ConstructWidget<UTextBlock>();
        lbl->SetText(FText::FromString(label));
        lbl->SetColorAndOpacity(FSlateColor(LabelColor));
        row->AddChildToHorizontalBox(lbl);

        valueOut = WidgetTree->ConstructWidget<UTextBlock>();
        valueOut->SetText(FText::FromString(TEXT("0")));
        valueOut->SetColorAndOpacity(FSlateColor(ValueColor));
        row->AddChildToHorizontalBox(valueOut);

        UVerticalBoxSlot* rowSlot = col->AddChildToVerticalBox(row);
        rowSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));
    };

    AddSection(TEXT("STATS"));
    MakeRow(TEXT("Empties      "), EmptiesText);
    MakeRow(TEXT("Impressions  "), ImpressionsText);
    MakeRow(TEXT("Completed    "), CompletedText);

    AddSection(TEXT("VISIBILITY"));
    MakeRow(TEXT("Angle        "), AngleText);
    MakeRow(TEXT("Visibility   "), VisibilityText);
    MakeRow(TEXT("Viewability  "), ViewabilityText);

    WidgetTree->RootWidget = root;
}

void UAnzuAdMetricsWidget::UpdateMetrics(int32 InEmpties, int32 InImpressions, int32 InCompleted, float InAngle, float InVisibility, float InViewability)
{
    if (EmptiesText)
    {
        EmptiesText->SetText(FText::FromString(FString::Printf(TEXT("%d"), InEmpties)));
    }

    if (ImpressionsText)
    {
        ImpressionsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), InImpressions)));
    }

    if (CompletedText)
    {
        CompletedText->SetText(FText::FromString(FString::Printf(TEXT("%d"), InCompleted)));
    }

    if (AngleText)
    {
        AngleText->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), InAngle)));
    }

    if (VisibilityText)
    {
        VisibilityText->SetText(FText::FromString(FString::Printf(TEXT("%.3f"), InVisibility)));
    }

    if (ViewabilityText)
    {
        ViewabilityText->SetText(FText::FromString(FString::Printf(TEXT("%.3f"), InViewability)));
    }
}
