/*	FIT2096 - Example Code
*	Game.cpp
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*/

#include "Game.h"
#include "TexturedShader.h"
#include "StaticObject.h"

#include "FirstPersonCamera.h"

#include "MathsHelper.h"
#include "DirectXTK/CommonStates.h"
#include <sstream>


Game* Game::m_pInstance = NULL;

Game::Game()
{
	m_renderer = NULL;
	m_currentCam = NULL;
	m_input = NULL;
	m_meshManager = NULL;
	m_textureManager = NULL;
	m_diffuseTexturedShader = NULL;
	m_unlitVertexColouredShader = NULL;
	m_spriteBatch = NULL;
	m_arialFont12 = NULL;
	m_arialFont18 = NULL;
	m_player = NULL;
	m_stateMachine = NULL;
	m_startButton = NULL;
	m_quitButton = NULL;
}

Game::~Game() {}

bool Game::Initialise(Direct3D* renderer, InputController* input)
{
	m_renderer = renderer;	
	m_input = input;
	m_meshManager = new MeshManager();
	m_textureManager = new TextureManager();
	m_currentCam = new Camera();

	m_healthBarRect = { 0, 0, 0, 0 };
	m_hurtOverlayColor = NULL;

	if (!InitShaders())
		return false;

	if (!LoadMeshes())
		return false;

	if (!LoadTextures())
		return false;

	LoadFonts();
	InitUI();
	InitGameWorld();
	RefreshUI();
	InitStates();

	m_collisionManager = new CollisionManager(&m_players, &m_ammunitions, &m_rubies, &m_enemies, &m_bullets);


	return true;
}

bool Game::InitShaders()
{
	m_unlitVertexColouredShader = new Shader();
	if (!m_unlitVertexColouredShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/UnlitVertexColourPixelShader.ps"))
		return false;

	m_unlitTexturedShader = new TexturedShader();
	if (!m_unlitTexturedShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/UnlitTexturedPixelShader.ps"))
		return false;

	m_diffuseTexturedShader = new TexturedShader();
	if (!m_diffuseTexturedShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/DiffuseTexturedPixelShader.ps"))
		return false;

	m_diffuseTexturedFogShader = new TexturedShader();
	if (!m_diffuseTexturedFogShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/DiffuseTexturedFogPixelShader.ps"))
		return false;

	return true;
}

bool Game::LoadMeshes()
{

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/ammoBlock.obj"))
		return false;
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/bullet.obj"))
		return false;
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/enemy.obj"))
		return false;
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/ground.obj"))
		return false;
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/ruby.obj"))
		return false;

	return true;
}

bool Game::LoadTextures()
{

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/bullet.png"))
		return false;
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_red.png"))
		return false;
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_redDarker.png"))
		return false;
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_redLighter.png"))
		return false;
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_redOrange.png"))
		return false;
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_redPink.png"))
		return false;
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/ground.png"))
		return false;
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/sprite_healthBar.png"))
		return false;
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/sprite_hurtOverlay.png"))
		return false;
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/button.png"))
		return false;

	return true;
}

void Game::LoadFonts()
{
	// There's a few different size fonts in there, you know
	m_arialFont12 = new SpriteFont(m_renderer->GetDevice(), L"Assets/Fonts/Arial-12pt.spritefont");
	m_arialFont18 = new SpriteFont(m_renderer->GetDevice(), L"Assets/Fonts/Arial-18pt.spritefont");
}

void Game::InitUI()
{
	m_spriteBatch = new SpriteBatch(m_renderer->GetDeviceContext());
	m_hurtOverlaySprite = m_textureManager->GetTexture("Assets/Textures/sprite_hurtOverlay.png");
	m_healthBarSprite = m_textureManager->GetTexture("Assets/Textures/sprite_healthBar.png");
	m_hurtOverlayColor = new Color(1, 1, 1, 0); // Transparant
	m_healthBarRect.left = 20;
	m_healthBarRect.top = 20;
	m_healthBarRect.bottom = 50;
	Texture* buttonTexture = m_textureManager->GetTexture("Assets/Textures/button.png");

	// Also init any buttons here
	m_startButton = new Button(128, 64, buttonTexture, L"Start Game", Vector2(574, 385), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		// Transition into the Gameplay state (these buttons are only used on the menu screen)
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	});

	m_quitButton = new Button(128, 64, buttonTexture, L"Quit", Vector2(706, 385), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		// Tell windows to send a WM_QUIT message into our message pump
		PostQuitMessage(0);
	});
}

void Game::RefreshUI()
{
	// Ensure text in UI matches latest scores etc (call this after data changes)
	std::wstringstream ss, sss;
	ss << "Ammunition No: " << m_player->getAmmunitionNo() << " Bullet No: " << m_player->getBulletNo();
	ammunitionString = ss.str();
	sss << "Ruby Left: " << m_rubies.size();
	rubyString = sss.str();
	m_healthBarRect.right = 20 + m_player->getHealth() * 1.5;

}

void Game::InitGameWorld()
{
	InitPlayers();
	InitAmmunitions();
	InitRubies();
	InitEnemies();

	// Static scenery objects
	m_gameObjects.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ground.obj"),
		m_diffuseTexturedFogShader,
		m_textureManager->GetTexture("Assets/Textures/ground.png")));

}

void Game::InitPlayers()
{
	m_player = new Player(m_currentCam, m_input);

	m_gameObjects.push_back(m_player);
	m_players.push_back(m_player);
}

void Game::InitAmmunitions()
{
	Vector3 position = Vector3(MathsHelper::RandomRange(-50.0f, 50.0f), 0.0f, MathsHelper::RandomRange(-50.0f, 50.0f));

	Ammunition* ammunition = new Ammunition(m_meshManager->GetMesh("Assets/Meshes/ammoBlock.obj"),
		m_diffuseTexturedFogShader,
		position);
	
	m_ammunitions.push_back(ammunition);
	m_gameObjects.push_back(ammunition);
}

void Game::InitRubies()
{
	for (int i = 0; i < 10; i++)
	{
		Vector3 position = Vector3(MathsHelper::RandomRange(-50.0f, 50.0f), 0.0f, MathsHelper::RandomRange(-50.0f, 50.0f));

		Ruby* ruby = new Ruby(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"),
			m_diffuseTexturedFogShader,
			position);

		m_rubies.push_back(ruby);
		//m_gameObjects.push_back(ruby);
	}
}

void Game::InitEnemies() {
	for (int i = 0; i < 5; i++)
	{
		Vector3 position = Vector3(MathsHelper::RandomRange(-50.0f, 50.0f), 0.0f, MathsHelper::RandomRange(-50.0f, 50.0f));

		Enemy* enemy = new Enemy(m_meshManager->GetMesh("Assets/Meshes/enemy.obj"),
			m_diffuseTexturedFogShader,
			m_textureManager->GetTexture("Assets/Textures/gradient_redPink.png"),
			position, i+1, m_player, m_rubies);

		m_enemies.push_back(enemy);
		m_gameObjects.push_back(enemy);
	}
}

void Game::InitStates()
{
	// Our state machine needs to know its owner (so it only runs the callbacks while its owner exists)
	m_stateMachine = new StateMachine<GameStates, Game>(this, GameStates::MENU_STATE);

	// Let's match some states with with their OnEnter, OnUpdate, OnRender, OnExit callbacks.
	// Have a look in StateMachine.h to see how this works internally.
	m_stateMachine->RegisterState(GameStates::MENU_STATE, &Game::Menu_OnEnter,
		&Game::Menu_OnUpdate, &Game::Menu_OnRender, &Game::Menu_OnExit);

	m_stateMachine->RegisterState(GameStates::GAMEPLAY_STATE, &Game::Gameplay_OnEnter,
		&Game::Gameplay_OnUpdate, &Game::Gameplay_OnRender, &Game::Gameplay_OnExit);

	m_stateMachine->RegisterState(GameStates::PAUSE_STATE, &Game::Pause_OnEnter,
		&Game::Pause_OnUpdate, &Game::Pause_OnRender, &Game::Pause_OnExit);
}

void Game::createBullet(Vector3 position, float rotationY, bool fromPlayer) {
	Bullet* bullet = new Bullet(m_meshManager->GetMesh("Assets/Meshes/bullet.obj"),
		m_diffuseTexturedFogShader,
		m_textureManager->GetTexture("Assets/Textures/bullet.png"),
		position,
		rotationY);
	if (fromPlayer) {
		bullet->setFromPlyer(true);
	}
	else {
		bullet->setFromPlyer(false);
	}
	m_bullets.push_back(bullet);
}

void Game::Update(float timestep)
{
	m_input->BeginUpdate();

	m_stateMachine->Update(timestep);

	m_input->EndUpdate();
}

void Game::Render()
{
	m_renderer->BeginScene(0.8f, 1.0f, 0.9f, 1.0f);

	m_stateMachine->Render();

	m_renderer->EndScene();		
}

void Game::DrawUI()
{
	// Sprites don't use a shader 
	m_renderer->SetCurrentShader(NULL);

	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	// Do UI drawing between the Begin and End calls

	// Let's draw some text over our game
	m_arialFont18->DrawString(m_spriteBatch, L"ESC to quit", Vector2(20, 160), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0,0));
	m_arialFont18->DrawString(m_spriteBatch, ammunitionString.c_str(), Vector2(20, 140), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
	m_arialFont18->DrawString(m_spriteBatch, rubyString.c_str(), Vector2(20, 120), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));

	
	// Here's how we draw a sprite over our game (caching it in m_currentItemSprite so we don't have to find it every frame)
	m_spriteBatch->Draw(m_healthBarSprite->GetShaderResourceView(), m_healthBarRect, Color(1.0f, 1.0f, 1.0f));
	m_spriteBatch->Draw(m_hurtOverlaySprite->GetShaderResourceView(), Vector2(20, 20), *m_hurtOverlayColor);
	//m_spriteBatch->Draw(m_currentItemSprite->GetShaderResourceView(), Vector2(20, 20), Color(1.0f, 1.0f, 1.0f));
	
	m_spriteBatch->End();
}

void Game::Shutdown()
{
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		delete m_gameObjects[i];
	}

	m_gameObjects.empty();

	if (m_currentCam)
	{
		delete m_currentCam;
		m_currentCam = NULL;
	}

	if (m_unlitVertexColouredShader)
	{
		m_unlitVertexColouredShader->Release();
		delete m_unlitVertexColouredShader;
		m_unlitVertexColouredShader = NULL;
	}

	if (m_unlitTexturedShader)
	{
		m_unlitTexturedShader->Release();
		delete m_unlitTexturedShader;
		m_unlitTexturedShader = NULL;
	}

	if(m_diffuseTexturedShader)
	{
		m_diffuseTexturedShader->Release();
		delete m_diffuseTexturedShader;
		m_diffuseTexturedShader = NULL;
	}

	if (m_diffuseTexturedFogShader)
	{
		m_diffuseTexturedFogShader->Release();
		delete m_diffuseTexturedShader;
		m_diffuseTexturedShader = NULL;
	}

	if (m_meshManager)
	{
		m_meshManager->Release();
		delete m_meshManager;
		m_meshManager = NULL;
	}

	if (m_textureManager)
	{
		m_textureManager->Release();
		delete m_textureManager;
		m_textureManager = NULL;
	}

	if (m_spriteBatch)
	{
		delete m_spriteBatch;
		m_spriteBatch = NULL;
	}

	if (m_arialFont12)
	{
		delete m_arialFont12;
		m_arialFont12 = NULL;
	}

	if (m_arialFont18)
	{
		delete m_arialFont18;
		m_arialFont18 = NULL;
	}
}

void Game::setMove(bool move) {
	m_move = move;
}

bool Game::getMove() {
	return m_move;
}

void Game::Menu_OnEnter()
{
	OutputDebugString("Menu OnEnter\n");
}

void Game::Menu_OnUpdate(float timestep)
{
	// Button's need to update so they can check if the mouse is over them (they swap to a hover section of the image)
	m_startButton->Update();
	m_quitButton->Update();
}

void Game::Menu_OnRender()
{
	DrawMenuUI();
}

void Game::Menu_OnExit()
{
	OutputDebugString("Menu OnExit\n");
}

void Game::Gameplay_OnEnter()
{
	OutputDebugString("GamePlay OnEnter\n");
}

void Game::Gameplay_OnUpdate(float timestep)
{
	// Update all our gameobjects. What they really are doesn't matter
	if (m_input->GetKeyDown(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->Update(timestep);
	}

	for (unsigned int i = 0; i < m_rubies.size(); i++) {
		if (m_rubies[i]->getPicked()) {
			m_rubies.erase(m_rubies.begin() + i);
			RefreshUI();
		}
	}

	if (m_rubies.size() == 0) {
		PostQuitMessage(0);
	}

	for (unsigned int i = 0; i < m_bullets.size(); i++) {
		if (m_bullets[i]->getDisappear()) {
			m_bullets.erase(m_bullets.begin() + i);
		}
	}

	for (unsigned int i = 0; i < m_bullets.size(); i++) {
		m_bullets[i]->Update(timestep);
	}

	for (unsigned int i = 0; i < m_enemies.size(); i++) {
		m_enemies[i]->updateRubies(m_rubies);
	}

	if (m_hurtOverlayColor->A() != 0) {
		m_hurtOverlayColor->A(m_hurtOverlayColor->A() - 0.1);
		RefreshUI();
	}

	m_collisionManager->CheckCollisions();

	m_currentCam->Update(timestep);

	// Should we pause
	if (m_input->GetKeyDown('P'))
	{
		m_stateMachine->ChangeState(GameStates::PAUSE_STATE);
	}
}

void Game::Gameplay_OnRender()
{
	// Draw our gameobjects
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->Render(m_renderer, m_currentCam);
	}

	for (unsigned int i = 0; i < m_rubies.size(); i++) {
		m_rubies[i]->Render(m_renderer, m_currentCam);
	}

	for (unsigned int i = 0; i < m_bullets.size(); i++) {
		m_bullets[i]->Render(m_renderer, m_currentCam);
	}

	DrawUI();
}

void Game::Gameplay_OnExit()
{
	OutputDebugString("GamePlay OnExit\n");
}

void Game::Pause_OnEnter()
{
	OutputDebugString("Pause OnEnter\n");
}

void Game::Pause_OnUpdate(float timestep)
{
	// Check if we should exit pause
	if (m_input->GetKeyDown('P'))
	{
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	}
}

void Game::Pause_OnRender()
{
	// Keep drawing the game when paused (it's not being updated though which is what freezes it)
	Gameplay_OnRender();

	// In addition to the game, draw some UI to say we're paused
	DrawPauseUI();
}

void Game::Pause_OnExit()
{
	OutputDebugString("Pause OnExit\n");
}

void Game::DrawMenuUI()
{
	BeginUI();

	m_startButton->Render();
	m_quitButton->Render();

	m_arialFont18->DrawString(m_spriteBatch, L"FIT2096 Week 9", Vector2(550, 295), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));

	EndUI();
}

void Game::DrawGameUI()
{
	BeginUI();

	// We don't want to see instructional text like this in your assignments (that's what a readme is for)
	m_arialFont18->DrawString(m_spriteBatch, L"P to toggle pause", Vector2(540, 680), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));

	EndUI();
}

void Game::DrawPauseUI()
{
	BeginUI();
	m_arialFont18->DrawString(m_spriteBatch, L"Paused", Vector2(605, 10), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
	EndUI();
}

void Game::BeginUI()
{
	// Sprites don't use a shader 
	m_renderer->SetCurrentShader(NULL);

	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());
}

void Game::EndUI()
{
	m_spriteBatch->End();
}