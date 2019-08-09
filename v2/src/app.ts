import * as Phaser from 'phaser';

enum AnimationKey {
  IDLE = 'IDLE',
  WALK = 'WALK'
}

class FooScene extends Phaser.Scene {

  constructor(config: string | Phaser.Types.Scenes.SettingsConfig) {
    super(config);
  }

  public preload(): void {
    this.load.multiatlas('emerl', 'assets/emerl.json', 'assets');
  }

  public create(): void {
    const emerl = this.add.sprite(100, 100, 'emerl', 'idle/Idle01.png');
    emerl.setScale(2);
    const frameNames = this.anims.generateFrameNames('emerl', {
      start: 1,
      end: 6,
      zeroPad: 2,
      prefix: 'idle/Idle',
      suffix: '.png'
    });
    this.anims.create({ key: AnimationKey.IDLE, frames: frameNames, frameRate: 10, repeat: -1 });
    emerl.anims.play(AnimationKey.IDLE);
  }

  public update(): void {
    // this.updateInputs();
  }

  private updateInputs(): void {
  }
}

const config: Phaser.Types.Core.GameConfig = {
  type: Phaser.AUTO,
  parent: 'phaser-example',
  width: 800,
  height: 600,
  scene: FooScene,
};

new Phaser.Game(config);

// function preload(this: Phaser.Scene) {
//   this.load.image('logo', logoImg);
// }
//
// function create(this: Phaser.Scene) {
//   const logo = this.add.image(400, 150, 'logo');
//
//   this.tweens.add({
//     targets: logo,
//     y: 450,
//     duration: 2000,
//     ease: 'Power2',
//     yoyo: true,
//     loop: -1,
//   });
// }
