#pragma once
//Game States

class Game
{
public:
  enum GameState { Menu, Paused, Playing, Final, Exiting};
  static GameState _gameState;
};
