#pragma once

#include "InputBuffer.h"
#include "SensorCollider.h"
#include <SFML/System/Vector2.hpp>
#include "MathUtility.h"
#include "VectorUtility.h"
#include <SFML/Graphics/Drawable.hpp>
#include "GameState.h"
#include "Knockback.h"
#include "RaycastCollider.h"
#include "Platform.h"
#include "GameInfo.h"
#include "Platform2.h"
#include <memory>
#include <stack>

using namespace Physics;
using namespace sf;
using namespace CustomUtilities;

/// <summary>
/// Object that represents a playable character.
/// </summary>
class Character : public Drawable{

private:
    //Enumerators
    enum class SpatialState { STAND, WALK, DASH, CROUCH, AIR };
    enum class ActionState { IDLE, ATTACK, HIT, DEAD };

    //Fields
    InputBuffer buffer;
    SensorCollider sensor;
    RaycastCollider raycaster = RaycastCollider(Vector2f(),Vector2f(), 0);
    Vector2f _position, velocity, acceleration, _size, terminalVelocity; 

    Vector2f platformVelocity;
    float angle;
    SpatialState sState;
    ActionState aState;
    Knockback knockback = Knockback::none();

    //Defaults
    float gndAcceleration, gndDeceleration, friction, gravity, highJump, lowJump, walkSpeed, dashSpeed;
    float airAcceleration = 180.f;
    float dashTimer = 5.f;
    float legLength = 16 / GameInfo::pixelsPerUnit;

    //Future kinematics
    Vector2f fPosition;
    
    // =================== //
    // State Tracking Info //
    // =================== // 

    //Jump
    InputCode jumpButton = InputCode::NONE;

    // Ghost
    Vector2f ghostPosition;
    bool ghostIsAttached, isBreaking = false;
    stack<Vector2f> ghostPositions;
    float afterimageDelay = .01f;
    float lastAfterimageTime = 0;
    float breakingDuration = .22f, breakingStartTime;

    // Platforms
    Vector2f localPosition;
    Platform2* currentPlatform;

    //Methods

    /// <summary>
    /// Updates the input buffer, and then any
    /// </summary>
    void updateState();
    /// <summary>
    /// Updates the position, velocity, and acceleration of this <see cref="Character"/>.
    /// </summary>
    void updateKinematics();

    /// <summary>
    /// Checks for collisions, and if there are any, updates position to reflect these collisions.
    /// </summary>
    void updateCollisions(std::vector<PlatformPtr>& colliders, const std::vector<Platform>& platforms = {});

    void finishGhosting();
    void updateGhost(Vector2f& newGhostPosition);
    void startGhosting(Vector2f& newGhostPosition);

    bool isGrounded();

    /// <summary>
    /// Tells whether or not colliding against a narrow pathway.
    /// </summary>
    /// <param name="collision"></param>
    /// <returns></returns>
    bool isNarrow(const Collision& collision) const;

    /// <summary>
    /// Gets a code corresponding to a jump input if the button has been pressed this frame.
    /// Returns InputCode::NONE otherwise.
    /// </summary>
    InputCode checkJump();

    /// <summary>
    /// Tells whether or not this character can respond to user input.
    /// </summary>
    bool isIdle();

    /// <summary>
    /// Determines if the current input (Left vs Right) matches the current velocity.
    /// Returns 1 if moving in the same direction (i.e input == right and velocity.x >= 0).
    /// Returns -1 if moving in the opposite direction (i.e input == left and veloicty.x >= 0).
    /// Returns 0 if not inputting a left or right motion.
    /// </summary>
    int getMotionSign(); 
    std::pair<InputCode, int> getMotionInfo();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    Character();
    void update(vector<PlatformPtr>& pforms, vector<Platform> platforms = {});
    void setSize(Vector2f newSize);
    Vector2f position();
    void setPosition(const Vector2f& newPosition);

    /// <summary>
    /// Applies a hit to this character.
    /// </summary>
    void applyHit(float damage, float baseKnockback, float knockbackScaling, float angle);

};

Character::Character() : buffer(InputBuffer(10)) {
    //Set defaults.
    gndAcceleration = 240;
    gndDeceleration = 360;
    friction = gndAcceleration;
    gravity = 410;
    highJump = -130;
    lowJump = -65;
    walkSpeed = 72;
    dashSpeed = 96;
    sState = SpatialState::STAND;
    terminalVelocity = Vector2f(dashSpeed, -highJump);

    //Set up buffer.
    buffer.addBinding(new KeyboardBinding(Keyboard::Left, InputCode::LEFT));
    buffer.addBinding(new KeyboardBinding(Keyboard::Right, InputCode::RIGHT));
    buffer.addBinding(new KeyboardBinding(Keyboard::Space, InputCode::B1));
    buffer.addBinding(new ButtonBinding(0, 0, InputCode::B1));
    buffer.addBinding(new AxisBinding(0, Joystick::Axis::X, InputCode::RIGHT, InputCode::LEFT));
    buffer.addBinding(new AxisBinding(0, Joystick::Axis::Y, InputCode::DOWN, InputCode::UP));

    buffer.addBinding(new KeyboardBinding(Keyboard::S, InputCode::B7));
    buffer.addBinding(new KeyboardBinding(Keyboard::A, InputCode::B8));

    buffer.addBinding(new ButtonBinding(0, 4, InputCode::B5));
    buffer.addBinding(new ButtonBinding(0, 5, InputCode::B6));
    buffer.addBinding(new KeyboardBinding(Keyboard::Q, InputCode::B5));
    buffer.addBinding(new KeyboardBinding(Keyboard::W, InputCode::B6));

    
    //Set up sensor.
    sensor = SensorCollider(_position, Vector2f(_size.x/2, 5), Vector2f(_size.x, _size.x/2 + 16));
}
void Character::update(vector<PlatformPtr>& pforms, vector<Platform> platforms) {
    buffer.update();
    updateState();
    if (!isBreaking) {
        updateKinematics();
        updateCollisions(pforms, platforms);
    }
    finishGhosting();
    

    //if (VectorUtility::magnitude(velocity) != 0)cout << "Velocity: (" << velocity.x << "," << velocity.y << ")" << endl;
    //cout << "Angle: " << angle << endl;

    //if (buffer.current().count() > 0) std::cout << buffer.current() << std::endl;
}

void Character::updateState() {

    //Check if in hitstun.
    float dt = GameState::time().dt();
    FrameInput current = buffer.current();

    //Check for GHOST
    if (current.containsIgnoreStrength(PlayerInput(InputCode::B6, InputType::RELEASE)) && !ghostIsAttached && !isBreaking) {
        velocity = Vector2f();
        isBreaking = true;
        breakingStartTime = GameState::time().timestamp();
        return;
    }

    //Check for changes to horizontal acceleration due to input.
    /*int motionSign = getMotionSign();
    int vxSign = MathUtility::sign(velocity.x);
    if (velocity.x == 0 && current.isDown(InputCode::LEFT)) vxSign = -1;
    else if (velocity.x == 0 && current.isDown(InputCode::RIGHT)) vxSign = 1;
    if (motionSign == 1) acceleration.x = vxSign*gndAcceleration;
    else if (motionSign == -1) acceleration.x = -vxSign*gndDeceleration;
    else acceleration.x = -vxSign*min(abs(velocity.x)/dt,friction);*/

    //Check for changes in motion due to input.
    if (isIdle()) {
        std::pair<InputCode, int> motionInfo = getMotionInfo();
        if (isGrounded()) {
            acceleration.x = 0;
            if (motionInfo.first != InputCode::NONE) {
                float hSpeed = walkSpeed;
                if (current[motionInfo.first].strength() == 100.f
                    && (current[motionInfo.first].duration() <= dashTimer/(dt*60.f) || sState == SpatialState::DASH)) {
                    sState = SpatialState::DASH;
                    hSpeed = dashSpeed;
                }
                else sState = SpatialState::WALK;
                velocity.x = hSpeed * motionInfo.second * current[motionInfo.first].strength() / 100.f;
                terminalVelocity.x = hSpeed;
            }
            else velocity.x = 0;
        }
        else {
            //Else, in the air. In the air, speed is not set to zero if a direction is released.
            acceleration.x = motionInfo.second * airAcceleration;
            if (motionInfo.second != 0 && motionInfo.second != sign(velocity.x)) {
                velocity.x = 0.f;
            }
            //cout << toString(velocity) << endl;
        }

        //Check for changes to vertical velocity due to input.
        if (isGrounded() && checkJump() != InputCode::NONE) {
            velocity.y = highJump;
            jumpButton = checkJump();
            angle = 0;
        }
        else if (jumpButton != InputCode::NONE && velocity.y < lowJump &&
            current.containsIgnoreStrength(PlayerInput(jumpButton, InputType::RELEASE))) {
            velocity.y = lowJump;
            jumpButton = InputCode::NONE;
        }
    }
    
    // If in the air, apply gravity.
    if (sState == SpatialState::AIR) {
        acceleration.y = gravity;
    }
    else acceleration.y = 0;

    if (current.containsIgnoreStrength(PlayerInput(InputCode::B8, InputType::PRESS))) {
        applyHit(35, 24, 1.1f, degToRad(215));
        cout << knockback.toString() << endl;
    }

    //Check for PAUSE
    if (current.isDown(InputCode::B5)) {
        
        float delta =  4.f * GameState::time().dt();
        float gts = GameState::time().glitchedTimeScale();
        GameState::time().glitchedTimeScale(max(0.f, gts - delta));
    }
    else GameState::time().glitchedTimeScale(1);
}

void Character::updateKinematics() {
    float dt = GameState::time().dt();
    velocity += acceleration*dt;

    velocity = clamp(velocity, terminalVelocity*-1.f, terminalVelocity);

    Vector2f attunedVelocity;
    attunedVelocity.x = velocity.x * cosf(angle) + velocity.y * sinf(angle);
    attunedVelocity.y = velocity.x * -sinf(angle) + velocity.y * cosf(angle);

    Vector2f kbv = knockback.velocity(GameState::time().timestamp());
    attunedVelocity += kbv;

    
    //Set raycaster angle.
    float raycasterAngle = atan2f(attunedVelocity.y, attunedVelocity.x);
    if (magnitude(attunedVelocity) != 0)raycaster.angle(raycasterAngle);

    //if (magnitude(kbv) != 0) cout << toString(kbv) << endl;
    //Update future position within local space of current platform.
   

    // Now, update future position.
    fPosition = currentPlatform ? currentPlatform->transform().transformPoint(localPosition) : localPosition;
    fPosition += attunedVelocity*dt;
    
}

void Character::updateCollisions(std::vector<PlatformPtr>& colliders, const std::vector<Platform>& null) {
    //Check for GHOST
    bool ghosting = buffer.current().isDown(InputCode::B6);

    /*//Initial popout check.
    Line path = Line(_position, fPosition);
    bool intersection = false;
    float t = 1;

    for (const Platform& platform : platforms) {
        pair<bool, float> pillarIntersection = platform.pillars().intersects(path);
        if (pillarIntersection.first) {
            intersection = true;
            t = min(t, pillarIntersection.second);
        }
    }
    if (intersection) {
        Vector2f edgePoint = path.atPoint(path.intersects(construct(_position, _size)).second);
        if (ghosting && !(angle > degToRad(45) && angle < degToRad(135))) {
            if (ghostIsAttached) {
                ghostPosition = path.atPoint(t) - edgePoint + _position;
            }
        }
        else {
            fPosition = path.atPoint(t) - edgePoint + _position;
        }
        //cout << _position.y + _size.y / 2 << endl;
        cout << toString(path.atPoint(t)) << " " << toString(edgePoint) << endl;
    }*/
    
    //if (_position.y != fPosition.y) cout << _position.y + _size.y / 2 << " " << fPosition.y + _size.y / 2 << endl;
    

    sensor.setCenter(fPosition);

    //Update horizontal position based on left and right collisions.
    vector<pair<size_t, float>> lCollisionData = sensor.collides(colliders, SurfaceType::LEFT);
    vector<pair<size_t, float>> rCollisionData = sensor.collides(colliders, SurfaceType::RIGHT);
    bool hasRightCollision = rCollisionData.size() != 0, hasLeftCollision = lCollisionData.size() != 0;
    float rightCollision = hasRightCollision ? rCollisionData[rCollisionData.size() - 1].second : 0;
    float leftCollision = hasLeftCollision ? lCollisionData[lCollisionData.size() - 1].second : 0;
    bool horizontalCollision = false;
    float newX = 0;
    
    if (hasRightCollision && fPosition.x  < rightCollision) {
        newX = min(fPosition.x, rightCollision - _size.x / 2);     
        horizontalCollision = true;
    }
    else if (hasLeftCollision && fPosition.x > leftCollision) {
        newX = max(fPosition.x, leftCollision + _size.x / 2);
        horizontalCollision = true;
    }

    if (ghosting && horizontalCollision) {
        startGhosting(Vector2f(newX, ghostPosition.y));
    }
    else if (horizontalCollision) {
        fPosition.x = newX;
        velocity.x = 0;
    }
    
    //Update vertical position based on ground and ceiling collisions.

    //Check for low ceilings if jumping this frame.
    bool lowCeiling = isGrounded() && checkJump() != InputCode::NONE;

    sensor.setCenter(fPosition);

    vector<pair<size_t, float>> gCollisionData = sensor.collides(colliders, SurfaceType::GROUND);
    vector<pair<size_t, float>> cCollisionData = sensor.collides(colliders, SurfaceType::CEILING);
    bool hasGroundCollision = false, hasCeilingCollision = cCollisionData.size() != 0;
    float groundCollision = 0;
    float ceilingCollision = hasCeilingCollision ? cCollisionData[cCollisionData.size() - 1].second : 0;
    size_t pIndex = -1;

    for (pair<size_t, float> gCollision : gCollisionData) {
        if (ghosting || colliders[gCollision.first]->type() != PlatformType::SOLID) {
            //If moving through a nonsolid platform, only treat collisions at knee length or below as legitimate.
            if (fPosition.y + _size.y / 2 - legLength < gCollision.second) {
                if (!hasGroundCollision || gCollision.second < groundCollision) {
                    hasGroundCollision = true;
                    groundCollision = gCollision.second;
                    pIndex = gCollision.first;
                }
            }
        }
        else {
            //Else, stand on the highest platform possible.
            if (!hasGroundCollision || gCollision.second < groundCollision) {
                hasGroundCollision = true;
                groundCollision = gCollision.second;
                pIndex = gCollision.first;
            }
        }
    }

    if (!hasGroundCollision || (velocity + knockback.velocity(GameState::time().timestamp())).y < 0) {
        sState = SpatialState::AIR;
        angle = 0;
        platformVelocity = Vector2f();
        currentPlatform = nullptr;
    }

    if (hasGroundCollision && (velocity + knockback.velocity(GameState::time().timestamp())).y >= 0 &&
        (isGrounded() || fPosition.y + _size.y / 2 > groundCollision)) {
        fPosition.y = groundCollision - _size.y / 2;
        sensor.setCenter(fPosition);
        angle = sensor.groundAngle(colliders[pIndex]);
        velocity.y = 0;
        if (sState == SpatialState::AIR) {
            if (abs(velocity.x) == dashSpeed) sState = SpatialState::DASH;
            else if (velocity.x != 0) sState = SpatialState::WALK;
            else sState = SpatialState::STAND;
        }
        currentPlatform = colliders[pIndex].get();
    }
    else if (hasCeilingCollision && !isGrounded() && fPosition.y - _size.y/2 < ceilingCollision){
        float newY = ceilingCollision + _size.y / 2;
        if (ghosting) {
            startGhosting(Vector2f(ghostPosition.x, newY));
        }
        else {
            fPosition.y = newY;
            velocity.y = max(0.f, velocity.y);
        }        
    }

    //If passage is too narrow, restrict movement.
    sensor.setCenter(fPosition);
    bool tooNarrow = (hasGroundCollision && hasCeilingCollision &&
        groundCollision <= fPosition.y + _size.y / 2 && ceilingCollision >= fPosition.y - _size.y / 2) ||
        (hasLeftCollision && hasRightCollision &&
            leftCollision <= fPosition.x - _size.x / 2 && rightCollision >= fPosition.x + _size.x / 2);
  
    if (tooNarrow || lowCeiling) {
        if (ghosting) {
            startGhosting(_position);
        }
        else {
            fPosition = _position;
            //velocity.x = 0;
        }
    }

    if (!ghosting || ghostIsAttached || (!(hasCeilingCollision && fPosition.y - _size.y / 2 < ceilingCollision)
        && !horizontalCollision && !tooNarrow)) {
        ghostPosition = fPosition;
        ghostIsAttached = true;
    }
    _position = fPosition;

    sensor.setCenter(fPosition);
    raycaster.setOrigin(fPosition);

    if (currentPlatform) localPosition = currentPlatform->transform().getInverse().transformPoint(_position);
    else localPosition = _position;

    if (ghosting)  updateGhost(_position);

}

void Character::startGhosting(Vector2f& newGhostPosition) {
    if (!ghostIsAttached) return;
    else {
        ghostIsAttached = false;
        ghostPositions = {};
        updateGhost(newGhostPosition);
    }
}

void Character::updateGhost(Vector2f& newGhostPosition) {
    if (ghostIsAttached) return;
    if (GameState::time().timestamp() - lastAfterimageTime >= afterimageDelay) {
        ghostPositions.push(newGhostPosition);
        lastAfterimageTime = GameState::time().timestamp();
        cout << ghostPositions.size() << " " << toString(newGhostPosition) << endl;
    }
}
void Character::finishGhosting() {
    if (!isBreaking) return;
    /*_position = ghostPositions.top();
    ghostPositions.pop();
    isBreaking = ghostPositions.size() > 0;*/
    float t = min(breakingDuration, GameState::time().timestamp() - breakingStartTime) / breakingDuration;
    _position = t * ghostPosition + (1 - t)*fPosition;
    isBreaking = (t != 1);
    if (!isBreaking) {
        localPosition = _position;
        currentPlatform = nullptr;
    }
}
void Character::applyHit(float damage, float baseKnockback, float knockbackScaling, float angle) {
    knockback = knockback + Knockback(damage, baseKnockback, knockbackScaling, angle, GameState::time().timestamp());
    velocity = Vector2f();
    //aState = ActionState::HIT;
}

bool Character::isGrounded() {
    return sState != SpatialState::AIR;
}

bool Character::isNarrow(const Collision& collision) const {
    return (collision.withGround() && collision.withCeiling() &&
        collision.ground() <= fPosition.y + _size.y / 2 && collision.ceiling() >= fPosition.y - _size.y / 2) ||
        (collision.withLeft() && collision.withRight() &&
            collision.left() <= fPosition.x - _size.x / 2 && collision.right() >= fPosition.x + _size.x / 2);
}
InputCode Character::checkJump() {
    FrameInput current = buffer.current();
    if (current.containsIgnoreStrength(PlayerInput(InputCode::B1, InputType::PRESS))) return InputCode::B1;
    else if (current.isDown(InputCode::UP) && current[InputCode::UP].strength() >= 65.f &&
        current[InputCode::UP].duration() <= dashTimer / (GameState::time().dt()*60.f)) return InputCode::UP;
    else return InputCode::NONE;
}
bool Character::isIdle() {
    return !knockback.appliesHitstun(GameState::time().timestamp());
}
int Character::getMotionSign() {
    FrameInput current = buffer.current();
    if (!current.isDown(InputCode::LEFT) && !current.isDown(InputCode::RIGHT)) return 0;
    else if ((current.isDown(InputCode::RIGHT) && velocity.x >= 0) ||
        (current.isDown(InputCode::LEFT) && velocity.x <= 0)) return 1;
    else return -1;
}
std::pair<InputCode, int> Character::getMotionInfo() {
    FrameInput current = buffer.current();
    if (current.isDown(InputCode::RIGHT) && current[InputCode::RIGHT].duration() >= current[InputCode::LEFT].duration()) {
        return{ InputCode::RIGHT, 1 };
    }
    else if (current.isDown(InputCode::LEFT) && current[InputCode::LEFT].duration() >= current[InputCode::RIGHT].duration()) {
        return{ InputCode::LEFT, -1 };
    }
    else return{ InputCode::NONE, 0 };
}

void Character::draw(sf::RenderTarget& target, sf::RenderStates states) const { 
    target.draw(sensor, states);
    target.draw(raycaster, states);
    FloatRect R = construct(Vector2f(_position), Vector2f(_size));
    FloatRect ghostBbox = construct(ghostPosition, _size);
    CustomUtilities::draw(ghostBbox, Color::Cyan, target, states);
    CustomUtilities::draw(R, Color::White, target, states);

    CustomUtilities::draw(R, currentPlatform ? currentPlatform->transform().getInverse() : Transform(), 
        Color::Magenta, target, states);
    
}

void Character::setSize(Vector2f newSize) {
    _size = newSize;
    float ppu = GameInfo::pixelsPerUnit;
    sensor = SensorCollider(_position, Vector2f(_size.x / 2, 16/ppu), Vector2f(_size.x, _size.y/2 + legLength));

    raycaster = RaycastCollider(_position, Vector2f(_size.y, _size.x), pi / 2, 5);
}

Vector2f Character::position() {
    return _position;
}

void Character::setPosition(const Vector2f& newPosition) {
    _position = newPosition; 
    ghostPosition = newPosition;
    fPosition = newPosition;
}