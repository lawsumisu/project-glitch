import * as Phaser from 'phaser';
import { Vector2 } from "src/vector";
import { DebugDrawPlugin } from "src/plugins/debug";

export interface PlatformConfig {
  width: number;
  height: number;
  speed?: number;
  trackPoints: Vector2[];
}
export class Platform {
  private speed: number;
  private position: Vector2;
  private width: number;
  private height: number;
  private track: Vector2[];
  private trackIndex: number = 0;

  constructor(config: PlatformConfig) {
    const { width, height, speed, trackPoints } = config;
    this.position = trackPoints[0];
    this.width = width;
    this.height = height;
    this.speed = speed || 0;
    this.track = trackPoints;
  }

  public update(delta: number): void {
    if (this.trackIndex < this.track.length -1 ) {
      const from = this.track[this.trackIndex];
      const to = this.track[this.trackIndex + 1];
      const direction = to.subtract(from);
      const velocity = direction.scale(this.speed);
      this.position = this.position.add(velocity.scale(delta));
    }
  }

  public draw(debugPlugin: DebugDrawPlugin) {
    debugPlugin.drawRect(this.position.x - this.width / 2, this.position.y - this.height / 2, this.width, this.height, 0xff000f);
  }
}