import * as Phaser from 'phaser';
import { Point, Vector2 } from 'src/utilities/vector/vector';
import { GameInput } from 'src/plugins/gameInput.plugin';
import { Platform, PlatformType } from 'src/platform';
import { Scalar } from 'src/utilities/math/scalar.util';
import { AfterimageData } from 'src/afterimage';
import { Level } from 'src/levels';

export enum PlayerAnimation {
  IDLE = 'IDLE',
  WALK = 'WALK',
  JUMP = 'JUMP',
  LAND = 'LAND',
  FALL = 'FALL',
  ATK1 = 'ATK1',
  ATK2 = 'ATK2',
  WALLSLIDE = 'WALLSLIDE',
}

enum PlayerState {
  IDLE = 'IDLE',
  ATTACK = 'ATTACK',
}

interface CollisionData {
  platform: Platform | null;
  value: number;
}

export class Player {
  public position = new Vector2(100, 100);

  private spr: Phaser.GameObjects.Sprite;
  private readonly level: Level;
  private velocity = new Vector2(0, 0);
  private wallJumpSpeed = new Vector2(65, 280);
  private scale = 1;
  private groundVelocity: number = 0;
  private acceleration = 170;
  private deceleration = 1800;
  private friction = 170;
  private gravity = 787.5;
  private jumpSpeed = 390;
  private lowJumpSpeed = 120;
  private maxSpeed = new Vector2(360, 960);
  private isGrounded = true;
  private isWallSliding = false;

  private debugFlag = true;
  private hasControl = true;
  private horizontalLock = 0;
  private state = PlayerState.IDLE;

  // Collision
  private size: Vector2 = new Vector2(25 * this.scale, 42 * this.scale);
  private platform: Platform | null = null;

  private afterimageData: AfterimageData;

  constructor(level: Level) {
    this.level = level;
  }

  public create(): void {
    this.spr = this.level.add.sprite(this.position.x, this.position.y, 'emerl', 'idle/01.png');
    this.spr.setScale(this.scale);
    this.addAnimation(PlayerAnimation.IDLE, 6, 'idle', 10, -1);
    this.addAnimation(PlayerAnimation.WALK, 8, 'walk', 20, -1);
    this.addAnimation(PlayerAnimation.JUMP, 5, 'jump', 15, 0);
    this.addAnimation(PlayerAnimation.FALL, 4, 'fall', 20, 0);
    this.addAnimation(PlayerAnimation.LAND, 3, 'land', 15, 0);
    this.addAnimation(PlayerAnimation.ATK1, 6, '1stAttack/Sonic', 15, 0);
    this.addAnimation(PlayerAnimation.ATK2, 6, '2ndAttack/Sonic', 15, 0);
    this.addAnimation(PlayerAnimation.WALLSLIDE, 3, 'wallSlide', 15, 0);
    this.playAnimation(PlayerAnimation.IDLE);

    // Setup afterimages
    this.afterimageData = new AfterimageData(this.spr, 4, 5, this.level);
    this.level.children.bringToTop(this.spr);
  }

  public get sprite(): Phaser.GameObjects.Sprite {
    return this.spr;
  }

  public update(__: number, deltaMillis: number, platforms: Platform[]): void {
    const dt = deltaMillis / 1000;
    // Update horizontal lock
    this.horizontalLock = Math.max(0, this.horizontalLock - dt);
    this.updateInputs();
    this.updateState();
    this.updateKinematics(dt);
    this.updateCollisions(platforms);
    this.updateSprite();
    this.afterimageData.update(this.level.isPaused);
    if (this.debugFlag) {
      this.updateDebug();
    }
  }

  private updateInputs(): void {
    const Qkey = this.level.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.Q);

    if (Phaser.Input.Keyboard.JustUp(Qkey)) {
      this.debugFlag = !this.debugFlag;
    }

    if (this.level.gameInput.isInputPressed(GameInput.INPUT2) && this.isGrounded) {
      if (this.state === PlayerState.IDLE) {
        this.playAnimation(PlayerAnimation.ATK1);
        this.state = PlayerState.ATTACK;
        // TODO add proper handling of attacking state
        this.level.addHitbox({
          box: new Phaser.Geom.Rectangle(this.position.x - this.size.x / 2, this.position.y - this.size.y / 2, this.size.x, this.size.y),
          tag: 'player_attack_1',
          force: new Vector2(100 * (this.spr.flipX ? -1 : 1), 0),
        });
      }
    }

    // Check for Pause
    this.level.isPaused = this.level.gameInput.isInputDown(GameInput.INPUT3);
  }

  private updateState(): void {
    if (this.state === PlayerState.ATTACK) {
      if (this.isGrounded) {
        // Player cannot move while attacking on the ground
        this.hasControl = false;
        this.groundVelocity = 0;
      }
      if (this.spr.anims.currentFrame.index >= 4 &&
        this.spr.anims.getCurrentKey() === PlayerAnimation.ATK1 && this.level.gameInput.isInputPressed(GameInput.INPUT2)) {
        this.playAnimation(PlayerAnimation.ATK2);
      }
      if (this.spr.anims.currentFrame.isLast) {
        // After attack finishes, player is set to idle state.
        this.state = PlayerState.IDLE;
        this.level.removeHitbox('player_attack_1');
      }
    } else if (this.state === PlayerState.IDLE) {
      this.hasControl = true;
    }
  }

  private updateKinematics(delta: number): void {
    const velocity = this.isGrounded ? this.groundVelocity : this.velocity.x;
    const acceleration = this.isGrounded ? this.acceleration : this.acceleration * 2;
    const deceleration = this.isGrounded ? this.deceleration : this.acceleration * 2;

    const isRightDown = this.level.gameInput.isInputDown(GameInput.RIGHT);
    const isLeftDown = this.level.gameInput.isInputDown(GameInput.LEFT);
    let speed = Math.abs(velocity);
    const shouldAccelerate = this.hasControl && this.horizontalLock === 0
      && ((velocity > 0 && isRightDown) || (velocity < 0 && isLeftDown));
    const shouldDecelerate = this.hasControl && this.horizontalLock === 0
      && ((velocity > 0 && isLeftDown) || (velocity < 0 && isRightDown));
    let sign = velocity >= 0 ? 1 : -1;
    if (speed > 0) {
      if (shouldAccelerate) {
        // Moving in the direction of motion, so accelerate
        speed += acceleration * delta;
      } else if (shouldDecelerate) {
        const deltaSpeed = deceleration * delta;
        if (deltaSpeed > speed || !this.isGrounded) {
          // The change in speed is higher than current speed, meaning that player should be turned around
          // Also in the air, player can turn around regardless of current speed
          sign *= -1;
          speed = deltaSpeed;
        } else {
          // Moving against the direction of motion, so decelerate
          speed -= deltaSpeed;
        }
      } else if (this.isGrounded) {
        // No input is provided, so apply friction (if on the ground)
        speed -= this.friction * delta;
        speed = Math.max(0, speed);
      }
    } else {
      if (this.hasControl && (isRightDown || isLeftDown)) {
        // Move has been inputted, so accelerate
        speed += this.acceleration * delta;
        sign = isRightDown ? 1 : -1;
      }
    }
    speed = Math.min(this.maxSpeed.x, speed);
    if (this.isGrounded) {
      this.groundVelocity = speed * sign;
      this.velocity.x = this.groundVelocity;
      this.velocity.y = this.hasControl && this.level.gameInput.isInputPressed(GameInput.INPUT1, 3) ? -this.jumpSpeed : 0;
    } else {
      // Else, player is in the air.
      if (this.level.gameInput.isInputReleased(GameInput.INPUT1)) {
        // Player released space bar, so curb jump velocity so that player does not jump as high.
        this.velocity.y = Math.max(this.velocity.y, -this.lowJumpSpeed);
      }
      this.velocity.x = speed * sign;
      this.velocity.y += this.gravity * delta;
      this.velocity.y = Math.min(this.velocity.y, this.maxSpeed.y);

      if (this.isWallSliding) {
        this.velocity.y = Math.min(this.velocity.y, 120);
        if (this.level.gameInput.isInputPressed(GameInput.INPUT1)) {
          const direction = this.spr.flipX ? 1 : -1;
          this.velocity = new Vector2(direction * this.wallJumpSpeed.x, -this.wallJumpSpeed.y);
          this.horizontalLock = .1;
        }
      }
    }

    this.position.x += this.velocity.x * delta;
    this.position.y += this.velocity.y * delta;

    // Make sure player remains in bounds of screen.
    const bounds = this.level.bounds;
    this.position.x = Scalar.clamp(this.position.x, bounds.left + this.size.x / 2, bounds.right - this.size.x / 2);
    this.position.y = Scalar.clamp(this.position.y, bounds.top + this.size.y / 2, bounds.bottom - this.size.y / 2);
  }

  private updateCollisions(platforms: Platform[]): void {
    const solidPlatforms = platforms.filter((platform: Platform) => platform.type === PlatformType.SOLID);
    const nonFluidPlatforms = platforms.filter((platform: Platform) => platform.type !== PlatformType.FLUID);

    let px = this.position.x;
    const py = this.position.y;
    const sx = this.size.x, sy = this.size.y;

    const sensorWallL = new Phaser.Geom.Line(px - sx / 2 - 1, py, px, py);
    const sensorWallR = new Phaser.Geom.Line(px + sx / 2 + 1, py, px, py);
    let gnd = 600;
    const getYFromPoint = (point: Point) => point.y;
    const getXFromPoint = (point: Point) => point.x;

    // Check for wall collisions
    const collisionWallL = this.checkCollisionsWithSensor(sensorWallL, solidPlatforms, px - sx / 2 - 1, getXFromPoint, Math.max);
    const collisionWallR = this.checkCollisionsWithSensor(sensorWallR, solidPlatforms, px + sx / 2 + 1, getXFromPoint, Math.min);
    if (collisionWallL && !collisionWallR) {
      // Player has collided with wall on the left, so push them to right
      this.position.x = collisionWallL.value + sx / 2 + 1;
      this.velocity.x = 0;
      if (this.isGrounded) {
        this.groundVelocity = 0;
      }
      // If holding left while colliding with this wall, then player is wall sliding.
      this.isWallSliding = this.level.gameInput.isInputDown(GameInput.LEFT);
    } else if (collisionWallR && !collisionWallL) {
      // Player has collided with wall on the right, so push them to left
      this.position.x = collisionWallR.value - sx / 2 - 1;
      this.velocity.x = 0;
      if (this.isGrounded) {
        this.groundVelocity = 0;
      }
      // If holding right while colliding with this wall, then player is wall sliding.
      this.isWallSliding = this.level.gameInput.isInputDown(GameInput.RIGHT);
    } else if (!collisionWallR && !collisionWallL) {
      this.isWallSliding = false;
    }
    px = this.position.x;

    // Check for ceiling collisions.
    let ceil = this.position.y - sy / 2;
    const sensorCeilL = new Phaser.Geom.Line(px - sx / 2, py, px - sx / 2, py - sy / 2);
    const sensorCeilR = new Phaser.Geom.Line(px + sx / 2, py, px + sx / 2, py - sy / 2);
    const collisionCeilL = this.checkCollisionsWithSensor(sensorCeilL, solidPlatforms, ceil, getYFromPoint, Math.max);
    const collisionCeilR = this.checkCollisionsWithSensor(sensorCeilR, solidPlatforms, ceil, getYFromPoint, Math.max);
    const hasCeilingCollision = !!collisionCeilL || !!collisionCeilR;
    if (collisionCeilR) {
      ceil = Math.max(ceil, collisionCeilR.value);
    }
    if (collisionCeilL) {
      ceil = Math.max(ceil, collisionCeilL.value);
    }
    if (!this.isGrounded && this.velocity.y <= 0 && this.position.y - sy / 2 < ceil) {
      this.position.y = ceil + sy / 2;
      this.velocity.y = 0;
    } else if (this.isGrounded && hasCeilingCollision) {
      // Stop player from jumping if they are in contact with ceiling while on the ground
      this.velocity.y = 0;
    }

    // Check for ground collisions. In the case there is no collision, use the gnd value.
    const sensorGndL = new Phaser.Geom.Line(px - sx / 2, py, px - sx / 2, py + sy / 2 + 16);
    const sensorGndR = new Phaser.Geom.Line(px + sx / 2, py, px + sx / 2, py + sy / 2 + 16);
    const collisionGndL = this.checkCollisionsWithSensor(sensorGndL, nonFluidPlatforms, gnd, getYFromPoint) ||
      {platform: null, value: gnd};
    const collisionGndR = this.checkCollisionsWithSensor(sensorGndR, nonFluidPlatforms, gnd, getYFromPoint) ||
      {platform: null, value: gnd};
    gnd = Math.min(collisionGndL.value, collisionGndR.value);
    const gndPlatform = collisionGndR.platform || collisionGndL.platform;

    const oldIsGrounded = this.isGrounded;
    if (this.isGrounded && py + sy / 2 + 16 >= gnd && this.velocity.y === 0) {
      // Player is moving along slope, so reposition player as the ground lowers.
      this.position.y = gnd - sy / 2;
      this.setPlatform(gndPlatform);
    } else if (!this.isGrounded && py + sy / 2 >= gnd && this.velocity.y >= 0) {
      // Player is falling through the ground, so reposition player atop it.
      this.position.y = gnd - sy / 2;
      this.isGrounded = true;
      this.groundVelocity = this.velocity.x;
      this.setPlatform(gndPlatform);
    } else {
      // Player is airborne
      this.isGrounded = false;
      this.setPlatform(null);
    }

    if (oldIsGrounded && !this.isGrounded && this.velocity.y >= 0) {
      this.horizontalLock = .1;
    }
  }

  private updateSprite(): void {
    const currentAnimation = <PlayerAnimation> this.spr.anims.getCurrentKey();
    this.spr.x = this.position.x;
    this.spr.y = this.position.y;
    this.spr.anims.setTimeScale(1);
    switch (this.state) {
      case PlayerState.IDLE:
        if (this.isGrounded) {
          const speed = Math.abs(this.velocity.x);
          if (speed > 0) {
            this.playAnimation(PlayerAnimation.WALK);
            this.spr.anims.setTimeScale(Math.max(.4, speed / this.maxSpeed.x));
          } else if (currentAnimation === PlayerAnimation.FALL && this.velocity.x === 0) {
            this.playAnimation(PlayerAnimation.LAND);
          } else if (currentAnimation !== PlayerAnimation.LAND && this.velocity.x === 0) {
            this.playAnimation(PlayerAnimation.IDLE);
          }
        } else {
          if (this.velocity.y < 0) {
            this.playAnimation(PlayerAnimation.JUMP);
          } else {
            this.playAnimation(PlayerAnimation.FALL);
          }
        }
        if (currentAnimation === PlayerAnimation.LAND && !this.spr.anims.isPlaying) {
          this.playAnimation(PlayerAnimation.IDLE);
        }
        break;
    }
    if (this.velocity.x > 0) {
      this.spr.flipX = false;
    } else if (this.velocity.x < 0) {
      this.spr.flipX = true;
    }
    if (this.spr.flipX) {
      // When the sprite is flipped, the pivots also need to be flipped.
      // Based on phaser/src/gameobjects/components/Animation.js:setCurrentFrame,
      // pivots are analogous to the sprite origin, so updating the sprite origin directly should have the desired effect.
      const frame = this.spr.anims.currentFrame.frame;
      if (frame.customPivot) {
        this.spr.setOrigin(1 - frame.pivotX, frame.pivotY);
      }
    }
  }

  private updateDebug(): void {
    const {displayWidth, displayHeight, x, y, originX, originY} = this.spr;
    const px = this.position.x;
    const py = this.position.y;
    this.level.debug.drawRect(x - displayWidth * originX, y - displayHeight * originY, displayWidth, displayHeight);

    // Wall sensors
    this.level.debug.drawLine(px - this.size.x / 2 - 1, py, px, py, 0xff0000);
    this.level.debug.drawLine(px + this.size.x / 2 + 1, py, px, py, 0xff00ff);
    // Ground sensors
    this.level.debug.drawLine(px - this.size.x / 2, py, px - this.size.x / 2, py + this.size.y / 2, 0x0000ff);
    this.level.debug.drawLine(px + this.size.x / 2, py, px + this.size.x / 2, py + this.size.y / 2, 0x00ffff);
    // Ceiling sensors
    this.level.debug.drawLine(px - this.size.x / 2, py, px - this.size.x / 2, py - this.size.y / 2, 0xaaff00);
    this.level.debug.drawLine(px + this.size.x / 2, py, px + this.size.x / 2, py - this.size.y / 2, 0xffff00);
  }

  /**
   * Takes a sensor and checks for most relevant collision given a list of platforms.
   * Relevance is determined by a combination of the getValueFn and comparisionFn.
   * If no collision is found, then null is returned.
   * @param {Phaser.Geom.Line} sensor
   * @param {Platform[]} platforms
   * @param {number} defaultValue
   * @param {(point: Point => number} getValueFn: Function that takes in a point and extracts a number.
   *  It is used in conjunction with the comparisonFn.
   * @param {any} comparisonFn: Determines how to compare values. Can be either Math.min or Math.max.
   * @returns {number | null}
   */
  private checkCollisionsWithSensor(
    sensor: Phaser.Geom.Line,
    platforms: Platform[],
    defaultValue: number,
    getValueFn: (point: Point) => number,
    comparisonFn: typeof Math.min | typeof Math.max = Math.min,
  ): CollisionData | null {
    let output: CollisionData | null = null;

    platforms.forEach((platform: Platform) => {

      const intersections: Point[] = Phaser.Geom.Intersects.GetLineToRectangle(sensor, platform.collider);
      if (intersections.length > 0) {
        if (output === null) {
          output = {platform: null, value: defaultValue};
        }
        const reducedValue = intersections
          .map(getValueFn)
          .reduce((accumulator: number, value: number) => comparisonFn(accumulator, value), defaultValue);

        const outputValue = comparisonFn(output.value, reducedValue);
        if (outputValue === reducedValue && output.value !== reducedValue) {
          // Output value was updated, so reference this platform
          output = {platform, value: outputValue};
        }
      }
    });
    return output;
  }

  private setPlatform(platform: Platform | null): void {
    if (this.platform) {
      this.platform.clearPlayer();
    }

    this.platform = platform;
    if (this.platform) {
      this.platform.player = this;
    }
  }

  private addAnimation(key: PlayerAnimation, count: number, prefix: string, frameRate: number, repeat: number): void {
    const frames = this.spr.anims.animationManager.generateFrameNames('emerl', {
      start: 1,
      end: count,
      zeroPad: 2,
      prefix: `${prefix}/`,
      suffix: '.png',
    });
    this.spr.anims.animationManager.create({key, frames, frameRate, repeat});
  }

  private playAnimation(key: PlayerAnimation): void {
    if (this.spr.anims.getCurrentKey() !== key) {
      this.spr.anims.play(key);
    }
  }
}
