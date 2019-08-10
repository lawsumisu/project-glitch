import * as Phaser from 'phaser';
import { DebugDrawPlugin } from "src/plugins/debug";

export enum PlayerAnimation {
  IDLE = 'IDLE',
  WALK = 'WALK'
}

export class Player {
  private spr: Phaser.GameObjects.Sprite;
  private scene: Phaser.Scene;
  private groundVelocity: number = 0;
  private acceleration = 170;
  private deceleration = 1800;
  private friction = 170;

  constructor(scene: Phaser.Scene) {
    this.scene = scene;
  }

  public create(): void {
    this.spr = this.scene.add.sprite(100, 100, 'emerl', 'idle/01.png');
    this.spr.setScale(2);
    const frameNames = this.spr.anims.animationManager.generateFrameNames('emerl', {
      start: 1,
      end: 6,
      zeroPad: 2,
      prefix: 'idle/',
      suffix: '.png'
    });
    this.spr.anims.animationManager.create({ key: PlayerAnimation.IDLE, frames: frameNames, frameRate: 10, repeat: -1 });

    const walkFrames = this.spr.anims.animationManager.generateFrameNames('emerl', {
      start: 1,
      end: 8,
      zeroPad: 2,
      prefix: 'walk/',
      suffix: '.png'
    });
    this.spr.anims.animationManager.create({ key: PlayerAnimation.WALK, frames: walkFrames, frameRate: 10, repeat: -1 });
    this.spr.anims.play(PlayerAnimation.IDLE);
  }

  public update(__: number, deltaMillis: number): void {
    const dt = deltaMillis / 1000;
    this.updateVelocity(dt);
    this.spr.x += this.groundVelocity * dt;
    this.updateSprite();
    this.updateDebug();
  }

  private updateVelocity(delta: number): void {
    const rightKey = this.scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.RIGHT);
    const leftKey = this.scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.LEFT);
    let speed = Math.abs(this.groundVelocity);
    const shouldAccelerate = (this.groundVelocity > 0 && rightKey.isDown) || (this.groundVelocity < 0 && leftKey.isDown);
    const shouldDecelerate = (this.groundVelocity < 0 && rightKey.isDown) || (this.groundVelocity > 0 && leftKey.isDown);
    let sign = this.groundVelocity >= 0 ? 1 : -1;
    if (speed > 0) {
      if (shouldAccelerate) {
        // Moving in the direction of motion, so accelerate
        speed += this.acceleration * delta;
      } else if (shouldDecelerate) {
        const deltaSpeed = this.deceleration * delta;
        if (deltaSpeed > speed) {
          // The change in speed is higher than current speed, meaning that player should be turned around
          sign *= -1;
          speed = deltaSpeed;
        } else {
          // Moving against the direction of motion, so decelerate
          speed -= deltaSpeed;
        }
      } else {
        // No input is provided, so apply friction
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
    this.groundVelocity = speed * sign;
  }

  private updateSprite(): void {
    if (this.groundVelocity > 0) {
      this.spr.flipX = false;
    } else if (this.groundVelocity < 0) {
      this.spr.flipX = true;
    }
    if (Math.abs(this.groundVelocity) > 0 && this.spr.anims.getCurrentKey() !== PlayerAnimation.WALK) {
      this.spr.anims.play(PlayerAnimation.WALK);
    } else if (this.groundVelocity === 0 && this.spr.anims.getCurrentKey() !== PlayerAnimation.IDLE) {
      this.spr.anims.play(PlayerAnimation.IDLE);
    }
  }

  private updateDebug(): void {
    const { displayWidth, displayHeight, x, y } = this.spr;
    this.debugPlugin.drawLine(x - displayWidth / 2, y, x + displayWidth / 2, y);
    this.debugPlugin.drawRect(x - displayWidth / 2, y - displayHeight / 2, displayWidth, displayHeight);
  }

  private get debugPlugin(): DebugDrawPlugin {
    return (<any> this.scene.sys).debug;
  }
}