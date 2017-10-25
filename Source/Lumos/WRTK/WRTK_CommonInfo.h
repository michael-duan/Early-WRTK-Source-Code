#pragma once

struct LUMOS_API WRTK_CommonInfo
{
	enum ButtonName { ONE, TWO, ANALOG, TRIGGER, GRIP };
};

UENUM(BlueprintType)
enum DirectionType
{
	ACTOR_RIGHT		UMETA(DisplayName = "Right"),
	ACTOR_FORWARD	UMETA(DisplayName = "Forward"),
	ACTOR_UP		UMETA(DisplayName = "Up"),
	ACTOR_NEG_RIGHT		UMETA(DisplayName = "NegRight"),
	ACTOR_NEG_FORWARD	UMETA(DisplayName = "NegForward"),
	ACTOR_NEG_UP		UMETA(DisplayName = "NegUp"),
};