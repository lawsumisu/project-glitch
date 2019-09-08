import { Scalar } from 'src/utilities/math/scalar.util';

export class RingBuffer<T = any> {
  public readonly size: number;
  private buffer: T[] = [];
  private ringIndex = 0;

  constructor(size: number) {
    if (size < 0) {
      throw new Error('size must be positive');
    } else if (size !== Math.round(size)) {
      throw new RangeError('Invalid array length');
    } else {
      this.size = size;
    }
  }

  public push(item: T): void {
    if (this.length < this.size) {
      this.buffer.push(item);
    } else {
      this.buffer[this.ringIndex] = item;
      this.ringIndex = (this.ringIndex + 1) % this.length;
    }
  }

  public at(index: number): T {
    if (index !== Math.round(index)) {
      throw new Error('index must be an integer');
    } else if (index !== Scalar.clamp(index, -this.size, this.size - 1)) {
      throw new Error(`index must be between ${-this.size} and ${this.size - 1}`);
    } else {
      const i = (this.ringIndex + index + this.size) % this.size;
      return this.buffer[i];
    }
  }

  public toArray(): T[] {
    const output: T[] = [];
    for (let i = 0; i < this.size; ++i) {
      output.push(this.at(i));
    }
    return output;
  }

  public get length(): number {
    return this.buffer.length;
  }
}
