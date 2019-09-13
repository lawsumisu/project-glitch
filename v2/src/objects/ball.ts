import { Point, Vector2 } from 'src/utilities/vector/vector';
import { Level } from 'src/levels';
import { Platform } from 'src/platform';
import * as Phaser from 'phaser';
import { Scalar } from 'src/utilities/math/scalar.util';
import { Hitbox, LevelObject } from 'src/objects/index';

export class Ball implements LevelObject {
  public position: Vector2;
  private velocity = new Vector2(0, 0);
  private friction = 170;
  private gravity = 787.5;
  private isGrounded = true;
  private readonly level: Level;
  private radius = 15;
  private activeHitboxes: Set<string>;

  constructor(level: Level) {
    this.level = level;
  }

  public create(): void {
    this.position = new Vector2(250, 100);
    this.activeHitboxes = new Set();
  }

  public force(f: Vector2): void {
    this.velocity = this.velocity.add(f);
  }

  public update(__: number, deltaMillis: number, platforms: Platform[], hitboxes: Hitbox[]): void {
    const dt = deltaMillis / 1000;
    if (!this.level.isPaused) {
      this.updateKinematics(dt, hitboxes);
      this.updateCollisions(platforms);
    }
    this.updateDebug();
  }

  private updateKinematics(delta: number, hitboxes: Hitbox[]): void {
    const newHits = new Set<string>();
    hitboxes.forEach((hitbox: Hitbox) => {
      if (Phaser.Geom.Intersects.CircleToRectangle(new Phaser.Geom.Circle(this.position.x, this.position.y, this.radius), hitbox.box)) {
        newHits.add(hitbox.tag);
        if (!this.activeHitboxes.has(hitbox.tag)) {
          this.force(hitbox.force);
        }
      }
    });
    this.activeHitboxes = newHits;
    // if (this.level.gameInput.isInputDown(GameInput.RIGHT)) {
    //   this.force(new Vector2(10, 0));
    // }
    // if (this.level.gameInput.isInputDown(GameInput.LEFT)) {
    //   this.force(new Vector2(-10, 0));
    // }
    // if (this.level.gameInput.isInputDown(GameInput.UP)) {
    //   this.force(new Vector2(0, -20));
    // }
    if (this.isGrounded) {
      let speed = Math.abs(this.velocity.x);
      const sign = this.velocity.x >= 0 ? 1 : -1;
      speed -= this.friction * delta;
      speed = Math.max(0, speed);
      this.velocity.x = speed * sign;
    } else {
      this.velocity.y += this.gravity * delta;
    }
    // Update position
    this.position = this.position.add(this.velocity.scale(delta));

    // Make sure player remains in bounds of screen.
    const bounds = this.level.bounds;
    this.position.x = Scalar.clamp(this.position.x, bounds.left + this.radius, bounds.right - this.radius);
    this.position.y = Scalar.clamp(this.position.y, bounds.top + this.radius, bounds.bottom - this.radius);
  }

  private updateCollisions(platforms: Platform[]): void {
    const getYFromPoint = (point: Point) => point.y;

    const r = this.radius;
    const px = this.position.x;
    const py = this.position.y;
    const sensorGnd = new Phaser.Geom.Line(px, py, px, py + r);
    const gnd = 600;

    const collisionGnd = Platform.checkCollisionsWithSensor(sensorGnd, platforms, gnd, getYFromPoint) || {platform: null, value: gnd};

    if (py + r >= collisionGnd.value && this.velocity.y > 0) {
      this.position.y = collisionGnd.value - r;
      this.isGrounded = true;
      if (this.velocity.y > 100) {
        // Cause the ball to bounce when it hits the ground
        this.velocity.y = -this.velocity.y / 2;
      } else {
        this.velocity.y = 0;
      }
    } else {
      this.isGrounded = false;
    }
  }

  private updateDebug(): void {
    this.level.debug.drawCircle(this.position.x, this.position.y, this.radius, 0xff0000);
  }
}
