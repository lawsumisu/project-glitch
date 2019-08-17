import * as Phaser from 'phaser';
import { Point, Vector2 } from 'src/utilities/vector/vector';
import { GameInput } from 'src/plugins/gameInput.plugin';
import { Scene } from 'src/utilities/phaser.util';

export enum PlayerAnimation {
  IDLE = 'IDLE',
  WALK = 'WALK',
  JUMP = 'JUMP',
  FALL = 'FALL',
  ATK1 = 'ATK1',
  ATK2 = 'ATK2',
}

enum PlayerState {
  IDLE = 'IDLE',
  ATTACK = 'ATTACK',
}

export class Player {
  private spr: Phaser.GameObjects.Sprite;
  private scene: Scene;
  private position = new Vector2(100,  100 );
  private velocity = new Vector2(0,  0 );
  private scale = 1;
  private groundVelocity: number = 0;
  private acceleration = 170;
  private deceleration = 1800;
  private friction = 170;
  private gravity = 787.5;
  private jumpSpeed = 390;
  private lowJumpSpeed = 240;
  private isGrounded = true;

  private debugFlag = true;
  private hasControl = true;
  private state = PlayerState.IDLE;

  // Collision
  private size: Vector2 = new Vector2(25 * this.scale, 42 * this.scale);

  constructor(scene: Scene) {
    this.scene = scene;
  }

  public create(): void {
    this.spr = this.scene.add.sprite(this.position.x, this.position.y, 'emerl', 'idle/01.png');
    this.spr.setScale(this.scale);
    this.addAnimation(PlayerAnimation.IDLE, 6, 'idle', 10, -1);
    this.addAnimation(PlayerAnimation.WALK, 8, 'walk', 10, -1);
    this.addAnimation(PlayerAnimation.JUMP, 5, 'jump', 15, 0);
    this.addAnimation(PlayerAnimation.FALL, 4, 'fall', 20, 0);
    this.addAnimation(PlayerAnimation.ATK1, 6, '1stAttack/Sonic', 15, 0);
    this.addAnimation(PlayerAnimation.ATK2, 6, '2ndAttack/Sonic', 15, 0);
    this.playAnimation(PlayerAnimation.IDLE);

  }

  public update(__: number, deltaMillis: number, colliders: Phaser.Geom.Rectangle[]): void {
    const dt = deltaMillis / 1000;
    this.updateInputs();
    this.updateState();
    this.updateKinematics(dt);
    this.updateCollisions(colliders);
    this.updateSprite();
    if (this.debugFlag) {
      this.updateDebug();
    }
  }

  private updateInputs(): void {
    const Qkey = this.scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.Q);

    if (Phaser.Input.Keyboard.JustUp(Qkey)) {
      this.debugFlag = !this.debugFlag;
    }

    if (this.scene.gameInput.isInputPressed(GameInput.INPUT2) && this.isGrounded) {
      if (this.state === PlayerState.IDLE) {
        this.playAnimation(PlayerAnimation.ATK1);
        this.state = PlayerState.ATTACK;
      }
    }
  }

  private updateState(): void {
    if (this.state === PlayerState.ATTACK) {
      if (this.isGrounded) {
        // Player cannot move while attacking on the ground
        this.hasControl = false;
        this.groundVelocity = 0;
      }
      if (this.spr.anims.currentFrame.index >= 4 &&
        this.spr.anims.getCurrentKey() === PlayerAnimation.ATK1 && this.scene.gameInput.isInputPressed(GameInput.INPUT2)) {
        this.playAnimation(PlayerAnimation.ATK2);
      }
      if (this.spr.anims.currentFrame.isLast) {
        // After attack finishes, player is set to idle state.
        this.state = PlayerState.IDLE;
      }
    } else if (this.state === PlayerState.IDLE) {
      this.hasControl = true;
    }
  }

  private updateKinematics(delta: number): void {
    const velocity = this.isGrounded ? this.groundVelocity : this.velocity.x;
    const acceleration = this.isGrounded ? this.acceleration : this.acceleration * 2;
    const deceleration = this.isGrounded ? this.deceleration : this.acceleration * 2;

    const isRightDown = this.scene.gameInput.isInputDown(GameInput.RIGHT);
    const isLeftDown = this.scene.gameInput.isInputDown(GameInput.LEFT);
    let speed = Math.abs(velocity);
    const shouldAccelerate = this.hasControl && ((velocity > 0 && isRightDown) || (velocity < 0 && isLeftDown));
    const shouldDecelerate = this.hasControl && ((velocity > 0 && isLeftDown) || (velocity < 0 && isRightDown));
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
    if (this.isGrounded) {
      this.groundVelocity = speed * sign;
      this.velocity.x = this.groundVelocity;
      this.velocity.y = this.scene.gameInput.isInputPressed(GameInput.INPUT1, 3) ? -this.jumpSpeed : 0;
    } else {
      // Else, player is in the air.
      if (this.scene.gameInput.isInputReleased(GameInput.INPUT1)) {
        // Player released space bar, so curb jump velocity so that player does not jump as high.
        this.velocity.y = Math.max(this.velocity.y, -this.lowJumpSpeed);
      }
      this.velocity.x = speed * sign;
      this.velocity.y += this.gravity * delta;
    }

    this.position.x += this.velocity.x * delta;
    this.position.y += this.velocity.y * delta;
    // console.log(this.position, this.velocity, this.isGrounded);

  }

  private updateCollisions(colliders: Phaser.Geom.Rectangle[]): void {
    const px = this.position.x, py = this.position.y;
    const sx = this.size.x, sy = this.size.y;

    const sensorWallL = new Phaser.Geom.Line(px - sx / 2 - 1, py, px, py);
    const sensorWallR = new Phaser.Geom.Line(px + sx / 2 + 1, py, px, py);
    const sensorGndL = new Phaser.Geom.Line(px - sx / 2, py, px - sx / 2, py + sy / 2);
    const sensorGndR = new Phaser.Geom.Line(px + sx / 2 , py, px + sx / 2, py + sy / 2);
    let gnd = 600;
    const getYFromPoint = (point: Point) => point.y;
    const getXFromPoint = (point: Point) => point.x;

    // Check for wall collisions
    const collisionWallL = this.checkCollisionsWithSensor(sensorWallL, colliders, px - sx / 2 - 1, getXFromPoint, Math.max);
    const collisionWallR = this.checkCollisionsWithSensor(sensorWallR, colliders, px + sx / 2 + 1, getXFromPoint, Math.min);
    if (collisionWallL && !collisionWallR) {
      // Player has collided with wall on the left, so push them to right
      this.position.x = collisionWallL + sx / 2 + 1;
      this.velocity.x = 0;
    } else if (collisionWallR && !collisionWallL) {
      // Player has collided with wall on the right, so push them to left
      this.position.x = collisionWallR - sx / 2 - 1;
      this.velocity.x = 0;
    }

    // Check for ground collisions. In the case there is no collision, use the gnd value.
    const collisionGndL = this.checkCollisionsWithSensor(sensorGndL, colliders, gnd, getYFromPoint) || gnd;
    const collisionGndR = this.checkCollisionsWithSensor(sensorGndR, colliders, gnd, getYFromPoint) || gnd;
    gnd = Math.min(collisionGndL, collisionGndR);

    if (py + sy / 2 >= gnd && this.velocity.y >= 0) {
      // Player is falling through the ground, so reposition player atop it.
      this.position.y = gnd - sy / 2;
      this.isGrounded = true;
      this.groundVelocity = this.velocity.x;
    } else {
      this.isGrounded = false;
    }
  }

  private updateSprite(): void {
    this.spr.x = this.position.x;
    this.spr.y = this.position.y;
    if (this.velocity.x > 0) {
      this.spr.flipX = false;
    } else if (this.velocity.x < 0) {
      this.spr.flipX = true;
    }
    switch (this.state) {
      case PlayerState.IDLE:
        if (this.isGrounded) {
          if (Math.abs(this.velocity.x) > 0) {
            this.playAnimation(PlayerAnimation.WALK);
          } else if (this.velocity.x === 0) {
            this.playAnimation(PlayerAnimation.IDLE);
          }
        } else {
          if (this.velocity.y < 0) {
            this.playAnimation(PlayerAnimation.JUMP);
          } else {
            this.playAnimation(PlayerAnimation.FALL);
          }
        }
        break;
    }
  }

  private updateDebug(): void {
    const { displayWidth, displayHeight, x, y } = this.spr;
    const px = this.position.x;
    const py = this.position.y;
    this.scene.debug.drawRect(x - displayWidth / 2, y - displayHeight / 2, displayWidth, displayHeight);

    this.scene.debug.drawLine(px - this.size.x / 2 - 1, py, px, py, 0xff0000);
    this.scene.debug.drawLine(px + this.size.x / 2 + 1, py, px, py, 0xff00ff);
    this.scene.debug.drawLine(px - this.size.x / 2, py, px - this.size.x / 2, py + this.size.y / 2, 0x0000ff);
    this.scene.debug.drawLine(px + this.size.x / 2, py, px + this.size.x / 2, py + this.size.y / 2, 0x00ffff);
  }

  /**
   * Takes a sensor and checks for most relevant collision given a list of colliders.
   * Relevance is determined by a combination of the getValueFn and comparisionFn.
   * If no collision is found, then null is retuned.
   * @param {Phaser.Geom.Line} sensor
   * @param {Phaser.Geom.Rectangle[]} colliders
   * @param {number} defaultValue
   * @param {(point: Point => number} getValueFn: Function that takes in a point and extracts a number.
   *  It is used in conjunction with the comparisonFn.
   * @param {any} comparisonFn: Determines how to compare values. Can be either Math.min or Math.max.
   * @returns {number | null}
   */
  private checkCollisionsWithSensor(
    sensor: Phaser.Geom.Line,
    colliders: Phaser.Geom.Rectangle[],
    defaultValue: number,
    getValueFn: (point: Point) => number,
    comparisonFn: typeof Math.min | typeof Math.max = Math.min,
  ): number | null {
    let output: number | null = null;
    colliders.forEach((collider: Phaser.Geom.Rectangle) => {
      const intersections: Point[] = Phaser.Geom.Intersects.GetLineToRectangle(sensor, collider);
      if (intersections.length > 0) {
        if (output === null) {
          output = defaultValue;
        }
        const reducedValue = intersections
          .map(getValueFn)
          .reduce((accumulator: number, value: number) => comparisonFn(accumulator, value), defaultValue);
        output = comparisonFn(reducedValue, output);
      }
    });
    return output;
  }

  private addAnimation(key: PlayerAnimation, count: number, prefix: string, frameRate: number, repeat: number): void {
    const frames = this.spr.anims.animationManager.generateFrameNames('emerl', {
      start: 1,
      end: count,
      zeroPad: 2,
      prefix: `${prefix}/`,
      suffix: '.png',
    });
    this.spr.anims.animationManager.create({ key, frames, frameRate, repeat });
  }

  private playAnimation(key: PlayerAnimation): void {
    if (this.spr.anims.getCurrentKey() !== key) {
      this.spr.anims.play(key);
    }
  }
}
