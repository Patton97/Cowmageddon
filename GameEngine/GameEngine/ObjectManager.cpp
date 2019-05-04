//Created by 16007006
//Controls the creation, updating, and deletion of game objects
//Also provides functionality to check for collisions

#include "ObjectManager.h"
#include <algorithm>
#include "GameObject.h"
#include "components.h"
#include "gamecode.h" 

ObjectManager::ObjectManager()  {}
ObjectManager::~ObjectManager() {} // Destructor

//Add new object pointer to the list
void ObjectManager::AddObject(GameObject* pNewObject)
{
	this->pObjectList.push_back(pNewObject);
}

//Object Factory
//  Creates GameObjects and attaches components to contextualise GOs, 
//  alleviating dependancies and cleans up any gamecode in the Game class
GameObject* ObjectManager::CreateUFO(Vector2D position)
{
	//Create new GO & initialise
	GameObject* pNewGO = new GameObject(this);
	pNewGO->Initialise(new RenderComponent(pNewGO, L"ufo.bmp", 1.25f),
					   new UFOCollisionComponent(pNewGO, 30.0f),
					   position, Vector2D(0,0));

	//Attach custom components required by UFO
	pNewGO->AddComponent(new InputComponent(pNewGO, 300.0f)); //UFO can be controlled by the player.
	pNewGO->AddComponent(new PhysicsComponent(pNewGO));       //UFO can move around the screen.
	pNewGO->AddComponent(new ExpirationComponent(pNewGO));    //UFO expires if off-screen	

	//Add GO reference to ObjectManager's pObjectList
	this->AddObject(pNewGO);
	return pNewGO;
}

GameObject* ObjectManager::CreateRock(Vector2D position, Vector2D velocity, float rotation)
{
	//Create new GO & initialise
	GameObject* pNewGO = new GameObject(this);
	//Rocks can be assigned 1 of 4 images
	wchar_t* filename = nullptr;
	switch ((rand() % 4) + 1)
	{
	case 1:
		filename = L"rock1.bmp";
		break;
	case 2:
		filename = L"rock2.bmp";
		break;
	case 3:
		filename = L"rock3.bmp";
		break;
	default:
		filename = L"rock4.bmp";
		break;
	}
	pNewGO->Initialise(new RenderComponent(pNewGO, filename),
					   new RockCollisionComponent(pNewGO, 50.0f),
					   position, velocity);
	//Flush temp filename wchar ptr
	filename = nullptr;

	//Attach custom components required by Rocks
	pNewGO->AddComponent(new PhysicsComponent(pNewGO, rotation));
	pNewGO->AddComponent(new ExpirationComponent(pNewGO, true));

	//Add GO reference to ObjectManager's pObjectList
	this->AddObject(pNewGO);
	return pNewGO;
}

GameObject* ObjectManager::CreateCow(Vector2D position, Vector2D velocity, float rotation)
{
	//Create new GO & initialise
	GameObject* pNewGO = new GameObject(this);
	pNewGO->Initialise(new RenderComponent(pNewGO, L"cow.bmp"), 
					   new CowCollisionComponent(pNewGO, 35.0f, 20.0f),
					   position, velocity);

	//Attach custom components required by Cows
	pNewGO->AddComponent(new PhysicsComponent(pNewGO, rotation));
	pNewGO->AddComponent(new ExpirationComponent(pNewGO, true));

	//Add GO reference to ObjectManager's pObjectList
	this->AddObject(pNewGO);
	return pNewGO;
}

GameObject* ObjectManager::CreateBullet(Vector2D position, Vector2D velocity)
{
	//Create new GO & initialise
	GameObject* pNewGO = new GameObject(this);
	pNewGO->Initialise(new RenderComponent(pNewGO, L"bullet.bmp", 3.0f), 
		               new BulletCollisionComponent(pNewGO, 1.0f, 1.0f),
		               position, velocity);

	//Attach custom components required by Bullets
	pNewGO->AddComponent(new PhysicsComponent(pNewGO));
	pNewGO->AddComponent(new ExpirationComponent(pNewGO));

	//Add GO reference to ObjectManager's pObjectList
	this->AddObject(pNewGO);
	return pNewGO;
}

//Orders all objects in list to update
void ObjectManager::UpdateAll()
{
	for (GameObject* pObject : pObjectList)
	{
		pObject->Update();
	}
	CheckAllCollisions();
}

//Delete any objects tagged as inactive
void ObjectManager::DeleteInactive()
{
	for (GameObject* &pObject : pObjectList)
	{
		if (pObject->isActive() == false)
		{
			delete pObject;    //Delete actual object
			pObject = nullptr; //Reset pointer
		}
	}
	auto it = std::remove(pObjectList.begin(), pObjectList.end(), nullptr);
	pObjectList.erase(it, pObjectList.end());
}

//Delete all objects in the list, irrespective of their current state.
void ObjectManager::DeleteAll()
{
	for (GameObject* &pObject : pObjectList)
	{
		delete pObject;    //Delete actual object
		pObject = nullptr; //Reset pointer
	}
	auto it = std::remove(pObjectList.begin(), pObjectList.end(), nullptr);
	pObjectList.erase(it, pObjectList.end());
}

//Checks all objects against eachother, detecting 
//which objects' collision shapes are intersecting
//Code itself is long-winded/hard to follow, comments attempt to stepthrough each segment
void ObjectManager::CheckAllCollisions(){
	std::list<GameObject*>::iterator it1;
	std::list<GameObject*>::iterator it2;
	//For each GameObject in the ObjectManager's pObjectList
	//  Select the next object in the list
	for (it1 = pObjectList.begin(); it1 != pObjectList.end(); it1++)
	{
		//Select the object AFTER it1 in the list
		for (it2 = std::next(it1); it2 != pObjectList.end(); it2++)
		{
			//Make sure both currently selected objects have a valid collision component, otherwise next step will fail
			if ((*it1)->GetCollision() && (*it1)->GetCollision())
			{
				//If the collisions shapes of the current objects in each list are overlapping
				if ((*it1)->GetCollision()->GetShape()->Intersects(*((*it2)->GetCollision()->GetShape())))
				{
					//Tell both objects' CollisionComponents to process collision, with reference to the opposing object
					(*it1)->GetCollision()->ProcessCollision(*it2);
					(*it2)->GetCollision()->ProcessCollision(*it1);
				}
			}
		}
	}
}