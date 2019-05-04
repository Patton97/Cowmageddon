//Created by 16007006
//Required for advanced element: flexible/generic component system
//Provides the behavioral functionality of individual components, as well as 
//the main abstract superclass for all components

#pragma once
#include "components.h"
#include "GameObject.h"
#include "mysoundengine.h"
#include "ObjectManager.h"

/**************
 * COMPONENTS *
 **************/

/**************************************************
 * SUPER COMPONENT ********************************
 **************************************************/

Component::Component(GameObject* pOwner)
{
	this->pOwner = pOwner;
}

Component::~Component()
{
	pOwner = nullptr;
}

/**************************************************
 * INPUT COMPONENT ********************************
 **************************************************/

InputComponent::InputComponent(GameObject* pOwner, float speed) : Component(pOwner)
{
	this->speed = speed;

	//This could be moved to a SoundComponent, but not important for assignment
	MySoundEngine* pSE = MySoundEngine::GetInstance();
	moveSound = pSE->LoadWav(L"thrustloop2.wav");
	shootSound = pSE->LoadWav(L"photon2.wav");

	pSE = nullptr;
}

InputComponent::~InputComponent() {}

void InputComponent::Update()
{
	timer.mark();
	float frameTime = timer.mdFrameTime; //Conversion required for vector calculations

	// *********************************************************************
	// Engine Inits	********************************************************
	// KB Controls
	MyInputs* pInputs = MyInputs::GetInstance();
	pInputs->SampleKeyboard();
	//Sound
	MySoundEngine* pSE = MySoundEngine::GetInstance();

	// *********************************************************************
	// Movement ************************************************************
	
	//Create temp Vector to store new velocity vector
	Vector2D velocity = Vector2D(0, 0);

	pInputs->KeyPressed(DIK_W) ? velocity.YValue =  speed : velocity.YValue;
	pInputs->KeyPressed(DIK_S) ? velocity.YValue = -speed : velocity.YValue;
	pInputs->KeyPressed(DIK_A) ? velocity.XValue = -speed : velocity.XValue;
	pInputs->KeyPressed(DIK_D) ? velocity.XValue =  speed : velocity.XValue;

	if (pInputs->KeyPressed(DIK_W) || pInputs->KeyPressed(DIK_S) || pInputs->KeyPressed(DIK_A) || pInputs->KeyPressed(DIK_D))
	{
		pSE->Play(moveSound, true);
	}
	else
	{ 
		pSE->Stop(moveSound);
	}
	
	pOwner->velocity = velocity;

	// *********************************************************************
	//Rotation *************************************************************

	/* Rotation controls are not needed for the example game I am creating */

	// *********************************************************************
	//Actions **************************************************************

	//Shoot bullet 	
	if ((pInputs->KeyPressed(DIK_Q) || pInputs->KeyPressed(DIK_E)) && bulletDelay <= 0.0f)
	{
		Vector2D bulletPosition;
		Vector2D bulletVelocity;
		//Q = shoot bullet left
		if (pInputs->KeyPressed(DIK_Q))
		{
			bulletPosition = Vector2D(pOwner->position.XValue - 40.0f, pOwner->position.YValue);
			bulletVelocity = Vector2D(-speed * 2, 0.0f);
		}
		//E = shoot bullet right
		else if (pInputs->KeyPressed(DIK_E))
		{
			bulletPosition = Vector2D(pOwner->position.XValue + 40.0f, pOwner->position.YValue);
			bulletVelocity = Vector2D(speed * 2, 0.0f);
		}

		//Create bullet
		pOwner->GetOM()->CreateBullet(bulletPosition, bulletVelocity);
		//Play shooting sound
		pSE->Play(shootSound);
		//Reset bullet delay
		bulletDelay = 1.1f; //Sound of shots are 1second
	}
	//Decrement bulletDelay
	if (bulletDelay >= 0.0f) { bulletDelay -= frameTime; }
}

/**************************************************
 * PHYSICS COMPONENT ******************************
 **************************************************/

PhysicsComponent::PhysicsComponent(GameObject* pOwner, float rotation) : Component(pOwner)
{
	this->rotation = rotation;
	timer.mark();
}

PhysicsComponent::~PhysicsComponent(){}

void PhysicsComponent::Update()
{
	timer.mark();
	float frameTime = timer.mdFrameTime;

	pOwner->position += pOwner->velocity * frameTime;
	pOwner->angle    += this->rotation * frameTime;
}


/**************************************************
 * RENDER COMPONENT *******************************
 **************************************************/

RenderComponent::RenderComponent(GameObject* pOwner, wchar_t* filename, float scale, float transparency) : Component(pOwner)
{
	this->scale = scale;
	this->transparency = transparency;
	MyDrawEngine* pDE = MyDrawEngine::GetInstance();
	img = pDE->LoadPicture(filename);
}

RenderComponent::~RenderComponent() {/*Nothing yet*/}

void RenderComponent::Update()
{
	//If visible, draw
	if (pOwner->active)
	{
		MyDrawEngine* pDE = MyDrawEngine::GetInstance();
		pDE->DrawAt(pOwner->position, img, scale, pOwner->angle, transparency);
	}
	//else, don't draw
}

//Load new image
void RenderComponent::LoadImg(wchar_t* filename)
{
	MyDrawEngine* pDE = MyDrawEngine::GetInstance();
	img = pDE->LoadPicture(filename);
}

//Retrieve object scale
float RenderComponent::GetScale()
{
	return scale;
}

/**************************************************
 * COLLISION COMPONENTS ***************************
 **************************************************/

//CollisionComponent is the root for all collision components
//  By default, it will destroy the owner GameObject regardless of what they have collided with.
CollisionComponent::CollisionComponent(GameObject* pOwner) : Component(pOwner) {/*Nothing*/}
CollisionComponent::~CollisionComponent() {/*Nothing*/}
void CollisionComponent::ProcessCollision(GameObject* otherObject)
{
	pOwner->active = false;
}

/****************************************
 * Shape-Specific Collision Components *
 ****************************************/

//BoxCollisionComponent will destroy the owner GameObject regardless of what they have collided with.
BoxCollisionComponent::BoxCollisionComponent(GameObject* pOwner, float width, float height) : CollisionComponent(pOwner)
{
	this->width  = width;
	this->height = height;
}
BoxCollisionComponent::~BoxCollisionComponent() {/*Nothing*/}
IShape2D* BoxCollisionComponent::GetShape()
{
	//Rescale collision shape relative to object
	float scale = pOwner->GetRender()->GetScale();
	
	//Force-update shape position
	shape.PlaceAt(pOwner->position.YValue + (scale * height), //Top
				  pOwner->position.XValue - (scale * width),  //Left
				  pOwner->position.YValue - (scale * height), //Bottom
				  pOwner->position.XValue + (scale * width)); //Right
	return (&shape); //Return reference
}


//DEBUG ONLY - Visualises collision shape
void BoxCollisionComponent::Update()
{
	//MyDrawEngine::GetInstance()->FillRect(this->shape, MyDrawEngine::YELLOW, pOwner->angle);
}

//CircleCollisionComponent will destroy the owner GameObject regardless of what they have collided with.
CircleCollisionComponent::CircleCollisionComponent(GameObject* pOwner, float radius) : CollisionComponent(pOwner)
{
	this->radius = radius;
}
CircleCollisionComponent::~CircleCollisionComponent() {/*Nothing*/} 
IShape2D* CircleCollisionComponent::GetShape()
{
	shape.PlaceAt(pOwner->position, pOwner->GetRender()->GetScale() * radius); //Force-update shape position
	return (&shape);                                        //before returning reference
}

//DEBUG ONLY - Visualises collision shape
void CircleCollisionComponent::Update()
{
	//MyDrawEngine::GetInstance()->FillCircle(pOwner->position, this->radius, MyDrawEngine::YELLOW);
}

/****************************************
 * Object-Specific Collision Components *
 ****************************************/

//Rocks are indestructible, and therefore override the ProcesCollision function to do nothing
RockCollisionComponent::RockCollisionComponent(GameObject* pOwner, float radius) : CircleCollisionComponent(pOwner, radius){/*Nothing*/}
RockCollisionComponent::~RockCollisionComponent() {/*Nothing*/}
void RockCollisionComponent::ProcessCollision(GameObject* otherObject){/*Rocks are indestructible*/}

//The UFO is destroyed by everything, except cows
UFOCollisionComponent::UFOCollisionComponent(GameObject* pOwner, float radius) : CircleCollisionComponent(pOwner, radius) {/*Nothing*/ }
UFOCollisionComponent::~UFOCollisionComponent() {/*Nothing*/ }
void UFOCollisionComponent::ProcessCollision(GameObject* otherObject)
{
	//The only object that doesn't kill the player is cows
	if (typeid(*otherObject->GetCollision()) != typeid(CowCollisionComponent))
	{
		pOwner->active = false;
	}
}

//Bullets may award points for hitting certain objects, i.e. cows are worth 100points
BulletCollisionComponent::BulletCollisionComponent(GameObject* pOwner, float width, float height) : BoxCollisionComponent(pOwner, width, height) {/*Nothing*/ }
BulletCollisionComponent::~BulletCollisionComponent() {/*Nothing*/ }
void BulletCollisionComponent::ProcessCollision(GameObject* otherObject)
{
	if (typeid(*otherObject->GetCollision()) == typeid(CowCollisionComponent))
	{
		Game::GetInstance()->AddPoints(100.0f); //Award player 100 points
	}
	pOwner->active = false; //Destroy bullet
}

//Bullets may award points for hitting certain objects, i.e. cows are worth 100points
CowCollisionComponent::CowCollisionComponent(GameObject* pOwner, float width, float height) : BoxCollisionComponent(pOwner, width, height) {/*Nothing*/ }
CowCollisionComponent::~CowCollisionComponent() {/*Nothing*/ }
void CowCollisionComponent::ProcessCollision(GameObject* otherObject)
{
	//If a cow is shot, it will be destroyed
	if (typeid(*otherObject->GetCollision()) == typeid(BulletCollisionComponent))
	{
		pOwner->active = false;
	}
}

/**************************************************
 * EXPIRATION COMPONENT ***************************
 **************************************************/

ExpirationComponent::ExpirationComponent(GameObject* pOwner, bool recyclable) : Component(pOwner) 
{
	this->recyclable = recyclable;
}
ExpirationComponent::~ExpirationComponent() {/*Nothing*/}

void ExpirationComponent::Update()
{
	//If object has gone out of bounds (expired)
	if (pOwner->position.XValue < -1920 ||  pOwner->position.XValue >  1920 
	||  pOwner->position.YValue < -1080 ||  pOwner->position.YValue >  1080)
	{
		//If object is recyclable, recycle. Otherwise, deactivate
		this->recyclable ? Recycle() : pOwner->active = false;
	}
}

void ExpirationComponent::Recycle()
{
	pOwner->position = Vector2D((rand() % 1920) + 1920.0f, rand() % 2160 - 1080.0f);
} 