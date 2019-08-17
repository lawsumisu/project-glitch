import * as Phaser from 'phaser';
import * as _ from 'lodash';

export enum GameInput {
  LEFT = 'LEFT',
  RIGHT = 'RIGHT',
  UP = 'UP',
  DOWN = 'DOWN',
  INPUT1 = 'INPUT1',
  INPUT2 = 'INPUT2',
  INPUT3 = 'INPUT3',
  INPUT4 = 'INPUT4',
  INPUT5 = 'INPUT5',
  INPUT6 = 'INPUT6',
}

enum InputType {
  KEYBOARD = 'KEYBOARD',
  GAMEPAD = 'GAMEPAD',
}

interface InputConfig {
  type: InputType;
}

interface KeyboardConfig extends InputConfig {
  type: InputType.KEYBOARD;
  key: number;
}

function isKeyboard(config: InputConfig): config is KeyboardConfig {
  return config.type === InputType.KEYBOARD;
}

type InputMap = {
  [key in GameInput]: InputConfig[];
};

export class GameInputPlugin extends Phaser.Plugins.ScenePlugin {
  public static defaultInputs = {
    [GameInput.DOWN]: [{ type: InputType.KEYBOARD, key: Phaser.Input.Keyboard.KeyCodes.DOWN }],
    [GameInput.UP]: [{ type: InputType.KEYBOARD, key: Phaser.Input.Keyboard.KeyCodes.UP }],
    [GameInput.RIGHT]: [{ type: InputType.KEYBOARD, key: Phaser.Input.Keyboard.KeyCodes.RIGHT }],
    [GameInput.LEFT]: [{ type: InputType.KEYBOARD, key: Phaser.Input.Keyboard.KeyCodes.LEFT }],
    [GameInput.INPUT1]: [],
    [GameInput.INPUT2]: [],
    [GameInput.INPUT3]: [],
    [GameInput.INPUT4]: [],
    [GameInput.INPUT5]: [],
    [GameInput.INPUT6]: [],
  };

  private inputMap: InputMap;
  private inputState: { [key in GameInput]: { isDown: boolean; duration: number }};

  public boot(): void {
    this.systems.events
      .on('start', this.onSceneStart)
      .on('update', this.onSceneUpdate)
      .once('destroy', this.onSceneDestroy);
  }

  public isInputPressed(input: GameInput): boolean {
    const state = this.inputState[input];
    return state.isDown && state.duration === 1;
  }

  public isInputReleased(input: GameInput): boolean {
    const state = this.inputState[input];
    return !state.isDown && state.duration === 1;
  }

  public isInputDown(input: GameInput, duration: number = 1): boolean {
    const state = this.inputState[input];
    return state.isDown && state.duration === duration;
  }

  public isInputUp(input: GameInput, duration: number = 1): boolean {
    const state = this.inputState[input];
    return !state.isDown && state.duration === duration;
  }

  public getDuration(input: GameInput): number {
    return this.inputState[input].duration;
  }

  private onSceneStart = (): void => {
    this.setupInputMap();
    this.clearInputs();
  }

  private onSceneUpdate = (): void => {
    _.forEach(this.inputMap, (configs: InputConfig[], input: GameInput) => {
      let isDown = false;
      for (const config of configs) {
        if (isKeyboard(config) && this.scene.input.keyboard.addKey(config.key).isDown) {
          // Found down input, so can exit early
          isDown = true;
          break;
        }
      }
      if (this.inputState[input].isDown !== isDown) {
        // Input was either just pressed or just released, so reset duration
        this.inputState[input].duration = 0;
      }
      this.inputState[input] = {
        isDown,
        duration: this.inputState[input].duration + 1,
      };
    });
  }

  private onSceneDestroy = (): void => {
    this.clearInputs();
  }

  private setupInputMap(): void {
    this.inputMap = {
      ...GameInputPlugin.defaultInputs,
    };
  }

  private clearInputs(): void {
    this.inputState = {
      [GameInput.DOWN]: { isDown: false, duration: 0 },
      [GameInput.UP]: { isDown: false, duration: 0 },
      [GameInput.RIGHT]: { isDown: false, duration: 0 },
      [GameInput.LEFT]: { isDown: false, duration: 0 },
      [GameInput.INPUT1]: { isDown: false, duration: 0 },
      [GameInput.INPUT2]: { isDown: false, duration: 0 },
      [GameInput.INPUT3]: { isDown: false, duration: 0 },
      [GameInput.INPUT4]: { isDown: false, duration: 0 },
      [GameInput.INPUT5]: { isDown: false, duration: 0 },
      [GameInput.INPUT6]: { isDown: false, duration: 0 },
    };
  }
}
