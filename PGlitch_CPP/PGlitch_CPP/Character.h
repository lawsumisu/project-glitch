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
#include "Platform.h"
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
    Vector2f initialCPosition, initialFPosition;
    
    // =================== //
    // State Tracking Info //
    // =================== // 

    //Jump
    InputCode jumpButton = InputCode::NONE;

    // Ghost
    Vector2f ghostPosition;
    bool ghostIsAttached, isBreaking = false;
    stack<Vector2f> ghostPositions;
    float afterimageDelay = .05f;
    float lastAfterimageTime = 0;
    float breakingDuration = .22f, breakingStartTime;
    vector<PlatformPtr> ghostingPlatforms;

    // Platforms
    Vector2f localPosition;
    Platform* currentPlatform;

    vector<pair<PlatformPtr, vector<Segment>>> currentSegments;
    

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
    void updateCollisions(std::vector<PlatformPtr>& platforms);

    void updateCollisions2(std::vector<PlatformPtr>& platforms);

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
    void update(vector<PlatformPtr>& platforms);
    void setSize(Vector2f newSize);
    Vector2f position();
    void setPosition(const Vector2f& newPosition);

    /// <summary>
    /// Applies a hit to this character.
    /// </summary>
    void applyHit(float damage, float baseKnockback, float knockbackScaling, float angle);

    void draw(sf::RenderTarget& target, sf::RenderStates states, bool debug = false) const;

};

Character::Character() : buffer(InputBuffer(10)) {
    //Set defaults.
    gndAcceleration = 240;
    gndDeceleration = 360;
    friction = gndAcceleration;
    gravity = 410;
    highJump = -160;
    lowJump = highJump/2;
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
void Character::update(vector<PlatformPtr>& pforms) {
    buffer.update();   
    updateState();
    finishGhosting();
    updateKinematics();
    updateCollisions2(pforms);
    
    

    //if (VectorUtility::magnitude(velocity) != 0)cout << "Velocity: (" << velocity.x << "," << velocity.y << ")" << endl;
    //cout << "Angle: " << angle << endl;

    //if (buffer.current().count() > 0) std::cout << buffer.current() << std::endl;
}

void Character::updateState() {
    if (isBreaking) return;
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
        applyHit(35, 24, 1.1f, degToRad(0));
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
    if (isBreaking) return;
    float dt = GameState::time().dt();
    velocity += acceleration*dt;

    velocity = clamp(velocity, terminalVelocity*-1.f, terminalVelocity);

    Vector2f attunedVelocity;
    attunedVelocity.x = velocity.x * cosf(angle) + velocity.y * sinf(angle);
    attunedVelocity.y = velocity.x * -sinf(angle) + velocity.y * cosf(angle);

    Vector2f kbv = knockback.velocity(GameState::time().timestamp());
    attunedVelocity += kbv;

    //if (magnitude(kbv) != 0) cout << toString(kbv) << endl;
    //Update future position within local space of current platform.
   

    // Now, update future position.
    fPosition = currentPlatform ? currentPlatform->transform().transformPoint(localPosition) : localPosition;
    fPosition += attunedVelocity*dt;
    
    initialFPosition = fPosition;
    initialCPosition = _position;

    //Set raycaster angle.
    Vector2f effectiveVelocity = (fPosition - _position) / dt;
    raycaster.angle(atan2f(effectiveVelocity.y, effectiveVelocity.x));
    //cout << raycaster.angle() << endl;
    
}

void Character::updateCollisions2(vector<PlatformPtr>& platforms) {
    //Check for GHOST
    if (isBreaking) return;
    bool ghosting = buffer.current().isDown(InputCode::B6);

    vector<pair<PlatformPtr, vector<Segment>>> collisions = sensor.collides(_position, fPosition, platforms);
    currentSegments = collisions;
    float xMin = -FLT_MAX, xMax = FLT_MAX, yMin = -FLT_MAX, yMax = FLT_MAX;

    //Update position based on potential collisions that occur through moving from current position to future position.
    //Check horizontal collisions.
    vector<pair<size_t, Vector2f>> rightNC = sensor.findNearestCollision(_position, fPosition, SurfaceType::RIGHT, collisions, xMin, xMax, yMin, yMax);
    vector<pair<size_t, Vector2f>> leftNC = sensor.findNearestCollision(_position, fPosition, SurfaceType::LEFT, collisions, xMin, xMax, yMin, yMax);
    float newX;
    if (rightNC.size() > 0) {
        xMax = rightNC[rightNC.size()-1].second.x;
        newX = clamp(xMax - _size.x / 2, _position.x, fPosition.x);
        //xMax = newX + _size.x / 2;
        cout << "On your right" << endl;
    }   
    else if (leftNC.size() > 0) {
        xMin = leftNC[leftNC.size()-1].second.x;
        newX = clamp(xMin + _size.x / 2, _position.x, fPosition.x);
        //xMin = newX - _size.x/2;
    }
    if (!ghosting && (rightNC.size() > 0 || leftNC.size() > 0)) {
        fPosition.x = newX;
        velocity.x = 0;
    }

    //Check vertical collisions.
    vector<pair<size_t, Vector2f>> groundNC = sensor.findNearestCollision(_position, fPosition, SurfaceType::GROUND, collisions, xMin, xMax, yMin, yMax);
    vector<pair<size_t, Vector2f>> ceilingNC = sensor.findNearestCollision(_position, fPosition, SurfaceType::CEILING, collisions, xMin, xMax, yMin, yMax); 
    if (groundNC.size() > 0 && (velocity + knockback.velocity(GameState::time().timestamp())).y >= 0) {
        //Need to extract the index for the platform whose ground is highest without the player being caught within it.
        int groundIndex = -1;
        float highestGround = FLT_MAX;
        for (size_t i = 0; i < groundNC.size() - 1; ++i) {
            if (groundNC[i].second.y < highestGround && !sensor.within(Vector2f(fPosition.x, groundNC[i].second.y - _size.y/2), 
                platforms[groundNC[i].first])) {
                highestGround = groundNC[i].second.y;
                groundIndex = i;
            }
        }
        if (groundIndex >= 0) {
            yMax = groundNC[groundIndex].second.y;
            fPosition.y = clamp(yMax - _size.y / 2, _position.y, fPosition.y);
        }       
    }
    else if (ceilingNC.size() > 0) {
        yMin = ceilingNC[ceilingNC.size()-1].second.y;
    }

    //Now, update position based on objects colliding with player at future position.
    Vector2f modifiedFPosition = fPosition;
    if (ghosting && (leftNC.size() > 0 || rightNC.size() > 0)) {
        modifiedFPosition.x = newX;
    }
    //Horizontal Checks
    vector<pair<size_t, Vector2f>> rightNS = sensor.findNearestSurface(modifiedFPosition, SurfaceType::RIGHT, collisions);
    vector<pair<size_t, Vector2f>> leftNS = sensor.findNearestSurface(modifiedFPosition, SurfaceType::LEFT, collisions);
    bool wallOnRight = rightNS.size() > 0, wallOnLeft = leftNS.size() > 0;
    if (wallOnRight) {
        newX = rightNS[rightNS.size()-1].second.x - _size.x / 2;
        //cout << "On your right." << endl;
    }
    else if (wallOnLeft) {
        newX = leftNS[leftNS.size()-1].second.x + _size.x / 2;
        //cout << "On your left." << endl;
    }

    // If ghosting, pass through collisions, but set ghost if necessary.
    if (ghosting && (wallOnLeft || wallOnRight || rightNC.size() > 0 || leftNC.size() > 0)) {
        startGhosting(Vector2f(newX, ghostPosition.y));
    }
    else if (wallOnLeft || wallOnRight) {
        fPosition.x = newX;
        velocity.x = 0;
    }

    //Vertical Checks
    vector<pair<size_t, Vector2f>> groundNS = sensor.findNearestSurface(fPosition, SurfaceType::GROUND, collisions);
    vector<pair<size_t, Vector2f>> ceilingNS = sensor.findNearestSurface(fPosition, SurfaceType::CEILING, collisions);
    //cout << (groundNS.first >= 0) << " " << ((velocity + knockback.velocity(GameState::time().timestamp())).y >= 0) << " " <<
       // (isGrounded() || fPosition.y + _size.y / 2 >= groundNS.second.y) << endl;

    //If found no steppable terrain, or ascending, assume that currently in the air.
    if (groundNS.size() == 0 || (velocity + knockback.velocity(GameState::time().timestamp())).y < 0) {
        sState = SpatialState::AIR;
        angle = 0;
        platformVelocity = Vector2f();
        currentPlatform = nullptr;
        //cout << toString(groundNC.second) << endl;
    }
    /*sensor.setCenter(fPosition);
    vector<pair<size_t, float>> gCollisionData = sensor.collides(platforms, SurfaceType::GROUND);*/
    if (groundNS.size() > 0 && (velocity + knockback.velocity(GameState::time().timestamp())).y >= 0 &&
        (isGrounded() || fPosition.y + _size.y / 2 >= groundNS[groundNS.size()-1].second.y)) {
        //Need to extract the index for the platform whose ground is highest without the player being caught within it.
        int groundIndex = -1;
        float highestGround = FLT_MAX;
        for (size_t i = 0; i < groundNS.size() - 1; ++i) {
            if (groundNS[i].second.y < highestGround &&
                !sensor.within(Vector2f(fPosition.x, groundNS[i].second.y - _size.y/2), platforms[groundNS[i].first])) {
                highestGround = groundNS[i].second.y;
                groundIndex = i;
            }
        }
        if (groundIndex >= 0) {
            size_t gi = groundIndex;
            fPosition.y = groundNS[gi].second.y - _size.y / 2;
            sensor.setCenter(fPosition);
            angle = sensor.groundAngle(platforms[groundNS[gi].first]);
            // if (angle == 0) cout << sensor.getTranslationBounds(initialCPosition, initialFPosition) << endl;
            currentPlatform = platforms[groundNS[gi].first].get();
            velocity.y = 0;
            if (sState == SpatialState::AIR) {
                if (abs(velocity.x) == dashSpeed) sState = SpatialState::DASH;
                else if (velocity.x != 0) sState = SpatialState::WALK;
                else sState = SpatialState::STAND;
            }
        }
       
    }
    else if (ceilingNS.size() > 0 && !isGrounded() && (velocity + knockback.velocity(GameState::time().timestamp())).y < 0) {
        float newY = ceilingNS[ceilingNS.size()-1].second.y + _size.y / 2;
        if (ghosting) {
            startGhosting(Vector2f(ghostPosition.x, newY));
        }
        else {
            fPosition.y = newY;
            velocity.y = max(0.f, velocity.y);
        }
    }
    

    _position = fPosition;
    sensor.setCenter(_position);
    if (currentPlatform) localPosition = currentPlatform->transform().getInverse().transformPoint(_position);
    else localPosition = _position;
    
    //Update platforms currently ghosting through.
    ghostingPlatforms = sensor.within(fPosition, platforms);
    updateGhost(_position);

 }
void Character::updateCollisions(std::vector<PlatformPtr>& colliders) {
    if (isBreaking) return;
    //Check for GHOST
    bool ghosting = buffer.current().isDown(InputCode::B6);

    currentSegments = sensor.collides(_position, fPosition, colliders);
    

    //Initial popout check.
    Segment path = Segment(_position, raycaster.angle(), _size.y/2);
    Vector2f newPathEnd = path.atPoint(path.intersects(construct(_position, _size)).second)+fPosition - _position;
    path = Segment(_position, newPathEnd);
    bool intersection = false;
    float t = 1;

    for (auto& platform : colliders) {
        pair<bool, float> pillarIntersection = platform->collides(path);
        if (pillarIntersection.first) {
            intersection = true;
            t = min(t, pillarIntersection.second);
        }
    }
    if (intersection) {
        //cout << path.toString() << " " << toString(construct(_position, _size)) << endl;
        pair<bool, float> edgeIntersection = path.intersects(construct(_position, _size));
        //cout << edgeIntersection.first << endl;
        Vector2f edgePoint = path.atPoint(path.intersects(construct(_position, _size)).second);
        if (ghosting && !(angle > degToRad(45) && angle < degToRad(135))) {
            if (ghostIsAttached) {
                ghostPosition = path.atPoint(t) - edgePoint + _position;
            }
        }
        else {
            fPosition += path.atPoint(t) - path.end();
        }
        //cout << _position.y + _size.y / 2 << endl;
        //cout << toString(_position) << " " << toString(path.atPoint(t)) << " " << toString(path.end()) << endl;
    }
    
    //if (_position.y != fPosition.y) cout << _position.y + _size.y / 2 << " " << fPosition.y + _size.y / 2 << endl;
    

    sensor.setCenter(fPosition);

    //Update horizontal position based on left and right collisions.
    vector<pair<size_t, float>> lCollisionData = sensor.collides(colliders, SurfaceType::LEFT);
    vector<pair<size_t, float>> rCollisionData = sensor.collides(colliders, SurfaceType::RIGHT);
    bool hasRightCollision = rCollisionData.size() != 0, hasLeftCollision = lCollisionData.size() != 0;
    float rightCollision = hasRightCollision ? rCollisionData[rCollisionData.size() - 1].second : 0;
    float leftCollision = hasLeftCollision ? lCollisionData[lCollisionData.size() - 1].second : 0;
    bool wallOnRight = hasRightCollision && fPosition.x - _size.x / 2 < rightCollision;
    bool wallOnLeft = hasLeftCollision && fPosition.x + _size.x / 2> leftCollision;
    float newX = 0;
    
    //Prioritize right side collisions over left.
    if (wallOnRight) {
        newX = min(fPosition.x, rightCollision - _size.x / 2);
        cout << "On your right?" << " " << rightCollision-fPosition.x <<  endl;
    }
    else if (wallOnLeft) {
        newX = max(fPosition.x, leftCollision + _size.x / 2);
        cout << "On your left" << endl;
    }

    // If ghosting, pass through collisions, but set ghost if necessary.
    if (ghosting && (wallOnLeft || wallOnRight)) {
        startGhosting(Vector2f(newX, ghostPosition.y));
    }
    else if (wallOnLeft || wallOnRight) {
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
    float ceilingCollision = hasCeilingCollision ? cCollisionData[cCollisionData.size() - 1].second : 0;
    bool wallAbove = hasCeilingCollision && fPosition.y + _size.y / 2 > ceilingCollision;

    //With the ground collision data, need to detect if the highest terrain that is steppable.
    bool steppableBelow = false;
    float groundCollision = 0;
    size_t platformIndex = -1;
    for (pair<size_t, float> gCollision : gCollisionData) {
        if (fPosition.y + _size.y/2 -legLength  < gCollision.second && (!steppableBelow || (gCollision.second < groundCollision))) {
            steppableBelow = true;
            groundCollision = gCollision.second;
            platformIndex = gCollision.first;
            
        }
    }
    float fAngle = 0;
    if (steppableBelow) fAngle = sensor.groundAngle(colliders[platformIndex]);
    //If found no steppable terrain, or ascending, assume that currently in the air.
    if (!steppableBelow || (velocity + knockback.velocity(GameState::time().timestamp())).y < 0) {
        sState = SpatialState::AIR;
        angle = 0;
        platformVelocity = Vector2f();
        currentPlatform = nullptr;
    }


    //Check to see if ground can be acquired.
    if (steppableBelow && (velocity + knockback.velocity(GameState::time().timestamp())).y >= 0 &&
        (isGrounded() || fPosition.y + _size.y / 2 > groundCollision)&& abs(fAngle) < .9f) {       
        fPosition.y = groundCollision - _size.y / 2;
        sensor.setCenter(fPosition);
        angle = sensor.groundAngle(colliders[platformIndex]);
        velocity.y = 0;
        if (sState == SpatialState::AIR) {
            if (abs(velocity.x) == dashSpeed) sState = SpatialState::DASH;
            else if (velocity.x != 0) sState = SpatialState::WALK;
            else sState = SpatialState::STAND;
        }
        currentPlatform = colliders[platformIndex].get();
    }
    //Else, check for ceiling collisions.
    else if (wallAbove && !isGrounded() && (velocity + knockback.velocity(GameState::time().timestamp())).y < 0){
        cout << ceilingCollision << endl;
        float newY = ceilingCollision + _size.y / 2;
        if (ghosting) {
            startGhosting(Vector2f(ghostPosition.x, newY));
        }
        else {
            fPosition.y = newY;
            velocity.y = max(0.f, velocity.y);
        }
        hasCeilingCollision = false;
    }

    //If passage is too narrow, restrict movement.
    /*bool tooNarrow = (isSteppable && hasCeilingCollision &&
        groundCollision <= fPosition.y + _size.y / 2 && ceilingCollision >= fPosition.y - _size.y / 2) ||
        (hasLeftCollision && hasRightCollision &&
            leftCollision <= fPosition.x - _size.x / 2 && rightCollision >= fPosition.x + _size.x / 2);*/
    
    //Check for narrow passage ways.
    
    if (lowCeiling) {
        if (ghosting) {
            startGhosting(_position);
        }
        else {
            fPosition = _position;
            //velocity.x = 0;
        }
    }

    /*sensor.setCenter(fPosition);
    if (wallOnLeft && wallOnRight && (!steppableBelow || !wallAbove)) {
        vector<pair<size_t, float>> scCollisionData = sensor.checkSecondaryCollisions(colliders, SurfaceType::CEILING);
        vector<pair<size_t, float>> sgCollisionData = sensor.checkSecondaryCollisions(colliders, SurfaceType::GROUND);

        if (scCollisionData.size() != 0 && sgCollisionData.size() != 0) {
            cout << "Squished horizontally!" << endl;
        }
        else if (scCollisionData.size() != 0 && !wallAbove) {
            //fPosition.y = scCollisionData[scCollisionData.size() - 1].second + _size.y / 2;
        }
        else if (sgCollisionData.size() != 0 && !steppableBelow) {
            //fPosition.y = sgCollisionData[sgCollisionData.size() - 1].second - _size.y / 2;
            //TODO plaform acquisition,etc.
        }
    }
    else if (steppableBelow && wallAbove && (!wallOnLeft || !wallOnRight)) {
        vector<pair<size_t, float>> srCollisionData = sensor.checkSecondaryCollisions(colliders, SurfaceType::RIGHT);
        vector<pair<size_t, float>> slCollisionData = sensor.checkSecondaryCollisions(colliders, SurfaceType::LEFT);
        //cout << "Narrow" << endl;
        if (srCollisionData.size() != 0 && slCollisionData.size() != 0) {
            //cout << "Squished vertically!" << endl;
        }
        else if (srCollisionData.size() != 0 && !wallOnRight) {
            cout << "Right" << endl;
            fPosition.x = srCollisionData[srCollisionData.size() - 1].second - _size.x / 2;
        }
        else if (slCollisionData.size() != 0) {
            cout << slCollisionData[slCollisionData.size() - 1].second << endl;
            fPosition.x = slCollisionData[slCollisionData.size() - 1].second + _size.x / 2;
        }
    }*/
    //Reset ghost's position if there are no current collisions besides ones with steppable terrain.
    if (!ghosting || ghostIsAttached || (!(hasCeilingCollision && fPosition.y - _size.y / 2 < ceilingCollision)
        && !(hasRightCollision || hasLeftCollision) && !(groundCollision && !steppableBelow))) {
        ghostPosition = fPosition;
        ghostIsAttached = true;
    }

    //Finally set new position based on found collisions.
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
        ghostPositions = {};
        ghostPosition = newGhostPosition;
        ghostIsAttached = false;
    }
}

void Character::updateGhost(Vector2f& newGhostPosition) {
    ghostIsAttached = ghostIsAttached || ghostingPlatforms.size() == 0;
    if (ghostIsAttached) {
        ghostPosition = _position;
        return;
    }
    else if (GameState::time().timestamp() - lastAfterimageTime >= afterimageDelay) {
        ghostPositions.push(newGhostPosition);
        lastAfterimageTime = GameState::time().timestamp();
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
    acceleration = Vector2f();
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

void Character::draw(sf::RenderTarget& target, sf::RenderStates states, bool debug) const {
    target.draw(sensor, states); 
    FloatRect R = construct(_position, _size);
    CustomUtilities::draw(R, Color::White, target, states);
    if (debug) {
        //target.draw(raycaster, states);
        FloatRect ghostBbox = construct(ghostPosition, _size);
        CustomUtilities::draw(ghostBbox, Color::Cyan, target, states);
        CustomUtilities::draw(R, currentPlatform ? currentPlatform->transform().getInverse() : Transform(),
            Color::Magenta, target, states);
        Segment ray(_position, raycaster.angle(), _size.y / 2);
        ray.draw(Color::Red, target, states);
        for (pair<PlatformPtr, vector<Segment>> segmentList : currentSegments) {
            for (auto& s : segmentList.second) s.draw(Color::Yellow, target, states);
        }
        for (auto& ptr : ghostingPlatforms) {
            ptr->draw(Color::Cyan, target, states);
        }
    }   
    
    //sensor.getCollider(initialCPosition, initialFPosition).draw(target, states);
    //cout << currentSegments.size() << endl;
}
void Character::draw(sf::RenderTarget& target, sf::RenderStates states) const { 
    draw(target, states, true);
    
}

void Character::setSize(Vector2f newSize) {
    _size = newSize;
    float ppu = GameInfo::pixelsPerUnit;
    //sensor = SensorCollider(_position, _size, Vector2f(_size.x / 2, 36/ppu), Vector2f(_size.x - 1 , _size.y/2 + legLength));
    sensor = SensorCollider(_position, _size, 18 / ppu, legLength);

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