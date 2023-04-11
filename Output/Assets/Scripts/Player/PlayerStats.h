#pragma once
#include "API/HelloBehavior.h"
#include "ScriptToInspectorInterface.h"
#include "Macro.h"

#include "API/API.h"

#include "PlayerStorage.h"

class PlayerMove;
class HpBar;
class HUD_Power_Up_Scrip;

class PlayerStats : HelloBehavior
{
public:
    void Start() override;
    void Update() override;

    void OnCollisionEnter(API_RigidBody other);

    float currentMaxHp;
    float maxHp;
    float upgradedMaxHp;
    float currentHp;
    void TakeDamage(float amount, float resistanceDamage);
    bool PlayerAlive() { return currentHp > 0.0f; }
    float shield;
    float maxResistance = 100.0f;
    float currentResistance;
    float inmunityTime = 2.0f;
    float hittedTime = 0.0f;
    bool secondLife;
    float lastHitTime;
    float deathTime = 0.0f;
    void Heal(float amount);
    API_ParticleSystem hitParticles;
    API_ParticleSystem healParticles;
    bool playingHealParticles;
    API_ParticleSystem aidKitParticles;

    int GetAmmonByType(int type);
    void GetAmmo(int type, int amount);
    void UseAmmo(int type, int amount = 1);

    int laserAmmo; // index 1
    int maxLaserAmmo;
    int specialAmmo;
    int maxFireAmmo; // index 2
    int maxRicochetAmmo; // index 3

    bool detected; //if the player is being detected somehow
    API_RigidBody actualZone;//the zone where the player is

    // skills values
    float deadlinePart = 20.0f;
    float upgradedDeadlinePart = 25.0f;
    float deadlineHeal = 5.0f;
    float upgradedDeadlineHeal = 7.5f;
    float aidKitHeal = 50.0f;
    float upgradedAidKitHeal = 75.0f;

    // skills tree
    void UpgradeTreeLvl(int tree);

    int movementTreeLvl; // tree 0
    int armoryTreeLvl; // tree 1
    int healthTreeLvl; // tree 2
    int specialTreeLvl; // tree 3

    // power ups
    float speedPowerUp; // 0
    float fireratePowerUp; // 1
    // shield // 2
    // max ammo // 3
    float slowTimePowerUp; // 4
    void GetPowerUp(int index);

    // storage
    API_GameObject playerGO;
    PlayerStorage* storage;
    void SaveInStorage(int index);
    void SaveChestData(int chestContent, int chestIndex);

    // move
    PlayerMove* playerMove;

    // HUD
    HUD_Power_Up_Scrip* hudPowerUp;
};