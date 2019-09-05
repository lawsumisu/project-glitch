import * as Phaser from 'phaser';

enum ConfigType {
  LINE = 'LINE',
  RECT = 'RECT',
  CIRCLE = 'CIRCLE',
}

interface DebugConfig {
  type: ConfigType;
  color: number;
  lineWidth: number;
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

interface CircleConfig extends DebugConfig {
  type: ConfigType.CIRCLE;
  x: number;
  y: number;
  r: number;
}

function isRect(config: DebugConfig): config is RectConfig {
  return config.type === ConfigType.RECT;
}

function isLine(config: DebugConfig): config is LineConfig {
  return config.type === ConfigType.LINE;
}

function isCircle(config: DebugConfig): config is CircleConfig {
  return config.type === ConfigType.CIRCLE;
}

export class DebugDrawPlugin extends Phaser.Plugins.ScenePlugin {
  private graphics: Phaser.GameObjects.Graphics | null = null;
  private configs: DebugConfig[] = [];

  public boot(): void {
    this.systems.events
      .on('start', this.onSceneStart)
      .on('postupdate', this.onScenePostUpdate)
      .on('shutdown', this.onSceneShutdown)
      .once('destroy', this.onSceneDestroy);
  }

  public drawLine(x1: number, y1: number, x2: number, y2: number, color: number = 0xffffff, lineWidth = 1): void {
    this.configs.push(<LineConfig> { type: ConfigType.LINE, x1, y1, x2, y2, color, lineWidth});
  }

  public drawRect(x: number, y: number, width: number, height: number, color: number = 0xffffff, lineWidth = 1): void {
    this.configs.push(<RectConfig> { type: ConfigType.RECT, x, y, width, height, color, lineWidth });
  }

  public drawCircle(x: number, y: number, r: number, color: number = 0xffffff, lineWidth = 1): void {
    this.configs.push(<CircleConfig> { type: ConfigType.CIRCLE, x, y, r, color, lineWidth});
  }

  private onSceneStart = (): void => {
    this.graphics = this.scene.add.graphics();
  }

  private onSceneShutdown = (): void => {
    if (this.graphics) {
      this.graphics.destroy();
    }
    this.graphics = null;
  }

  private onScenePostUpdate = (): void => {
    if (this.graphics) {
      this.graphics.clear();
      this.systems.displayList.bringToTop(this.graphics);
      this.configs.forEach((config: DebugConfig) => {
        if (this.graphics) {
          this.graphics.lineStyle(config.lineWidth, config.color);
          if (isLine(config)) {
            this.graphics.strokeLineShape(new Phaser.Geom.Line(config.x1, config.y1, config.x2, config.y2));
          } else if (isRect(config)) {
            this.graphics.strokeRect(config.x, config.y, config.width, config.height);
          } else if (isCircle(config)) {
            this.graphics.strokeCircleShape(new Phaser.Geom.Circle(config.x, config.y, config.r));
          }
        }
      });
      this.configs = [];
    }
  }

  private onSceneDestroy = () => {
    this.systems.events
      .off('start', this.onSceneStart)
      .off('render', this.onScenePostUpdate)
      .off('shutdown', this.onSceneShutdown)
      .off('destroy', this.onSceneDestroy);
  }
}
