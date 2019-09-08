import * as Phaser from 'phaser';
import { Scene } from 'src/utilities/phaser.util';
import { RingBuffer } from 'src/utilities/ringBuffer/ringBuffer.util';
import { Vector2 } from 'src/utilities/vector/vector';

export class AfterimageData {
  private afterimages: Phaser.GameObjects.Sprite[] = [];
  private frames: RingBuffer<{key: string; flipX: boolean; origin: Vector2}>;
  private positions: RingBuffer<Vector2>;
  private originalSprite: Phaser.GameObjects.Sprite;

  constructor(originalSprite: Phaser.GameObjects.Sprite, afterimageCount: number, historyLength: number, scene: Scene) {
    this.originalSprite = originalSprite;
    for (let i = 0; i < afterimageCount; ++i) {
      const sprite = scene.add.sprite(originalSprite.x, originalSprite.y, originalSprite.frame.texture.key, originalSprite.frame.name);
      this.afterimages.push(sprite);
    }
    this.frames = new RingBuffer<{key: string; flipX: boolean; origin: Vector2}>(historyLength);
    this.positions = new RingBuffer<Vector2>(historyLength);
  }

  public update(isVisible?: boolean): void {
    const { flipX, originX, originY } = this.originalSprite;
    this.frames.push({key: this.originalSprite.frame.name, flipX, origin: new Vector2(originX, originY)  });
    this.positions.push(new Vector2(this.originalSprite.x, this.originalSprite.y));
    for (let i = 0; i < this.afterimages.length; i++) {
      const j = Math.round(i * (this.frames.size - 1) / this.afterimages.length);
      const sprite = this.afterimages[i];
      const position = this.positions.at(j);
      const frameData = this.frames.at(j);
      if (position) {
        sprite.x = position.x;
        sprite.y = position.y;
        sprite.setFrame(frameData.key);
        sprite.flipX = frameData.flipX;
        sprite.setOrigin(frameData.origin.x, frameData.origin.y);
      }

      if (isVisible) {
        sprite.alpha = .3 + .4 * i / (this.afterimages.length - 1);
      } else {
        sprite.alpha = 0;
      }
    }
  }
}
