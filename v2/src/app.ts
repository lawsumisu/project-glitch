import * as Phaser from 'phaser';
import { DebugDrawPlugin } from 'src/plugins/debug.plugin';
import { GameInputPlugin } from 'src/plugins/gameInput.plugin';
import { TestLevel2 } from 'src/levels/test2.level';

const gameConfig: Phaser.Types.Core.GameConfig = {
  type: Phaser.AUTO,
  parent: 'phaser-example',
  width: 800,
  height: 600,
  scene: TestLevel2,
  input: {
    gamepad: true,
  },
  plugins: {
    scene: [
      { key: 'debug', plugin: DebugDrawPlugin, mapping: 'debug' },
      { key: 'gameInput', plugin: GameInputPlugin, mapping: 'gameInput'},
    ],
  },
};

new Phaser.Game(gameConfig);
