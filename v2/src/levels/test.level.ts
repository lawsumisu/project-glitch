import { Level } from 'src/levels/index';
import { Ball } from 'src/objects/ball';
import * as Phaser from 'phaser';

export class TestLevel extends Level {
  constructor(config: string | Phaser.Types.Scenes.SettingsConfig) {
    super(config);
    this.objects.push(new Ball(this));
  }
}
