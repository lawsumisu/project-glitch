#pragma once

#include <SFML/System/Vector2.hpp>
#include <sstream>
#include <iostream>
#include <math.h>

using namespace sf;
using namespace std;
namespace Physics {
    class Knockback;
    Knockback operator+(const Knockback& kb1, const Knockback& kb2);
    bool operator==(const Knockback& kb1, const Knockback& kb2);
    /// <summary>
    /// Class that represents information regarding properties of an object receiving knockback, 
    /// such as hitlag and hitstun duration, and knockback strength and duration over time.
    /// Immutable.
    /// </summary>
    class Knockback {
    private:
        //Fields

        // Angle of hit in radians.
        float angle;

        // Initial knockback of the hit.
        Vector2f originalKnockback;

        //How long in seconds for which this knockback is non-zero.
        float duration;

        float hitstun;

        // Time (in seconds) hit was received on.
        // RI: >= 0.
        float timestamp;

        //Constructor
        Knockback(Vector2f& knockback, float timestamp);

        // How much knockback should decay each frame.
        static const float knockbackDecay;

        // What proportion of duration correponds to hitstun frames.
        static const float hitstunFactor;

        // What proportion of knockback corresponds to launch speed.
        static const float launchSpeedFactor;

        //Methods
        /// <summary>
        /// Calculates the knockback of an attack.
        /// </summary>
        Vector2f calculateKnockback(float damage, float baseKnockback, float knockbackScaling, float angle) const;
    public:
        //Constructor
        Knockback(float damage, float baseKnockback, float knockbackScaling, float angle, float timestamp);

        /// <summary>
        /// Gets the knockback velocity of this <see cref="Knockback"/> with respect to the number of frames it's been active.
        /// Requires that the timestamp be >= 0.
        /// </summary>
        /// <returns></returns>
        Vector2f velocity(float timestamp) const;

        /// <summary>
        /// Gets the knockback strength with respect to the # of frames this <see cref="Knockback"/> has been active.
        /// Requires that the timestamp be >=0.
        /// </summary>
        /// <param name="frameCount"></param>
        /// <returns></returns>
        Vector2f knockback(float timestamp) const;

        /// <summary>
        /// Tells whether or not this <see cref="Knockback"/> applies hitstun at a specified time.
        /// </summary>
        bool appliesHitstun(float timestamp) const;

        string toString() const;
        /// <summary>
        /// Returns a new <see cref="Knockback"/> that is the result of adding a <see cref="Knockback"/> to this one.
        /// Knockbacks are added differently depending on a variety of factors, including # of frames between knockbacks and 
        /// relative strengths.
        /// TODO: expand spec
        /// </summary>
        /// <param name="kb"></param>
        /// <returns></returns>
        friend Knockback Physics::operator+(const Knockback& kb1, const Knockback& kb2);

        friend bool Physics::operator==(const Knockback& kb1, const Knockback& kb2);

        static Knockback& none();

    };
}

#include "VectorUtility.h"

using namespace Physics;
using namespace CustomUtilities;

const float Knockback::knockbackDecay = 100.f;
const float Knockback::hitstunFactor = .66f;
const float Knockback::launchSpeedFactor = 1.8f;

Knockback::Knockback(Vector2f& knockback, float timestamp) {
    this->angle = atan2f(knockback.y, knockback.x);
    this->originalKnockback = knockback;
    this->timestamp = timestamp;
    this->duration = magnitude(knockback) / knockbackDecay;
    hitstun = hitstunFactor * duration;
}

Knockback::Knockback(float damage, float baseKnockback, float knockbackScaling, float angle, float timestamp) :
    Knockback(calculateKnockback(damage, baseKnockback, knockbackScaling, angle), timestamp) {}

Vector2f Knockback::calculateKnockback(float damage, float baseKnockback, float knockbackScaling, float angle) const{
    static float dFactor = .05f;
    static float additiveDamage = 18.f;
    float magnitude = (damage * damage * dFactor + additiveDamage) * knockbackScaling + baseKnockback;
    return Vector2f(cosf(angle), sinf(angle))*magnitude;
}
Vector2f Knockback::knockback(float timestamp) const {
    float timeDifference = timestamp - this->timestamp;
    if (timeDifference >= duration) return Vector2f();
    return originalKnockback - Vector2f(cosf(angle), sinf(angle)) * knockbackDecay * timeDifference;
}

Vector2f Knockback::velocity(float timestamp) const {
    return knockback(timestamp) * launchSpeedFactor;
}

bool Knockback::appliesHitstun(float timestamp) const {
    if (timestamp < this->timestamp) return false;
    else return timestamp - this->timestamp < hitstun;
}
Knockback Physics::operator+(const Knockback& kb1, const Knockback& kb2)  {
    if (kb1 == Knockback::none()) return kb2;
    else if (kb2 == Knockback::none()) return kb1;
    else if (kb1.timestamp > kb2.timestamp) return kb2 + kb1;

    //kb1.timestamp <= kb2.timestamp
    float maxTimeDifference = 1.f/6;
    if (kb2.timestamp - kb1.timestamp >= maxTimeDifference) {
        return Knockback(kb2.originalKnockback + kb1.knockback(kb2.timestamp), kb2.timestamp);
    }
    else return kb2;
}

Knockback& Knockback::none() {
    static Knockback& kb = Knockback(Vector2f(), 0);
    return kb;
}

bool Physics::operator==(const Knockback& kb1, const Knockback& kb2) {
    if (kb1.originalKnockback == Vector2f() && kb1.originalKnockback == kb2.originalKnockback) return true;
    else return kb1.originalKnockback == kb2.originalKnockback && kb1.timestamp == kb2.timestamp;
}

string Knockback::toString() const {
    stringstream ss;
    ss << "Knockback:[strength: " << magnitude(originalKnockback) <<
        ", angle: " << angle <<
        ", hitstun: " << hitstun << "s, duration: " << duration << "s]";
    return ss.str();
}