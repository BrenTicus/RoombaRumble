#pragma once
#include "PhysicsManager.h"
#include "Roomba.h"
#include <vector>


class AIManager
{
private:

	int aiCount;
	vector<Roomba*>* allAIRoombas;
	int playerCount;
	vector<Roomba*>* allPlayers;

	vector<DriveControl*> resultAIControl;
public:
	AIManager(vector<Roomba*>* playerList, vector<Roomba*>* aiList){
		aiCount =aiList->size();
		playerCount = playerList->size();
		
		allAIRoombas = aiList;
		allPlayers = playerList;

		for (int i =0; i < aiCount ; i++){
			resultAIControl.push_back(new DriveControl());
		}
	}
	~AIManager(void);

	void Update();


	vector<DriveControl*>* getAIControls();
};

