import * as Phaser from 'phaser';
import { Player } from "src/player";
import { DebugDrawPlugin } from "src/plugins/debug";

class FooScene extends Phaser.Scene {
  private player: Player;

  constructor(config: string | Phaser.Types.Scenes.SettingsConfig) {
    super(config);
    this.player = new Player(this);
  }

  public preload(): void {
    this.load.multiatlas('emerl', 'assets/emerl.json', 'assets');
  }

  public create(): void {
    this.player.create();
  }

  public update(time: number, delta: number): void {
    this.player.update(time, delta);
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
