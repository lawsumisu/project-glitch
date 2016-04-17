#pragma once

#include "InputBuffer.h"
#include "SensorCollider.h"
#include <SFML/System/Vector2.hpp>
#include "MathUtility.h"
#include "VectorUtility.h"
#include <SFML/Graphics/Drawable.hpp>
#include "GameState.h"
#include "Knockback.h"

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
    Vector2f _position, velocity, acceleration, _size, terminalVelocity; 
    float angle;
    SpatialState sState;
    ActionState aState;
    Knockback knockback = Knockback::none();

    //Defaults
    float gndAcceleration, gndDeceleration, friction, gravity, highJump, lowJump, walkSpeed, dashSpeed;
    float airAcceleration = 180.f;
    float dashTimer = 5.f;
    
    // State Tracking Info: Jump
    InputCode jumpButton = InputCode::NONE;
    //TODO jumpAngle

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
    void updateCollisions(std::vector<PillarCollider> colliders);

    bool isGrounded();

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
    void update(vector<PillarCollider> colliders);
    void setSize(Vector2f newSize);
    Vector2f position();
    void setPosition(Vector2f newPosition) { _position = newPosition; }

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

    buffer.addBinding(new KeyboardBinding(Keyboard::A, InputCode::B5));
    buffer.addBinding(new KeyboardBinding(Keyboard::S, InputCode::B7));
    buffer.addBinding(new KeyboardBinding(Keyboard::D, InputCode::B6));
    buffer.addBinding(new KeyboardBinding(Keyboard::W, InputCode::B8));

    
    //Set up sensor.
    sensor = SensorCollider(_position, Vector2f(_size.x/2, 5), Vector2f(_size.x, _size.x/2 + 16));
}
void Character::update(vector<PillarCollider> colliders) {
    buffer.update();
    updateState();
    updateKinematics();
    updateCollisions(colliders);

    //if (VectorUtility::magnitude(velocity) != 0)cout << "Velocity: (" << velocity.x << "," << velocity.y << ")" << endl;
    //cout << "Angle: " << angle << endl;

    //if (buffer.current().count() > 0) std::cout << buffer.current() << std::endl;
}

void Character::updateState() {
    //Check if in hitstun.
    float dt = GameState::time().dt();
    FrameInput current = buffer.current();

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
            //cout << current[motionInfo.first].strength()<< endl;
        }



        //Check for changes to vertical velocity due to input.
        if (isGrounded() && checkJump() != InputCode::NONE) {
            velocity.y = highJump;
            jumpButton = checkJump();
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

    if (current.containsIgnoreStrength(PlayerInput(InputCode::B5, InputType::PRESS))) {
        applyHit(35, 24, 1.1f, degToRad(215));
        cout << knockback.toString() << endl;
    }
}

void Character::updateKinematics() {
    float dt = GameState::time().dt();
    velocity += acceleration*dt;

    velocity = clamp(velocity, terminalVelocity*-1.f, terminalVelocity);

    Vector2f attunedVelocity;
    attunedVelocity.x = velocity.x * cosf(angle) + velocity.y * sinf(angle);
    attunedVelocity.y = velocity.x * -sinf(angle) + velocity.y * cosf(angle);

    Vector2f v = knockback.velocity(GameState::time().timestamp());
    attunedVelocity += v;

    //if (VectorUtility::magnitude(v) != 0) cout << VectorUtility::toString(v) << endl;
    // Now, update position.
    _position += attunedVelocity*dt*4.f;
}

void Character::updateCollisions(std::vector<PillarCollider> colliders) {
    sensor.setCenter(_position);
    Collision collision = sensor.collides(colliders);

    //Update horizontal position based on left and right collisions.
    if (collision.withRight()) {
        _position.x = min(_position.x, collision.right() - _size.x / 2);
        velocity.x = 0;
    }
    else if (collision.withLeft()) {
        _position.x = max(_position.x, collision.left() + _size.x / 2);
        velocity.x = 0;
    }

    //Update vertical position based on ground and ceiling collisions.
    sensor.setCenter(_position);
    collision = sensor.collides(colliders);
    if (collision.withGround() && velocity.y >= 0 && 
        (isGrounded() || _position.y + _size.y / 2 > collision.ground())) {
        _position.y = collision.ground() - _size.y / 2;
        angle = collision.groundAngle();
        velocity.y = 0;
        if (sState == SpatialState::AIR) {
            if (abs(velocity.x) == dashSpeed) sState = SpatialState::DASH;
            else if (velocity.x != 0) sState = SpatialState::WALK;
            else sState = SpatialState::STAND;
        }
    }
    else if (collision.withCeiling()){
        _position.y = max(_position.y, collision.ceiling() + _size.y / 2);
        velocity.y = max(0.f, velocity.y);
    }

    if (!collision.withGround()) {
        sState = SpatialState::AIR;
        angle = 0;
    }

    sensor.setCenter(_position);
}

void Character::applyHit(float damage, float baseKnockback, float knockbackScaling, float angle) {
    knockback = knockback + Knockback(damage, baseKnockback, knockbackScaling, angle, GameState::time().timestamp());
    velocity = Vector2f();
    //aState = ActionState::HIT;
}
bool Character::isGrounded() {
    return sState != SpatialState::AIR;
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
    FloatRect R = construct(Vector2f(_position), Vector2f(_size));
    CustomUtilities::draw(R, Color(Color::White), target, states);
}

void Character::setSize(Vector2f newSize) {
    _size = newSize;
    sensor = SensorCollider(_position, Vector2f(_size.x / 2, 5), Vector2f(_size.x -2, _size.y / 2 + 16));
}

Vector2f Character::position() {
    return _position;
}