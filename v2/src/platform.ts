import * as Phaser from 'phaser';
import { DebugDrawPlugin } from 'src/plugins/debug.plugin';
import { Vector2 } from 'src/utilities/vector/vector';
import { Scene } from 'src/utilities/phaser.util';
import { Player } from 'src/player';

export enum PlatformType {
  SOLID = 'SOLID',
  THIN = 'THIN',
  FLUID = 'FLUID',
}
export interface PlatformConfig {
  width: number;
  height: number;
  speed?: number;
  reverseTimer?: number;
  trackPoints: Vector2[];
  scene: Scene;
  type?: PlatformType;
}
export class Platform {
  public player: Player | null = null;

  private speed: number;
  private position: Vector2;
  private width: number;
  private height: number;
  private track: Vector2[];
  private trackIndex: number = 0;
  private direction: number = 1;
  private scene: Scene;
  private reverseTimer: number;
  private currentTimer = 0;
  private _type: PlatformType;

  constructor(config: PlatformConfig) {
    const { width, height, speed, trackPoints, scene, reverseTimer, type } = config;
    this.position = trackPoints[0];
    this.width = width;
    this.height = height;
    this.speed = speed || 0;
    this.track = trackPoints;
    this.scene = scene;
    this.reverseTimer = reverseTimer || 0;
    this._type = type || PlatformType.SOLID;
  }

  public update(delta: number): void {
    if (this.scene.isPaused) {
      return;
    }
    this.updateTimer(delta);
    this.updateKinematics(delta);
  }

  public debug(debugPlugin: DebugDrawPlugin) {
    let color;
    switch (this._type) {
      case PlatformType.FLUID:
        color = 0x0000ff;
        break;
      case PlatformType.THIN:
        color = 0x00ff00;
        break;
      case PlatformType.SOLID:
        color = 0xff0000;
        break;
    }
    color = this.player ? 0x00ffff : color;
    debugPlugin.drawRect(this.position.x - this.width / 2, this.position.y - this.height / 2, this.width, this.height, color);
    this.track.forEach((v: Vector2) => {
      debugPlugin.drawCircle(v.x, v.y, 5, 0xff00ff);
    });
  }

  public clearPlayer(): void {
    this.player = null;
  }

  public get collider(): Phaser.Geom.Rectangle {
    return new Phaser.Geom.Rectangle(this.position.x - this.width / 2, this.position.y - this.height / 2, this.width, this.height);
  }

  public get type(): PlatformType {
    if (this._type === PlatformType.FLUID && this.scene.isPaused) {
      return PlatformType.THIN;
    } else {
      return this._type;
    }
  }

  private updateTimer(delta: number): void {
    this.currentTimer = Math.max(this.currentTimer - delta, 0);
  }

  private updateKinematics(delta: number) {
    if (this.currentTimer > 0) {
      return;
    }
    const oldPosition = this.position;
    if (this.track.length >= 2 ) {
      const from = this.track[this.trackIndex];
      const to = this.track[this.trackIndex + this.direction];
      const direction = to.subtract(from).normalize();
      const velocity = direction.scale(this.speed);
      this.position = this.position.add(velocity.scale(delta));

      // Check if platform has passed the next location
      const platformDistance = this.position.subtract(from).magnitude();
      const trackDistance = to.subtract(from).magnitude();
      if (platformDistance >= trackDistance) {
        // Set platform to track position so that platform doesn't overshoot.
        this.position = to;

        // Update track position
        this.trackIndex += this.direction;
        if (this.trackIndex === this.track.length - 1 || this.trackIndex === 0) {
          // Reached the end or beginning of the track, so change direction
          this.direction *= -1;
          this.currentTimer = this.reverseTimer;
        }
      }
    }

    if (this.player) {
      // Update player position based on updated position of this platform
      const diff = this.position.subtract(oldPosition);
      this.player.position = this.player.position.add(diff);
    }
  }
}
