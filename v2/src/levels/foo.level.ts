import { Level } from 'src/levels/index';
import { Platform, PlatformType } from 'src/platform';
import { Vector2 } from 'src/utilities/vector/vector';

function createPlatformHelix(level: Level): Platform[] {
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
        trackPoints: [new Vector2(500, y), new Vector2(600, y)],
        level,
        reverseTimer: 1,
        type,
      }),
    );
  }
  return platforms;
}

function createPlatformRiver(level: Level, length: number): Platform[] {
  const width = 100;
  const height = 100;
  const startX = 650;
  return [
    new Platform({
      width,
      height,
      speed: 0,
      trackPoints: [new Vector2(650, 550)],
      level,
    }),
    new Platform({
      width: length,
      height,
      speed: 0,
      trackPoints: [new Vector2(startX + width / 2 + length / 2, 550)],
      level,
      type: PlatformType.FLUID,
    }),
    new Platform({
      width,
      height,
      speed: 0,
      trackPoints: [new Vector2(startX + width + length, 550)],
      level,
    }),
  ];
}

export class FooScene extends Level {
  public create(): void {
    super.create();
    this.platforms = [
      new Platform({
        width: 100,
        height: 400,
        speed: 0,
        trackPoints: [new Vector2(50, 400)],
        level: this,
      }),
      new Platform({
        width: 100,
        height: 75,
        speed: 0,
        trackPoints: [new Vector2(400, 455)],
        level: this,
      }),
      new Platform({
        width: 100,
        height: 75,
        speed: 0,
        trackPoints: [new Vector2(250, 555)],
        level: this,
      }),
      new Platform({
        width: 100,
        height: 35,
        speed: 200,
        trackPoints: [
          new Vector2(200, 450), new Vector2(250, 450), new Vector2(250, 50),
        ],
        level: this,
      }),

      ...createPlatformHelix(this),
      ...createPlatformRiver(this, 500),
    ];
  }
}
