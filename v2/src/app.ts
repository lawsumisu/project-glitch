import * as Phaser from 'phaser';
import { Platform } from 'src/platform';
import { Player } from 'src/player';
import { DebugDrawPlugin } from 'src/plugins/debug.plugin';
import { Vector2 } from 'src/utilities/vector/vector';
import { GameInputPlugin } from 'src/plugins/gameInput.plugin';
import { Scene } from 'src/utilities/phaser.util';
import { Scalar } from 'src/utilities/math/scalar.util';

class FooScene extends Scene {
  private player: Player;
  private platforms: Platform[];
  private cameraSpeed = 15;

  constructor(config: string | Phaser.Types.Scenes.SettingsConfig) {
    super(config);
    this.player = new Player(this);
  }

  public preload(): void {
    this.load.multiatlas('emerl', 'assets/emerl.json', 'assets');
  }

  public create(): void {
    this.player.create();
    this.cameras.main.setBounds(0, 0, 1200, 600);
    this.platforms = [
      new Platform({
        width: 100,
        height: 75,
        speed: 0,
        trackPoints: [ new Vector2(400, 455)],
        scene: this,
      }),
      new Platform({
        width: 100,
        height: 75,
        speed: 0,
        trackPoints: [ new Vector2(250, 555)],
        scene: this,
      }),
      new Platform({
        width: 100,
        height: 35,
        speed: 200,
        trackPoints: [
          new Vector2(200, 450), new Vector2(250, 450), new Vector2(350, 350),
        ],
        scene: this,
      }),
    ];
  }

  public update(time: number, delta: number): void {
    this.platforms.forEach((platform: Platform) => {
      platform.update(delta / 1000);
      platform.debug(this.debug);
    });
    this.player.update(time, delta, this.platforms);
    this.updateCamera();
  }

  private updateCamera(): void {
    if (this.isPaused) {
      return;
    } else {
      const camera = this.cameras.main;
      const dx = Scalar.clamp(this.player.position.x - camera.scrollX - camera.width / 2, -this.cameraSpeed,  this.cameraSpeed);
      const dy = Scalar.clamp(this.player.position.y - camera.scrollY - camera.height / 2, -this.cameraSpeed, this.cameraSpeed);
      camera.scrollX += dx;
      camera.scrollY += dy;
    }
  }
}

const gameConfig: Phaser.Types.Core.GameConfig = {
  type: Phaser.AUTO,
  parent: 'phaser-example',
  width: 400,
  height: 400,
  scene: FooScene,
  plugins: {
    scene: [
      { key: 'debug', plugin: DebugDrawPlugin, mapping: 'debug' },
      { key: 'gameInput', plugin: GameInputPlugin, mapping: 'gameInput'},
    ],
  },
};

new Phaser.Game(gameConfig);
