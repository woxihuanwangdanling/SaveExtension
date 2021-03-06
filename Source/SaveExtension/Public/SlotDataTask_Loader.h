// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "ISaveExtension.h"

#include <Engine/Level.h>
#include <Engine/LevelStreaming.h>
#include <GameFramework/Actor.h>
#include <Engine/LevelScriptActor.h>
#include <GameFramework/Controller.h>
#include <AIController.h>

#include "SavePreset.h"
#include "SlotInfo.h"
#include "SlotData.h"

#include "SlotDataTask.h"
#include "SlotDataTask_Loader.generated.h"


/**
* Manages the loading process of a SaveData file
*/
UCLASS()
class USlotDataTask_Loader : public USlotDataTask
{
	GENERATED_BODY()

	int32 Slot;

	UPROPERTY()
	USlotInfo* NewSlotInfo;

protected:

	// Async variables
	TWeakObjectPtr<ULevel> CurrentLevel;
	TWeakObjectPtr<ULevelStreaming> CurrentSLevel;

	int32 CurrentActorIndex;
	TArray<TWeakObjectPtr<AActor>> CurrentLevelActors;

public:

	bool bLoadingMap;


	USlotDataTask_Loader() : Super(), bLoadingMap(false) {}

	auto Setup(int32 InSlot)
	{
		Slot = InSlot;
		return this;
	}

	void OnMapLoaded();

private:

	virtual void OnStart() override;

	virtual void Tick(float DeltaTime) override {
		if(CurrentLevel.IsValid())
			DeserializeASyncLoop();
	}

	void AfterMapValidation();

	/** Spawns Actors hat were saved but which actors are not in the world. */
	void RespawnActors(const TArray<FActorRecord>& Records, const ULevel* Level);

protected:

	/** BEGIN Deserialization */
	void BeforeDeserialize();
	void DeserializeSync();
	void DeserializeLevelSync(const ULevel* Level, const ULevelStreaming* StreamingLevel = nullptr);

	void DeserializeASync();
	void DeserializeLevelASync(ULevel* Level, ULevelStreaming* StreamingLevel = nullptr);

	virtual void DeserializeASyncLoop(float StartMS = 0.0f);

	void FinishedDeserializing();

	void PrepareAllLevels();
	void PrepareLevel(const ULevel* Level, const FLevelRecord& LevelRecord);

	/** Deserializes all Level actors. */
	inline void DeserializeLevel_Actor(AActor* const Actor, const FLevelRecord& LevelRecord);

private:

	/** Deserializes a Level Blueprint Actor */
	void DeserializeLevelScript(ALevelScriptActor* Level, const FLevelRecord& LevelRecord);

	/** Deserializes an AI Controller Actor */
	void DeserializeAI(AAIController* AIController, const FLevelRecord& LevelRecord);

	/** Deserializes Game Mode Actor and its Properties.
	Requires 'SaveGameMode' flag to be used. */
	void DeserializeGameMode();

	/** Deserializes Game State Actor and its Properties.
	Requires 'SaveGameMode' flag to be used. */
	void DeserializeGameState();

	/** Deserializes Player State Actor and its Properties.
	Requires 'SaveGameMode' flag to be used. */
	void DeserializePlayerState(int32 PlayerId);

	/** Deserializes Player Controller Actor and its Properties.
	Requires 'SaveGameMode' flag to be used. */
	void DeserializePlayerController(int32 PlayerId);

	/** Deserializes Player HUD Actor and its Properties.
	Requires 'SaveGameMode' flag to be used. */
	void DeserializePlayerHUD(int32 PlayerId);

	/** Deserializes Current Player's Pawn and its Properties.
	Requires 'SaveGameMode' flag to be used. */
	//void DeserializePlayerPawn(int32 PlayerId);

	/** Deserializes Game Instance Object and its Properties.
	Requires 'SaveGameMode' flag to be used. */
	void DeserializeGameInstance();

	/** Serializes an actor into this Actor Record */
	bool DeserializeActor(AActor* Actor, const FActorRecord& Record);

	/** Deserializes an actor into this Controller Record */
	bool DeserializeController(AController* Actor, const FControllerRecord& Record);

	/** Deserializes the components of an actor from a provided Record */
	void DeserializeActorComponents(AActor* Actor, const FActorRecord& ActorRecord, int8 indent = 0);
	/** END Deserialization */

protected:

	// HELPERS
	FLevelRecord* FindLevelRecord(const ULevelStreaming* Level) const;
	void FindNextAsyncLevel(ULevelStreaming*& OutLevelStreaming) const;
};