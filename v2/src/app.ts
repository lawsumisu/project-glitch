import * as Phaser from 'phaser';
import { Platform, PlatformType } from 'src/platform';
import { Player } from 'src/player';
import { DebugDrawPlugin } from 'src/plugins/debug.plugin';
import { Vector2 } from 'src/utilities/vector/vector';
import { GameInputPlugin } from 'src/plugins/gameInput.plugin';
import { Scene } from 'src/utilities/phaser.util';
import { Scalar } from 'src/utilities/math/scalar.util';

function createPlatformHelix(scene: Scene): Platform[] {
  const width = 50;
  const height = 35;
  const speed = 45;
  const spacing = 75;
  const type = PlatformType.THIN;
  const platforms: Platform[] = [];
  for (let i = 0; i < 5; ++i) {
    const y = 500 - spacing * i;
    platforms.push(
      new Platform({
        width,
        height,
        speed,
        trackPoints: [ new Vector2(500, y), new Vector2(600, y)],
        scene,
        reverseTimer: 1,
        type,
      }),
    );
  }
  return platforms;
}
class FooScene extends Scene {
  private player: Player;
  private platforms: Platform[];
  private cameraSpeed = 10;

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
    this.cameras.main.setZoom(2);
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
          new Vector2(200, 450), new Vector2(250, 450), new Vector2(250, 50),
        ],
        scene: this,
      }),

      ...createPlatformHelix(this),
    ];
  }

  public update(time: number, delta: number): void {
    this.platforms.forEach((platform: Platform) => {
      platform.update(delta / 1000);
      platform.debug(this.debug);
    });
    this.player.update(time, delta, this.platforms);
    this.updateCamera();
    const bounds = this.bounds;
    this.debug.drawRect(bounds.left, bounds.top, bounds.width, bounds.height, 0xffff00, 3);
  }

  private updateCamera(): void {
    const camera = this.cameras.main;
    if (this.isPaused) {
      // camera.zoomTo(1.5, 50, 'Quad.easeInOut', true);
    } else {
      // camera.zoomTo(2, 50, 'Quad.easeInOut', true);
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
  width: 600,
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
