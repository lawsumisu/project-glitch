import * as Phaser from 'phaser';
import { Player } from "src/player";
import { DebugDrawPlugin } from "src/plugins/debug";
import { Platform } from "src/platform";
import { Vector2 } from "src/utilities/vector/vector";

class FooScene extends Phaser.Scene {
  private player: Player;
  private colliders: Phaser.Geom.Rectangle[];
  private platforms: Platform[];

  constructor(config: string | Phaser.Types.Scenes.SettingsConfig) {
    super(config);
    this.player = new Player(this);
  }

  public preload(): void {
    this.load.multiatlas('emerl', 'assets/emerl.json', 'assets');
  }

  public create(): void {
    this.player.create();
    this.colliders = [new Phaser.Geom.Rectangle(350, 420, 100, 75)];
    this.platforms = [new Platform({
      width: 100,
      height: 35,
      speed: 60,
      trackPoints: [
        new Vector2(200, 450), new Vector2(250, 450)
      ]})]
  }

  public update(time: number, delta: number): void {
    this.player.update(time, delta, this.colliders);
    this.colliders.forEach((collider: Phaser.Geom.Rectangle) => {
     this.debugPlugin.drawRect(collider.x, collider.y, collider.width, collider.height, 0xffff00);
    });
    this.platforms.forEach((platform: Platform) => {
      platform.update(delta / 1000);
      platform.draw(this.debugPlugin);
    });
  }

  private get debugPlugin(): DebugDrawPlugin {
    return (<any> this.sys).debug;
  }

}

const config: Phaser.Types.Core.GameConfig = {
  type: Phaser.AUTO,
  parent: 'phaser-example',
  width: 800,
  height: 600,
  scene: FooScene,
  plugins: {
    scene: [
      { key: 'debug', plugin: DebugDrawPlugin, mapping: 'debug' }
    ]
  }
};

new Phaser.Game(config);
