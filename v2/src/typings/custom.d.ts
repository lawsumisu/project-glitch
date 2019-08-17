import { DebugDrawPlugin } from 'src/plugins/debug.plugin';
import { GameInputPlugin } from 'src/plugins/gameInput.plugin';

declare module '*.png' {
  const value: string;
  export default value;
}
