export class Scalar {
  public static clamp(value: number, lo: number, hi: number): number {
    return Math.min(Math.max(value, lo), hi);
  }
}
