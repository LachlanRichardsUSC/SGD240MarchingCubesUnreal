// Copyright Epic Games, Inc. All Rights Reserved.

#include "SGD240ProceduralGameMode.h"
#include "SGD240ProceduralCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASGD240ProceduralGameMode::ASGD240ProceduralGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
