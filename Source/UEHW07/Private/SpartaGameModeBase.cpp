#include "SpartaGameModeBase.h"
#include "SpartaPawnCharacter.h"
#include "SpartaPlayerController.h"

ASpartaGameModeBase::ASpartaGameModeBase()
{
	DefaultPawnClass = ASpartaPawnCharacter::StaticClass();
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
}
