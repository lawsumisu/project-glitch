import { Platform } from 'src/platform';
import { Vector2 } from 'src/utilities/vector/vector';
import * as Phaser from 'phaser';

export interface Hitbox {
  box: Phaser.Geom.Rectangle;
  force: Vector2;
  // Unique string that identifies this hitbox.
  tag: string;
}

export interface LevelObject {
  create: () => void;
  update: (time: number, deltaMillis: number, platforms: Platform[], hitboxes: Hitbox[]) => void;
}
