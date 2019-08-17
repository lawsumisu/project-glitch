import { GameInputPlugin } from 'src/plugins/gameInput.plugin';
import * as Phaser from 'phaser';
import { DebugDrawPlugin } from 'src/plugins/debug.plugin';

export class Scene extends Phaser.Scene {
  public get debug(): DebugDrawPlugin {
    return (<any> this.sys).debug;
  }

  public get gameInput(): GameInputPlugin {
    return (<any> this.sys).gameInput;
  }
}
