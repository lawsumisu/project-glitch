import * as Phaser from 'phaser';

enum ConfigType {
  LINE = 'LINE',
  RECT = 'RECT',
}

interface DebugConfig {
  type: ConfigType;
  color: number;
}

interface LineConfig extends DebugConfig {
  type: ConfigType.LINE;
  x1: number;
  y1: number;
  x2: number;
  y2: number;
}

interface RectConfig extends DebugConfig {
  type: ConfigType.RECT;
  x: number;
  y: number;
  width: number;
  height: number;
}

function isRect(config: DebugConfig): config is RectConfig {
  return config.type === ConfigType.RECT;
}

function isLine(config: DebugConfig): config is LineConfig {
  return config.type === ConfigType.LINE;
}

export class DebugDrawPlugin extends Phaser.Plugins.ScenePlugin {
  private graphics: Phaser.GameObjects.Graphics | null = null;
  private configs: DebugConfig[] = [];

  public boot (): void {
    this.systems.events
      .on('start', this.sceneStart, this)
      .on('render', this.sceneRender, this)
      .on('shutdown', this.sceneShutdown, this)
      .once('destroy', this.sceneDestroy, this);
  }

  public drawLine(x1: number, y1: number, x2: number, y2: number, color: number = 0xffffff): void {
    this.configs.push(<LineConfig> { type: ConfigType.LINE, x1, y1, x2, y2, color});
  }

  public drawRect(x: number, y: number, width: number, height: number, color: number = 0xffffff): void {
    this.configs.push(<RectConfig> { type: ConfigType.RECT, x, y, width, height, color })
  }

  private sceneStart (): void {
    this.graphics = this.scene.add.graphics();
  }

  private sceneShutdown (): void {
    if (this.graphics) {
      this.graphics.destroy();
    }
    this.graphics = null;
  }

  private sceneRender (): void {
    if (this.graphics) {
      this.graphics.clear();
      this.systems.displayList.bringToTop(this.graphics);
      this.configs.forEach((config: DebugConfig) => {
        if (this.graphics) {
          this.graphics.lineStyle(1, config.color);
          if (isLine(config)) {
            this.graphics.strokeLineShape(new Phaser.Geom.Line(config.x1, config.y1, config.x2, config.y2));
          } else if (isRect(config)) {
            this.graphics.strokeRect(config.x, config.y, config.width, config.height);
          }
        }
      });

      this.configs = [];
    }
  }

  private sceneDestroy () {
    this.systems.events
      .off('start', this.sceneStart, this)
      .off('render', this.sceneRender, this)
      .off('shutdown', this.sceneShutdown, this)
      .off('destroy', this.sceneDestroy, this);
  }
}