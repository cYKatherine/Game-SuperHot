/*	FIT2096 - Example Code
*	Game.h
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*	This class is the heart of our game and is also where our game logic will reside
*	It contains the overall Update and Render method for the whole game
*	You should eventually split some game logic into other objects which Game will manage
*	Watch the size of this class - it can grow out of control very fast!
*/

#ifndef GAME_H
#define GAME_H

#include "AudioSystem.h"
#include "Ammunition.h"
#include "Direct3D.h"
#include "Camera.h"
#include "InputController.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "CollisionManager.h"
#include "GameObject.h"
#include "Ruby.h"
#include "Enemy.h"
#include "Bullet.h"
#include "StateMachine.h"
#include "Button.h"
#include "ItemBox.h"
#include "Explosive.h"

#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"

#include <vector>

class Player;
class Enemy;

class Game
{
private:
	enum class GameStates
	{
		MENU_STATE,
		MODE_MENU_STATE,
		STORYMODE_STATE,
		COMPETITIVEMODE_STATE,
		STORYMODE_PAUSE_STATE,
		COMPETITIVEMODE_PAUSE_STATE,
		STATE_COUNT
	};

	StateMachine<GameStates, Game>* m_stateMachine;

	Game();
	static Game* m_pInstance;
	Camera* m_currentCam;
	Direct3D* m_renderer;
	InputController* m_input;
	MeshManager* m_meshManager;
	TextureManager* m_textureManager;
	CollisionManager* m_collisionManager;
	AudioSystem* m_audio;
	Explosive* m_explosive;

	bool m_move = false;
	bool m_firstTimeInit = true;
	int m_competitiveModeScore = 0;

	// Sprites / Fonts
	SpriteBatch* m_spriteBatch;
	SpriteFont* m_arialFont12;
	SpriteFont* m_arialFont18;

	Button* m_startButton;
	Button* m_quitButton;
	Button* m_storyModeButton;
	Button* m_competitiveModeButton;

	// Shaders
	Shader* m_unlitVertexColouredShader;
	Shader* m_unlitTexturedShader;
	Shader* m_diffuseTexturedShader;
	Shader* m_diffuseTexturedFogShader;

	Player* m_player;

	// This contains everything for easy calls to update and render
	std::vector<GameObject*> m_gameObjects;

	// We also need more specific collections for easier collision checks
	std::vector<Player*> m_players;
	std::vector<Ammunition*> m_ammunitions;
	std::vector<Ruby*> m_rubies;
	std::vector<Enemy*> m_enemies;
	std::vector<Bullet*> m_bullets;
	std::vector<ItemBox*> m_itemBoxes;
	std::vector<Explosive*> m_explosives;

	RECT m_healthBarRect;
	Texture* m_hurtOverlaySprite;
	Texture* m_healthBarSprite;

	std::wstring ammunitionString;
	std::wstring rubyString;
	std::wstring competitiveModeScoreString;


	// Initialisation Helpers
	bool InitShaders();
	bool LoadAudio();
	bool LoadMeshes();
	bool LoadTextures();
	void LoadFonts();
	void InitGameWorld();
	void InitPlayers();
	void InitAmmunitions();
	void InitRubies();
	void InitEnemies();
	void InitStates();
	void InitItemBoxes();
	void InitExplosives();

	void InitUI();
	void DrawUI();
	void DrawMenuUI();
	void DrawModeMenuUI();
	void DrawGameUI();
	void DrawPauseUI();
	void BeginUI();
	void EndUI();

	// Every state in our game will have four callbacks
	// We register these with the StateMachine and it calls them for us
	void Menu_OnEnter();
	void Menu_OnUpdate(float timestep);
	void Menu_OnRender();
	void Menu_OnExit();
	
	void Mode_Menu_OnEnter();
	void Mode_Menu_OnUpdate(float timestep);
	void Mode_Menu_OnRender();
	void Mode_Menu_OnExit();

	void Story_Mode_OnEnter();
	void Story_Mode_OnUpdate(float timestep);
	void Story_Mode_OnRender();
	void Story_Mode_OnExit();

	void Competitive_Mode_OnEnter();
	void Competitive_Mode_OnUpdate(float timestep);
	void Competitive_Mode_OnRender();
	void Competitive_Mode_OnExit();

	void Story_Mode_Pause_OnEnter();
	void Story_Mode_Pause_OnUpdate(float timestep);
	void Story_Mode_Pause_OnRender();
	void Story_Mode_Pause_OnExit();

	void Competitive_Mode_Pause_OnEnter();
	void Competitive_Mode_Pause_OnUpdate(float timestep);
	void Competitive_Mode_Pause_OnRender();
	void Competitive_Mode_Pause_OnExit();

public:
	static Game* GetInstance() {
		if (m_pInstance == NULL) m_pInstance = new Game();
		return m_pInstance;
	}
	//Game();	
	~Game();

	bool Initialise(Direct3D* renderer, AudioSystem* audio, InputController* input); //The initialise method will load all of the content for the game (meshes, textures, etc.)

	void Update(float timestep);	//The overall Update method for the game. All gameplay logic will be done somewhere within this method
	void Render();					//The overall Render method for the game. Here all of the meshes that need to be drawn will be drawn

	void Shutdown(); //Cleanup everything we initialised

	void createBullet(Vector3 position, float rotationY, bool fromPlayer);
	void RefreshStoryModeUI();
	void RefreshCompetitiveModeUI();
	void RefreshHealthUI();
	void RefreshAmmunicationUI();
	void setMove(bool move);
	bool getMove();
	bool pickExplosive(Vector3 playerPosition);
	void setExplosive(Vector3 playerPosition);
	void explode(Vector3 explosivePosition);
	void updateScore(int score);
	Color* m_hurtOverlayColor;
};

#endif
