import * as Phaser from 'phaser';
import { DebugDrawPlugin } from 'src/plugins/debug.plugin';
import { GameInputPlugin } from 'src/plugins/gameInput.plugin';
import {TestLevel} from 'src/levels/test.level';

const gameConfig: Phaser.Types.Core.GameConfig = {
  type: Phaser.AUTO,
  parent: 'phaser-example',
  width: 800,
  height: 600,
  scene: TestLevel,
  plugins: {
    scene: [
      { key: 'debug', plugin: DebugDrawPlugin, mapping: 'debug' },
      { key: 'gameInput', plugin: GameInputPlugin, mapping: 'gameInput'},
    ],
  },
};

new Phaser.Game(gameConfig);
