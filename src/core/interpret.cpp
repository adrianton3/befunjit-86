#include "interpret.h"


InterpretResult interpret (Playfield& playfield, Cursor cursor, int64_t* stack, int64_t offset, const StaticBindings& staticBindings, uint64_t stepCountMax) {
    uint64_t stepCount = 0;

    bool stringMode = false;

    while (true) {
        stepCount++;

        const auto value = playfield.getAtUnsafe(cursor.location.x, cursor.location.y);

        if (stringMode) [[unlikely]] {
            if (value == '"') {
                stringMode = false;
            } else {
                stack[offset] = value;
                offset++;
            }
            cursor.advance();
            continue;
        }

        switch (value) {
            case '0': [[fallthrough]];
            case '1': [[fallthrough]];
            case '2': [[fallthrough]];
            case '3': [[fallthrough]];
            case '4': [[fallthrough]];
            case '5': [[fallthrough]];
            case '6': [[fallthrough]];
            case '7': [[fallthrough]];
            case '8': [[fallthrough]];
            case '9': {
                stack[offset] = value - '0';
                offset++;
                break;
            }

            case '^': cursor.directUp(); break;
            case '<': cursor.directLeft(); break;
            case 'v': cursor.directDown(); break;
            case '>': cursor.directRight(); break;

            case '#': cursor.advance(); break;

            case '+': {
                stack[offset - 2] += stack[offset - 1];
                offset--;
                break;
            }

            case '-': {
                stack[offset - 2] -= stack[offset - 1];
                offset--;
                break;
            }

            case '*': {
                stack[offset - 2] *= stack[offset - 1];
                offset--;
                break;
            }

            case '/': {
                stack[offset - 2] /= stack[offset - 1];
                offset--;
                break;
            }

            case '%': {
                stack[offset - 2] %= stack[offset - 1];
                offset--;
                break;
            }

            case '!': {
                stack[offset - 1] = !stack[offset - 1];
                break;
            }

            case '`': {
                stack[offset - 2] = stack[offset - 2] > stack[offset - 1];
                offset--;
                break;
            }

            case '"': {
                stringMode = true;
                break;
            }

            case ':': {
                stack[offset] = stack[offset - 1];
                offset++;
                break;
            }

            case '\\': {
                const auto tmp = stack[offset - 2];
                stack[offset - 2] = stack[offset - 1];
                stack[offset - 1] = tmp;
                break;
            }

            case '$': {
                offset--;
                break;
            }

            case '_': {
                if (stack[offset - 1] == 0) {
                    cursor.directRight();
                } else {
                    cursor.directLeft();
                }
                offset--;
                break;
            }

            case '|': {
                if (stack[offset - 1] == 0) {
                    cursor.directDown();
                } else {
                    cursor.directUp();
                }
                offset--;
                break;
            }

            case '?': {
                switch (std::rand() % 4) {
                    case 0: cursor.directUp(); break;
                    case 1: cursor.directLeft(); break;
                    case 2: cursor.directDown(); break;
                    default: cursor.directRight(); break;
                }
                break;
            }

            case 'g': {
                const auto x = stack[offset - 2];
                const auto y = stack[offset - 1];
                stack[offset - 2] = Playfield::isWithinBounds(x, y) ? playfield.getAtUnsafe(x, y) : 0;
                offset--;
                break;
            }

            case 'p': {
                const auto value = stack[offset - 3];
                const auto x = stack[offset - 2];
                const auto y = stack[offset - 1];
                if (Playfield::isWithinBounds(x, y)) {
                    playfield.setAtUnsafe(x, y, value);
                }
                offset -= 3;
                break;
            }

            case '.': {
                staticBindings.writeInt64(stack[offset - 1]);
                offset--;
                break;
            }

            case ',': {
                staticBindings.writeChar(stack[offset - 1]);
                offset--;
                break;
            }

            case '&': {
                stack[offset] = staticBindings.readInt64();
                offset++;
                break;
            }

            case '~': {
                stack[offset] = staticBindings.readChar();
                offset++;
                break;
            }

            case '@': return { cursor, offset, true };

            default:; // nothing
        }

        if (stepCount > stepCountMax) {
            break;
        }

        cursor.advance();
    }

    return { cursor, offset, false };
}