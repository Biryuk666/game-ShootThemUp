// Shoot Them Up Game, All Rights Reserved.

#include "Engine/Canvas.h"
#include "UI/STUGameHUD.h"
#include "UI/STUPlayerHUDWidget.h"

void ASTUGameHUD::DrawHUD() {
	Super::DrawHUD();
	//DrawCrossHair();
}

void ASTUGameHUD::BeginPlay() {
	Super::BeginPlay();
	auto PlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDWidgetClass);

	if (PlayerHUDWidget)
		PlayerHUDWidget->AddToViewport();
}

void ASTUGameHUD::DrawCrossHair()
{
	const TInterval<float> Center(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f);

	const float HalfLineSize = 10.0f;
	const float LineThickness = 2.0f;
	const FLinearColor LineColor = FLinearColor::Green;

	DrawLine(Center.Min - HalfLineSize, Center.Max, Center.Min + HalfLineSize, Center.Max, LineColor, LineThickness);
	DrawLine(Center.Min, Center.Max - HalfLineSize, Center.Min, Center.Max + HalfLineSize, LineColor, LineThickness);
}
