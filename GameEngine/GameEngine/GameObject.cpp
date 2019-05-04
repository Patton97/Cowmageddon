//Created by 16007006
//Provides a GameObject class which can then be modified and controlled by different components.
//GO's have an initialise component to allow gameplay progammer to set up object outside of constructor.

#include "GameObject.h"
#include "Shapes.h"
//#include <iostream>

GameObject::GameObject(ObjectManager* pObjectManager = nullptr)
{
	this->active = false;
	this->pObjectManager = pObjectManager;		
}

GameObject::~GameObject()
{
	//Destroy all connected components & flush list
	for (Component* &pComponent : pComponentList)
	{
		delete pComponent;
		pComponent = nullptr;
	}
}

void GameObject::Initialise(RenderComponent* pRenderComponent, CollisionComponent* pCollisionComponent, Vector2D position, Vector2D velocity)
{
	//Attach core components
	this->pRenderComponent = pRenderComponent;
	this->pCollisionComponent = pCollisionComponent;
	this->AddComponent(pRenderComponent);
	this->AddComponent(pCollisionComponent);
	
	//Initialise variables
	this->position = position;	
	this->velocity = velocity;	
	this->angle    = velocity.angle();
	this->active   = true;
}

void GameObject::Update()
{
	for (Component* pComponent : pComponentList)
	{
		pComponent->Update();
	}
}

bool GameObject::isActive() 
{
	return active;
}

void GameObject::AddComponent(Component* newComponent)
{
	this->pComponentList.push_back(newComponent);
}

RenderComponent* GameObject::GetRender()
{
	return this->pRenderComponent;
}

CollisionComponent* GameObject::GetCollision()
{
	return this->pCollisionComponent;
}

ObjectManager* GameObject::GetOM()
{
	return this->pObjectManager;
}