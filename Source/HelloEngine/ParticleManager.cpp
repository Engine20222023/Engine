#include "Headers.h"
#include "ParticleManager.h"
#include "Emitter.h"
#include "Particle.h"
#include "ParticleSystemComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "LayerGame.h"
#include "BillBoardComponent.h"

ParticleManager::ParticleManager()
{

}

ParticleManager::~ParticleManager()
{

}

void ParticleManager::Init()
{
}

void ParticleManager::Draw()
{

	for (Emitter* emitter : EmitterList)
	{

		if (LayerGame::S_IsPlaying() || emitter->component->playOnScene)
		{
			//UPDATE EACH EMITTER

			if (emitter->component != nullptr)
				emitter->component->particleProps.position = emitter->component->_gameObject->transform->GetGlobalPosition();

			emitter->EmitParticles(emitter->component->particleProps);

			emitter->UpdateParticles(emitter->component->GetGameObject()->GetComponent<BillBoardComponent>()->GetBBRotation());

			// DRAW EACH EMITTER
			
			//var->Draw();
			
		}

	}
	
}

void ParticleManager::RemoveEmitterInList(Emitter* emitter)
{

	for (int i = 0; i < EmitterList.size(); i++)
	{
		if (EmitterList[i] == emitter)
		{
			EmitterList.erase(EmitterList.begin() + i);
		}
	}

}

void ParticleManager::RemoveAllEmitters()
{
	EmitterList.clear();
}


