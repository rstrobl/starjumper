#include "PlayerUpdater.h"

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
    _player->setAngles(0.0f, playerState->getAngleY());
  
    traverse(node, nv); 
}

osg::Vec3 PlayerUpdater::calculateNextPosition(PlayerState *playerState)
{
    KinematicCharacterController *playerController = _player->getController();
    float speed = playerState->getSpeed();
    float angleY = playerState->getAngleY();
    
    btVector3 direction = btVector3(0, 0, 0);
    
    // check for move requests
    
    if(playerState->requestMoveLeft())
    {
        direction -= btVector3(speed / 10.0f, 0, 0);
        playerState->setAngleY(angleY + 3 < 30 ? angleY + 3 : 30);
    }
    else if(playerState->requestMoveRight())
    {
        direction += btVector3(speed / 10.0f, 0, 0);
        playerState->setAngleY(angleY - 3 > -30 ? angleY - 3 : -30);
    }
    else
    {
        if(angleY > 0)
            playerState->setAngleY(angleY - 5 > 0 ? angleY - 5 : 0);
        else if(angleY < 0)
            playerState->setAngleY(angleY + 5 < 0 ? angleY + 5 : 0);
        else
            playerState->setAngleY(0);
    }
    
    if(playerState->requestAccelerate())
        playerState->setSpeed(speed + 0.02 <= 2 ? speed + 0.02 : 2);
    else if(playerState->requestDecelerate())
        playerState->setSpeed(speed - 0.04 >= 0 ? speed - 0.04 : 0);
//    else
//        playerState->setSpeed(speed - 0.05 >= 0 ? speed - 0.05 : 0);
    
    direction += btVector3(0, speed, 0);

    if(playerState->requestJump())
        playerController->jump();
    
    playerController->setWalkDirection(direction);
    
    btVector3 position = playerController->getGhostObject()->getWorldTransform().getOrigin();
    
    return osgbBullet::asOsgVec3(position);
}