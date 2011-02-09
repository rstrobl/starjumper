#include "PlayerUpdater.h"
#include <iostream>

PlayerUpdater::PlayerUpdater(Player *player) :
    _player(player)
{
    // nothing more to do here
}

void PlayerUpdater::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
    PlayerState *playerState = _player->getPlayerState();
    osg::Vec3 newPosition = calculateNextPosition(playerState);
    _player->setPosition(newPosition);
  
    traverse(node, nv); 
}

osg::Vec3 PlayerUpdater::calculateNextPosition(PlayerState *playerState)
{
    btKinematicCharacterController *playerController = _player->getController();
    float speed = playerState->getSpeed();
    
    btVector3 direction = btVector3(0, 0, 0);
    
    if(playerState->requestMoveLeft())
        direction -= btVector3(0.2, 0, 0);
    
    if(playerState->requestMoveRight())
        direction += btVector3(0.2, 0, 0);
    
    if(playerState->requestAccelerate())
    {
        direction += btVector3(0, 1 * speed, 0);
        playerState->setSpeed(speed + 0.1 <= 2 ? speed + 0.1 : 2);
    }
    else if(playerState->requestDecelerate())
    {
        direction -= btVector3(0, 1 * speed, 0);
        playerState->setSpeed(speed - 0.3 >= 0 ? speed - 0.3 : 0);
    }
    else
    {
        direction += btVector3(0, 1 * speed, 0);
        playerState->setSpeed(speed - 0.1 >= 0 ? speed - 0.1 : 0);
    }
    
    if(playerState->requestJump())
        direction += btVector3(0, 0, 0.5);
    
    playerController->setWalkDirection(direction);
    
    btVector3 position = playerController->getGhostObject()->getWorldTransform().getOrigin();
    
    return osgbBullet::asOsgVec3(position);
}