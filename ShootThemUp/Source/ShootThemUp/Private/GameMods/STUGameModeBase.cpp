// Shoot Them Up Game, All Rights Reserved.

#include "Characters/STUBaseCharacter.h"
#include "GameMods/STUGameModeBase.h"
#include "PlayerControllers/STUPlayerController.h"
#include "UI/STUGameHUD.h"

ASTUGameModeBase::ASTUGameModeBase()
{
	DefaultPawnClass = ASTUBaseCharacter::StaticClass();
	PlayerControllerClass = ASTUPlayerController::StaticClass();
	HUDClass = ASTUGameHUD::StaticClass();
}