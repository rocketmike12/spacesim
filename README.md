# spacesim
A dumb little simulation of gravitational forces between objects in space

# Building
You have to install [raylib](https://github.com/raysan5/raylib) to compile this program <br>
To compile, run
```shell
g++ spacesim.cpp -o spacesim -lraylib
```
or just
```shell
make build
```
# Running
You can run one of the prebundled simulations<br>
For a pixel-scale simulation demo
```shell
./spacesim examples/3body.jsonc
```
For a unit-scale simulation demo
```shell
./spacesim examples/earth_moon.jsonc
```
# Creating your own simulations
The format used for simulation files (jsonc) is as follows:
```jsonc
{
    // mode (string: "pixel" or "unit")
    "mode": "pixel",

    // screen width and height in pixels (int)
    "width": 512,
    "height": 512,

    // fps (int)
    "fps": 60,

    // scaling factor and time delta (double) (only required if mode is set to "unit", ignored otherwise)
	"S": 1.11e-6,
	"dT": 1440,

    // gravitational constant (double) 
    "G": 6.674e-11,

    // softening factor (double)
    "epsilon": 0.1,

    // array of body objects (all double)
    "bodies": [
        {
            "x": 0.0,
            "y": 0.0,
            "m": 1000.0,
            "R": 15.0,
            "v": {
                "x": 0.0,
                "y": 0.0
            }
        }
    ]
}
```
# Planned features
- dynamic radius calculation;
- screensaver mode;
- restart on out-of-bounds mode;
