import { Level } from 'src/levels/index';
import * as json from 'assets/test2.json';

export class TestLevel2 extends Level {
  public preload(): void {
    super.preload();
    this.load.image('tiles', 'assets/lfz-0.png');
    this.load.tilemapTiledJSON('map', 'assets/test2.json');
  }

  public create(): void {
    super.create();
    const map = this.make.tilemap({ key: 'map' });

    // Parameters are the name you gave the tileset in Tiled and then the key of the tileset image in
    // Phaser's cache (i.e. the name you used in preload)
    const tileset = map.addTilesetImage('lfz-0', 'tiles');

    // Parameters: layer name (or index) from Tiled, tileset, x, y
    const layer = map.createStaticLayer('Tile Layer 1', tileset, 0, 0);
    layer.setDepth(-1);
    this._collisionLayer = layer;

    this.cameras.main.setBounds(0, 0, json.width * json.tilewidth, json.height * json.tileheight);
  }
}
