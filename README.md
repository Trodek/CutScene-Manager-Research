# 2D Cutscene Manager

## Cutscene vs Cinematic?

The trminology usually used to refer to videos that are shown in a videogmaes is Cinematic.
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

The solution that I proposed is a real time cutscene system based on SDL2.0, requires access to XML files and the engine needs to have their own entity, GUI, map and audio system.
This solutions is a base to be implemented on other engines so the functionality is reduced just to show how to implement it.

### UML

[![](https://i.gyazo.com/4396083f2a0e759d0492b84a06545235.png)](https://i.gyazo.com/4396083f2a0e759d0492b84a06545235.png)

### Creating a Cutscene

To create a cutscene, first be sure that all elements that need to appear are implemented on the engine and the cutscene manager support them.
After that, create an xml file and use this base structure:

```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<!-- Here go all the actions that should be done during the scene -->
	</scene>
</file>

```

**Creating Elements**

The Cutscene Manager classify the elements depending the group they have. 
The groups that I created as examples are:
- Map
- Image
- Entity
- Music
- Sound Effect
- Text

Appling that to the xml file, shoul create a structure similar to this one:
```markdown
<file>
	<elements>
		<type group="map">
			<!-- Here go all elements for group map -->
		</type>
		<type group="image">
			<!-- Here go all elements for group image -->
		</type>
		<type group="entity">
			<!-- Here go all elements for group entity -->
		</type>
	</elements>
	<scene>
		<!-- Here go all the actions that should be done during the scene -->
	</scene>
</file>

```

All elements have common attributes that need to be defined on the XML:
- name: the name of the element.
- active: define if the element is active when scene starts.
- path: the path of the file cotining element data (can be an image path, xml, music...)

**_Creating a Map_**

The map madule that comes with this research allow to load one map and draw it on the screen. 
Map don't have any unique parameter that needs to be set but it can already be loaded befor the scene is played.

This is the attribut needed to create a map:
- preload: tells the manager if the map is already loaded.

Map XML example:
```markdown
<file>
	<elements>
		<type group="map">
			<map preload ="false" name="map" active="true" path="cut_scene_map.tmx"/>
		</type>
	</elements>
	<scene>
		<!-- Here go all the actions that should be done during the scene -->
	</scene>
</file>

```

**_Creating an Image_**

Images are static elements that can be drawn at any point of the screen, can also be used to create text boxes.

Images need these atributes:
- pos_x: x position on the screen.
- pos_y: y position on the screen.
- rect_x: x position of the image rectangle inside the texture.
- rect_y: y position of the image rectangle inside the texture.
- rect_w: image width inside the texture.
- rect_h: image height inside the texture.
- layer: the layer the image should be printed.

Image creation example:
```markdown
<file>
	<elements>
		<type group="image">
			<image name="test_image" active="true" path="textures/Base_entity.png" pos_x ="20" pos_y="60" rect_x="0" rect_y="0" rect_w="90" rect_h="90" layer="2"/>
		</type>
	</elements>
	<scene>
		<!-- Here go all the actions that should be done during the scene -->
	</scene>
</file>

```

**_Creating an Entity_**

Entity is an element that can perform  pre-programed actions inside the engine. I've created a simple entity system that allow them to move and do an action.

Entities need these attributes:
- pos_x: initial x position on screen.
- pos_y: initial y position on screen.

Examlpe of entity creation:
```markdown
<file>
	<elements>
		<type group="entity">
			<entity name ="link" active="true" path="link.xml" pos_x="50" pos_y="50"/>
		 </type>
	</elements>
	<scene>
		<!-- Here go all the actions that should be done during the scene -->
	</scene>
</file>

```

**_Creating a Music_**

Music is a sound that will loop until stoped.

Music don't need any unique attribute.

Music creation example:
```markdown
<file>
	<elements>
		<type group="music">
			<music name="main_music" active="true" path="Audio/Music/guessing-game_house.ogg"/>
		</type>
	</elements>
	<scene>
		<!-- Here go all the actions that should be done during the scene -->
	</scene>
</file>

```

**_Creating a Sound Effect_**

Sound effects are short sounds that are played the specified number of times.

Sound Effects need this attribute:
- loops: number of times a sound effect needs to be played.

Example of creating a sounf effect:
```markdown
<file>
	<elements>
		<type group="sound_effect">
			<sound_effect name="hello_man" active="true" path="Audio/FX/hello_man.wav" loops="0"/>
		</type>
	</elements>
	<scene>
		<!-- Here go all the actions that should be done during the scene -->
	</scene>
</file>

```

**_Creating a Text_**

Text is always created inside screen. If the camera is moved, text will follow camera movement.

Text need these attributes:
- pos_x: x position on the screen.
- pos_y: y position on the screen.
- txt: the initial text to be displayed.

Text creation example:
```markdown
<file>
	<elements>
		<type group="text">
			<text preload="false" name="info_text" active="true" pos_x="50" pos_y="250" txt="this is a test"/>
		</type>
	</elements>
	<scene>
		<!-- Here go all the actions that should be done during the scene -->
	</scene>
</file>

```

**Creating Actions**
