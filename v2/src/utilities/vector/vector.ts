export interface Point {
  x: number;
  y: number;
}

export class Vector2 implements Point {
  public x: number;
  public y: number;

  constructor(x: number, y: number) {
    this.x = x;
    this.y = y;
  }

  public static get ZERO(): Vector2 {
    return new Vector2(0, 0);
  }

  public add(v: Vector2): Vector2 {
    return new Vector2(this.x + v.x, this.y + v.y);
  }

  public subtract(v: Vector2): Vector2 {
    return new Vector2(this.x - v.x, this.y - v.y);
  }

  public negate(): Vector2 {
    return new Vector2(-this.x, -this.y);
  }

  public scale(s: number): Vector2 {
    return new Vector2(this.x * s, this.y * s);
  }

  public normalize(): Vector2 {
    if (this.x === 0 && this.y === 0) {
      return this;
    } else {
      return this.scale(1 / this.magnitude());
    }
  }

  public magnitude(): number {
    return Math.sqrt(Math.pow(this.x, 2) + Math.pow(this.y, 2));
  }
}
