# 2D Cutscene Manager

## What is a Cutscene and a Cinematic?

The trminology usually used to refer to a video that is shown in a videogmaes is Cinematic.
Cinematic refers to all the sequences shown that don't involve player interactions but that sequences can be divided in different types:
- Live-action Cutscenes: Real actors are filmed on a constructed set.
- Pre-rendered Cutscenes: The scene is animated and rendered beforhand.
- Real time Cutscene: The scene is rendered at the same time it happens using the game engine.
- Mixed media Cutscene: They are a convination between pre-rendered and real time cutscenes.
- Interactive Cutscene: The game takes control of the character and request the player for some inputs during the cinematic.

## How other engines do cutscenes?

Most of the free engines use a similar system. The have a tool that allow to change position, size, rotation and all available properties for that object during time.
For that they use what are usually called key-frames. 
Key-frames are points where the developer asign a value for the properties the object have. Then the game engine, using interpolations, transform the object betwen these key-frames to create a smoth animation.

Here are some video examples of how they are created on Unity and Unreal Engine 4:

- [Unity Cutscene](https://www.youtube.com/watch?v=DONjqcwwaBg)
- [Unreal Engine 4 Cutscene](https://www.youtube.com/watch?v=mrCEtE5RnHw)

## Proposed Solution:

The solution that I proposed is based on SDL2.0, requires access to XML files and the engine needs to have their own entity system, GUI and audio.
This solutions is a base to be implemented on other engines so the functionality is reduced just to show how to implement it.

### UML

![](https://i.gyazo.com/4396083f2a0e759d0492b84a06545235.png)

