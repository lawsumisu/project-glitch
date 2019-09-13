import * as Phaser from 'phaser';
import { DebugDrawPlugin } from 'src/plugins/debug.plugin';
import {Point, Vector2} from 'src/utilities/vector/vector';
import { Player } from 'src/player';
import {Level} from 'src/levels';

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
  level: Level;
  type?: PlatformType;
}

export interface CollisionData {
  platform: Platform | null;
  value: number;
}

export class Platform {
  public static checkCollisionsWithSensor(
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
          output = { platform: null, value: defaultValue };
        }
        const reducedValue = intersections
          .map(getValueFn)
          .reduce((accumulator: number, value: number) => comparisonFn(accumulator, value), defaultValue);

        const outputValue = comparisonFn(output.value, reducedValue);
        if (outputValue === reducedValue && output.value !== reducedValue) {
          // Output value was updated, so reference this platform
          output = { platform, value: outputValue };
        }
      }
    });
    return output;
  }

  public player: Player | null = null;

  private speed: number;
  private position: Vector2;
  private width: number;
  private height: number;
  private track: Vector2[];
  private trackIndex: number = 0;
  private direction: number = 1;
  private level: Level;
  private reverseTimer: number;
  private currentTimer = 0;
  private _type: PlatformType;

  constructor(config: PlatformConfig) {
    const { width, height, speed, trackPoints, level, reverseTimer, type } = config;
    this.position = trackPoints[0];
    this.width = width;
    this.height = height;
    this.speed = speed || 0;
    this.track = trackPoints;
    this.level = level;
    this.reverseTimer = reverseTimer || 0;
    this._type = type || PlatformType.SOLID;
  }

  public update(delta: number): void {
    if (this.level.isPaused) {
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
    if (this._type === PlatformType.FLUID && this.level.isPaused) {
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
