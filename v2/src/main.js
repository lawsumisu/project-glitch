import 'pixi';
import 'p2';
import Phaser from 'phaser';

class Game extends Phaser.Game {
    constructor () {
        const docElement = document.documentElement;

        super(docElement.clientWidth, docElement.clientHeight, Phaser.CANVAS, 'content', null);
    }
  
    preload () {

        game.load.image('logo', 'assets/phaser.png');

    }

    create () {
        var logo = game.add.sprite(game.world.centerX, game.world.centerY, 'logo');
        logo.anchor.setTo(0.5, 0.5);
    }
}

window.game = new Game();
