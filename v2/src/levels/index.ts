import * as Phaser from 'phaser';
import { DebugDrawPlugin } from 'src/plugins/debug.plugin';
import { GameInputPlugin } from 'src/plugins/gameInput.plugin';
import { Player } from 'src/player';
import { Platform } from 'src/platform';
import { Scalar } from 'src/utilities/math/scalar.util';
import { Hitbox, LevelObject } from 'src/objects';
import * as _ from 'lodash';
import { Vector2 } from 'src/utilities/vector/vector';

export abstract class Level extends Phaser.Scene {
  public isPaused = false;
  protected player: Player;
  protected platforms: Platform[];
  protected hitboxes: {[key: string]: Hitbox };
  protected objects: LevelObject[];
  protected cameraSpeed = 10;
  protected _collisionLayer: Phaser.Tilemaps.StaticTilemapLayer;

  protected constructor(config: string | Phaser.Types.Scenes.SettingsConfig) {
    super(config);
    this.player = new Player(this);
    this.platforms = [];
    this.hitboxes = {};
    this.objects = [];
  }

  public preload(): void {
    this.load.multiatlas('emerl', 'assets/emerl.json', 'assets');
  }

  public create(): void {
    this.player.create();
    this.objects.forEach((object: LevelObject) => {
      object.create();
    });
    this.cameras.main.setBounds(0, 0, 1500, 600);
    this.cameras.main.setZoom(1.5);
  }

  public update(time: number, delta: number): void {
    this.platforms.forEach((platform: Platform) => {
      platform.update(delta / 1000);
      platform.debug(this.debug);
    });
    this.objects.forEach((object: LevelObject) => {
      object.update(time, delta, this.platforms, _.values(this.hitboxes));
      if (object.platform) {
        object.platform.update(delta / 1000);
        object.platform.debug(this.debug);
      }
    });

    const objectPlatforms = <Platform[]> _.chain(this.objects)
      .map((object: LevelObject) => object.platform)
      .filter((platform: Platform | null) => platform !== null)
      .value();
    const allPlatforms = this.platforms.concat(objectPlatforms);
    this.player.update(time, delta, allPlatforms);
    this.updateCamera();
    this.updateDebug();
  }

  public get debug(): DebugDrawPlugin {
    return (<any> this.sys).debug;
  }

  public get gameInput(): GameInputPlugin {
    return (<any> this.sys).gameInput;
  }

  public get collisionLayer(): Phaser.Tilemaps.StaticTilemapLayer {
    return this._collisionLayer;
  }

  public get bounds(): Phaser.Geom.Rectangle {
    const camera = this.cameras.main;
    if (this.isPaused) {
      const rect = new Phaser.Geom.Rectangle(0, 0, camera.width / camera.zoom, camera.height / camera.zoom);
      rect.centerX = camera.scrollX + camera.width / 2;
      rect.centerY = camera.scrollY + camera.height / 2;
      return rect;
    } else {
      return camera.getBounds();
    }
  }

  public get size(): Vector2 {
    const bounds = this.cameras.main.getBounds();
    return new Vector2(bounds.width, bounds.height);
  }

  public addHitbox(hitbox: Hitbox): void {
    this.hitboxes[hitbox.tag] = hitbox;
  }

  public removeHitbox(tag: string): void {
    delete this.hitboxes[tag];
  }

  protected updateCamera(): void {
    const camera = this.cameras.main;
    const dx = Scalar.clamp(this.player.position.x - camera.scrollX - camera.width / 2, -this.cameraSpeed, this.cameraSpeed);
    const dy = Scalar.clamp(this.player.position.y - camera.scrollY - camera.height / 2, -this.cameraSpeed, this.cameraSpeed);
    camera.scrollX += dx;
    camera.scrollY += dy;
  }

  protected updateDebug(): void {
    const bounds = this.bounds;
    this.debug.drawRect(bounds.left, bounds.top, bounds.width, bounds.height, 0xffff00, 3);
    _.forEach(this.hitboxes, (hitbox: Hitbox) => {
      this.debug.drawRect(hitbox.box.x, hitbox.box.y, hitbox.box.width, hitbox.box.height, 0xff0000);
    });
  }
}
