import { GameInputPlugin } from 'src/plugins/gameInput.plugin';
import * as Phaser from 'phaser';
import { DebugDrawPlugin } from 'src/plugins/debug.plugin';

export class Scene extends Phaser.Scene {
  public isPaused = false;

  public get debug(): DebugDrawPlugin {
    return (<any> this.sys).debug;
  }

  public get gameInput(): GameInputPlugin {
    return (<any> this.sys).gameInput;
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
}
