import * as Phaser from 'phaser';
import { DebugDrawPlugin } from 'src/plugins/debug.plugin';
import { Vector2 } from 'src/utilities/vector/vector';

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
  private direction: number = 1;

  constructor(config: PlatformConfig) {
    const { width, height, speed, trackPoints } = config;
    this.position = trackPoints[0];
    this.width = width;
    this.height = height;
    this.speed = speed || 0;
    this.track = trackPoints;
  }

  public update(delta: number): void {
    this.updateKinematics(delta);
  }

  public debug(debugPlugin: DebugDrawPlugin) {
    debugPlugin.drawRect(this.position.x - this.width / 2, this.position.y - this.height / 2, this.width, this.height, 0xff000f);
    this.track.forEach((v: Vector2) => {
      debugPlugin.drawCircle(v.x, v.y, 5, 0xff00ff);
    });
  }

  public get collider(): Phaser.Geom.Rectangle {
    return new Phaser.Geom.Rectangle(this.position.x - this.width / 2, this.position.y - this.height / 2, this.width, this.height);
  }

  private updateKinematics(delta: number) {
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
        }
      }
    }
  }
}
