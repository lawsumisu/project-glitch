#pragma once

#include "InputBuffer.h"
#include "SensorCollider.h"
#include <SFML/System/Vector2.hpp>
#include "MathUtility.h"
#include <SFML/Graphics/Drawable.hpp>

using namespace Physics;
using namespace sf;
using namespace CustomUtilities;

/// <summary>
/// Object that represents a playable character.
/// </summary>
class Character : public Drawable{

private:
    //Enumerators
    enum class SpatialState { STAND, RUN, CROUCH, AIR };
    enum class ActionState { IDLE, ATTACK, HIT, DEAD };

    //Fields
    InputBuffer buffer;
    SensorCollider sensor;
    Vector2f position, velocity, acceleration, _size; 
    float angle;
    SpatialState sState;
    ActionState aState;

    //Defaults
    float gndAcceleration, gndDeceleration, friction, gravity, highJump, lowJump;
    
    //Flags
    bool jumpFlag;
    //TODO jumpAngle

    //Methods

    /// <summary>
    /// Updates the input buffer, and then any
    /// </summary>
    void updateState(float dt);
    /// <summary>
    /// Updates the position, velocity, and acceleration of this <see cref="Character"/>
    /// </summary>
    void updateKinematics(float dt);

    /// <summary>
    /// Checks for collisions, and if there are any, updates position to reflect these collisions.
    /// </summary>
    void updateCollisions(std::vector<PillarCollider> colliders);

    bool isGrounded();

    /// <summary>
    /// Determines if the current input (Left vs Right) matches the current velocity.
    /// Returns 1 if moving in the same direction (i.e input == right and velocity.x >= 0).
    /// Returns -1 if moving in the opposite direction (i.e input == left and veloicty.x >= 0).
    /// Returns 0 if not inputting a left or right motion.
    /// </summary>
    int getMotionSign();   
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    Character();
    void update(vector<PillarCollider> colliders, float dt);
    void setSize(Vector2f newSize);
    void setPosition(Vector2f newPosition) { position = newPosition; }

};

Character::Character() : buffer(InputBuffer(10)) {
    //Set defaults.
    gndAcceleration = 180;
    gndDeceleration = 360;
    friction = 14;
    gravity = 720;
    highJump = -360;
    lowJump = -240;
    jumpFlag = false;
    sState = SpatialState::STAND;

    //Set up buffer.
    buffer.addBinding(new KeyboardBinding(Keyboard::Left, InputCode::LEFT));
    buffer.addBinding(new KeyboardBinding(Keyboard::Right, InputCode::RIGHT));
    buffer.addBinding(new KeyboardBinding(Keyboard::Space, InputCode::B1));
    
    //Set up sensor.
    sensor = SensorCollider(position, Vector2f(_size.x/2, 5), Vector2f(_size.x, _size.x/2 + 16));
}
void Character::update(vector<PillarCollider> colliders, float dt) {
    buffer.update();
    updateState(dt);
    updateKinematics(dt);
    updateCollisions(colliders);

    //cout << "Velocity: (" << velocity.x << "," << velocity.y << ")" << endl;
    //cout << "Angle: " << angle << endl;
}

void Character::updateState(float dt) {
    FrameInput current = buffer.current();

    //Check for changes to horizontal acceleration due to input.
    int motionSign = getMotionSign();
    int vxSign = MathUtility::sign(velocity.x);
    if (motionSign == 1) acceleration.x = vxSign*gndAcceleration;
    else if (motionSign == -1) acceleration.x = -vxSign*gndDeceleration;
    else acceleration.x = -vxSign*min(abs(velocity.x)/dt,friction);

    //Check for changes to vertical velocity due to input.
    if (isGrounded() && current.contains(PlayerInput(InputCode::B1, InputType::PRESS))) {
        velocity.y = highJump;
        jumpFlag = true;
    }
    else if (jumpFlag && velocity.y < lowJump && 
        current.contains(PlayerInput(InputCode::B1, InputType::RELEASE))){
        velocity.y = lowJump;
    }

    if (sState == SpatialState::AIR) {
        acceleration.y = gravity;
    }
    else acceleration.y = 0;
}

void Character::updateKinematics(float dt) {
    velocity += acceleration*dt;

    Vector2f attunedVelocity;
    attunedVelocity.x = velocity.x * cosf(angle) - velocity.y * sinf(angle);
    attunedVelocity.y = velocity.x * -sinf(angle) + velocity.y * cosf(angle);

    // Now, update position.
    position += attunedVelocity*dt;
}

void Character::updateCollisions(std::vector<PillarCollider> colliders) {
    sensor.setCenter(position);
    Collision collision = sensor.collides(colliders);
    cout << collision.toString() << endl;

    //Update horizontal position based on left and right collisions.
    if (collision.withRight()) {
        cout << "Colliding with right." << endl;
        position.x = min(position.x, collision.right() - _size.x / 2);
        velocity.x = 0;
    }
    else if (collision.withLeft()) {
        cout << "Colliding with left" << endl;
        position.x = max(position.x, collision.left() + _size.x / 2);
        velocity.x = 0;
    }

    //Update vertical position based on ground and ceiling collisions.
    if (collision.withGround() && velocity.y >= 0 && 
        (isGrounded() || position.y + _size.y / 2 > collision.ground())) {
        position.y = collision.ground() - _size.y / 2;
        angle = collision.groundAngle();
        velocity.y = 0;
        if (sState == SpatialState::AIR) sState = SpatialState::STAND;
    }
    else if (collision.withCeiling()){
        position.y = max(position.y, collision.ceiling() + _size.y / 2);
        velocity.y = 0;
    }

    if (!collision.withGround()) {
        sState = SpatialState::AIR;
        angle = 0;
    }

    sensor.setCenter(position);
}

bool Character::isGrounded() {
    return sState != SpatialState::AIR;
}

int Character::getMotionSign() {
    FrameInput current = buffer.current();
    if (!current.isDown(InputCode::LEFT) && !current.isDown(InputCode::RIGHT)) return 0;
    else if ((current.isDown(InputCode::RIGHT) && velocity.x >= 0) ||
        (current.isDown(InputCode::LEFT) && velocity.x <= 0)) return 1;
    else return -1;
}

void Character::draw(sf::RenderTarget& target, sf::RenderStates states) const { 
    target.draw(sensor, states);
    FloatRect R = RectUtility::construct(Vector2f(position), Vector2f(_size));
    RectUtility::draw(R, Color(Color::White), target, states);
}

void Character::setSize(Vector2f newSize) {
    _size = newSize;
    sensor = SensorCollider(position, Vector2f(_size.x / 2, 5), Vector2f(_size.x -2, _size.y / 2 + 16));
}