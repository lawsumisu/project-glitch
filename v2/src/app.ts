import * as Phaser from 'phaser';
import { Platform } from 'src/platform';
import { Player } from 'src/player';
import { DebugDrawPlugin } from 'src/plugins/debug.plugin';
import { Vector2 } from 'src/utilities/vector/vector';
import { GameInputPlugin } from 'src/plugins/gameInput.plugin';
import { Scene } from 'src/utilities/phaser.util';

class FooScene extends Scene {
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
        new Vector2(200, 450), new Vector2(250, 450), new Vector2(350, 350),
      ],
      scene: this,
    })];
  }

  public update(time: number, delta: number): void {
    this.player.update(time, delta, this.colliders.concat(this.platforms.map((platform: Platform) => platform.collider)));
    this.colliders.forEach((collider: Phaser.Geom.Rectangle) => {
     this.debug.drawRect(collider.x, collider.y, collider.width, collider.height, 0xffff00);
    });
    this.platforms.forEach((platform: Platform) => {
      platform.update(delta / 1000);
      platform.debug(this.debug);
    });
  }
}

const gameConfig: Phaser.Types.Core.GameConfig = {
  type: Phaser.AUTO,
  parent: 'phaser-example',
  width: 800,
  height: 600,
  scene: FooScene,
  plugins: {
    scene: [
      { key: 'debug', plugin: DebugDrawPlugin, mapping: 'debug' },
      { key: 'gameInput', plugin: GameInputPlugin, mapping: 'gameInput'},
    ],
  },
};

new Phaser.Game(gameConfig);
