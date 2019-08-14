import * as Phaser from 'phaser';
import { DebugDrawPlugin } from "src/plugins/debug";

export enum PlayerAnimation {
  IDLE = 'IDLE',
  WALK = 'WALK',
  JUMP = 'JUMP',
  FALL = 'FALL',
}

export class Player {
  private spr: Phaser.GameObjects.Sprite;
  private scene: Phaser.Scene;
  private position: {x: number; y: number } = { x: 100, y: 100 };
  private velocity: {x: number, y: number} = { x: 0, y: 0 };
  private groundVelocity: number = 0;
  private acceleration = 170;
  private deceleration = 1800;
  private friction = 170;
  private gravity = 787.5;
  private jumpSpeed = 390;
  private isGrounded = true;

  constructor(scene: Phaser.Scene) {
    this.scene = scene;
  }

  public create(): void {
    this.spr = this.scene.add.sprite(this.position.x, this.position.y, 'emerl', 'idle/01.png');
    this.spr.setScale(2);
    this.addAnimation(PlayerAnimation.IDLE, 6, 'idle', 10, -1);
    this.addAnimation(PlayerAnimation.WALK, 8, 'walk', 10, -1);
    this.addAnimation(PlayerAnimation.JUMP, 5, 'jump', 15, 0);
    this.addAnimation(PlayerAnimation.FALL, 4, 'fall', 20, 0);
    this.playAnimation(PlayerAnimation.IDLE);
  }

  public update(__: number, deltaMillis: number): void {
    const dt = deltaMillis / 1000;
    this.updateVelocity(dt);
    this.updateCollisions();
    this.updateSprite();
    this.updateDebug();
  }

  private updateVelocity(delta: number): void {
    const rightKey = this.scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.RIGHT);
    const leftKey = this.scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.LEFT);
    const spaceKey = this.scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.SPACE);
    const velocity = this.isGrounded ? this.groundVelocity : this.velocity.x;
    const acceleration = this.isGrounded ? this.acceleration : this.acceleration * 2;
    const deceleration = this.isGrounded ? this.deceleration : this.acceleration * 2;

    let speed = Math.abs(velocity);
    const shouldAccelerate = (velocity > 0 && rightKey.isDown) || (velocity < 0 && leftKey.isDown);
    const shouldDecelerate = (velocity && rightKey.isDown) || (velocity && leftKey.isDown);
    let sign = velocity >= 0 ? 1 : -1;
    if (speed > 0) {
      if (shouldAccelerate) {
        // Moving in the direction of motion, so accelerate
        speed += acceleration * delta;
      } else if (shouldDecelerate) {
        const deltaSpeed = deceleration * delta;
        if (deltaSpeed > speed || !this.isGrounded) {
          // The change in speed is higher than current speed, meaning that player should be turned around
          // In the air, player can turn around immediately
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
      if (rightKey.isDown || leftKey.isDown) {
        // Move has been inputted, so accelerate
        speed += this.acceleration * delta;
        sign = rightKey.isDown ? 1 : -1;
      }
    }
    if (this.isGrounded) {
      this.groundVelocity = speed * sign;
      this.velocity.x = this.groundVelocity;
      this.velocity.y = spaceKey.isDown ? -this.jumpSpeed : 0;
    } else {
      // Else, player is in the air.
      this.velocity.x = speed * sign;
      this.velocity.y += this.gravity * delta;
    }

    this.position.x += this.velocity.x * delta;
    this.position.y += this.velocity.y * delta;
    // console.log(this.position, this.velocity, this.isGrounded);

  }

  private updateCollisions(): void {
    if (this.position.y < 400) {
      this.isGrounded = false;
    } else {
      this.isGrounded = true;
      this.groundVelocity = this.velocity.x;
    }
    this.position.y = Math.min(400, this.position.y);
  }

  private updateSprite(): void {
    this.spr.x = this.position.x;
    this.spr.y = this.position.y;
    if (this.velocity.x > 0) {
      this.spr.flipX = false;
    } else if (this.velocity.x < 0) {
      this.spr.flipX = true;
    }
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
  }

  private updateDebug(): void {
    const { displayWidth, displayHeight, x, y } = this.spr;
    this.debugPlugin.drawLine(x - displayWidth / 2, y, x + displayWidth / 2, y);
    this.debugPlugin.drawRect(x - displayWidth / 2, y - displayHeight / 2, displayWidth, displayHeight);
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

  private get debugPlugin(): DebugDrawPlugin {
    return (<any> this.scene.sys).debug;
  }

}