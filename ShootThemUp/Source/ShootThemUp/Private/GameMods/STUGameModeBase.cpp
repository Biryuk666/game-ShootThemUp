// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/STUBaseCharacter.h"
#include "GameMods/STUGameModeBase.h"
#include "PlayerControllers/STUPlayerController.h"

ASTUGameModeBase::ASTUGameModeBase()
{
	DefaultPawnClass = ASTUBaseCharacter::StaticClass();
	PlayerControllerClass = ASTUPlayerController::StaticClass();
}