# 2D Cutscene Manager

## Cutscene vs Cinematic?

The terminology usually used to refer to videos that are shown in a videogames is Cinematic.
Cinematic refers to all the sequences shown that don't involve player interactions but that sequences can be divided in different types:
- Live-action Cutscenes: Real actors are filmed on a constructed set.
- Pre-rendered Cutscenes: The scene is animated and rendered beforehand.
- Real time Cutscene: The scene is rendered at the same time it happens using the game engine.
- Mixed media Cutscene: They are a combination between pre-rendered and real time cutscenes.
- Interactive Cutscene: The game takes control of the character and request the player for some inputs during the cinematic.

## How other engines do cutscenes?

Most of the free engines use a similar system. The have a tool that allow to change position, size, rotation and all available properties for that object during time.
For that they use what are usually called key-frames. 
Key-frames are points where the developer assign a value for the properties the object have. Then the game engine, using interpolations, transform the object between these key-frames to create a smooth animation.

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


- **Creating Elements**

The Cutscene Manager classify the elements depending the group they have. 
The groups that I created as examples are:
- Map
- Image
- Entity
- Music
- Sound Effect
- Text

Appling that to the xml file, should create a structure similar to this one:
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
- path: the path of the file containing element data (can be an image path, xml, music...)

The camera is an element that don't need to be created. The camera already exist and actions can be applied to it.

**_Creating a Map_**

The map module that comes with this research allow to load one map and draw it on the screen. 
Map don't have any unique parameter that needs to be set but it can already be loaded before the scene is played.

This is the attribute needed to create a map:
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

Images need these attributes:
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

Entity is an element that can perform pre-programed actions inside the engine. I've created a simple entity system that allow them to move and do an action.

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

Music is a sound that will loop until stopped.

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

Example of creating a sound effect:
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


- **Creating Actions**

Actions are each change that happens on the cutscene. From a camera movement to a change in the text.

Actions have a type that define what they should do. Depending on the type they behaviour and definition are different.

This manager has eight different actions:
- Move
- Action
- Play
- Stop
- Modify
- Enable
- Disable
- Change Scene

All actions have common attributes:
- element: name of the element to act.
- type: kind of action to perform.
- start: start time in seconds
- duration: time that takes to finish the action.

Example of action:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act type="change_scene" start="17" duration="4">
			<!-- Here go all extra attributes needed -->
		</act>
	</scene>
</file>

```


**_Creating a Move_**

A move is a change in the position. 

Move action can be done on these elements:
- Image
- Entity
- Text
- Camera

To create a move, these are the extra attributes needed:
- x: final x position
- y: final y position
- ref: reference type (local, global, map). This only apply to entities, other elements use global reference always.
- bezier: defines if a bezier easing is used.
- p1: bezier easing point 1 (needs a x and y value);
- p2: bezier easing point 2 (needs a x and y value);

Example of Movement:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act element="camera" type="move" start="4" duration="5">
			<move x="300" y="500" ref="global" bezier="true">
				<p1 x="1" y="0"/>
				<p2 x="0" y="1"/>
			</move>
		</act>
	</scene>
</file>

```


**_Creating an Action_**

Just entities have actions. From the entities provided with the example just link have action.

Action don’t need special attributes but they don't have to have duration.

Example of Action:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act element="link" type="action" start="2"/>
	</scene>
</file>

```


**_Creating a Play_**

Play only apply to music and sound effects. The same way that happens with actions, play actions don’t need a duration.

Example of Play:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act element="main_music" type="play" start="9"/>
	</scene>
</file>

```


**_Creating a Stop_**

Stop is the inverse of Play but only affects to music.

Example of Stop:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act element="main_music" type="stop" start="5"/>
	</scene>
</file>

```


**_Creating a Modify_**

Modify change instantly a property of an element. Modify can be applied to entities, images and texts.

Depending on the element the attributes needed are different.

_Modify an entity_

Entities can be modified in 3 ways:
- kill
- spawn
- pos

The attributes needed for this modify are:
- action: way to modify the entity.
- x: x position to spawn or move.
- y: y position to spawn or move.

Example of modify entity:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act element="link" type="modify" start="10">
			<modify action="kill" x="100" y="250"/>
		</act>
	</scene>
</file>

```

_Modify an image_

Images can be modified in 3 ways:
- textures (tex)
- rect
- both

The attributes needed for this modify are:
- var: variable to modify.
- path: path of the new texture.
- x: x position of the new rect.
- y: y position of the new rect.
- w: width of the new rect.
- h: height of the new rect.

Example of modify image:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act element="test_image" type="modify" start="10">
			<modify var="both" path="textures/test_texture.png" x="100" y="250" w="50" h="75"/>
		</act>
	</scene>
</file>

```

_Modify a text_

Modify a text change the string shown on screen for a new one.

A text is needed to modify the text.

Example:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act element="test_image" type="modify" start="10">
			<modify txt="Modified text."/>
		</act>
	</scene>
</file>

```


_Modify the camera_

Camera position can be modified. 

The attributes needed are:
- x: camera x new position.
- y: camera y new position.

Example of camera modify:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act element="camera" type="modify" start="10">
			<modify x="25" y="250"/>
		</act>
	</scene>
</file>

```


**_Creating a Enable_**

Enable change the state of an element active parameter to true.

Enable don't need special attributes. Enable don't need a duration.

Example of enable:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act element="info_text" type="enable" start="12"/>
	</scene>
</file>

```


**_Creating a Disable_**

Disable is the opposite of enable. 

Disable don't need special attributes. Disable don't need a duration.

Example of disable:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act element="info_text" type="disable" start="12"/>
	</scene>
</file>

```


**_Creating a Change Scene_**

Change Scene change current scene to another one. 

Change Scene need these attributes:
- path: the new cutscene to be played.
- effect: the effect to reproduce between scenes. (circle, star, fade)

This is an example of Change Scene:
```markdown
<file>
	<elements>
		<!-- Here go all elements that appear at the scene -->
	</elements>
	<scene>
		<act type="change_scene" start="17" duration="4">
			<change_scene path="test_cutscene.xml" effect="circle"/>
		</act>
	</scene>
</file>

```

## Exercise:

The objective of this exercise is to help understanding how the manager works.

- **TODO 1: I'm too far, I can't help Zelda**

Move link to make his attack reach the skeleton.
For this you need to create a move action.
The target destination should be: x="784" y="610"
The movement should start after 6 sec.
Check "Creating a Move" for all needed attributes.

- **TODO 2: That's not my place**

Change scenes between the actual one and link house one.
The effect used on solution is circle.
The change should start after 35 sec.

- **TODO 3: And now... What**

Create the link house scene.
You can reproduce the same one from the solution or create your own version.
Needed values:
- Link House image: path: textures/Link's_House.png pos_x: 414 pos_y: 182 rect_x: 0 rect_y:0 rect_w: 452 rect_h: 356
- Music: path: Audio/Music/Majora's Mask- Giant's Theme.ogg
- Link: pos_x: 525 pos_y: 265

_Pro tip: you can change scenes without a next scene._

## Useful Links

[https://www.giantbomb.com/in-engine-cinematic/3015-964/](https://www.giantbomb.com/in-engine-cinematic/3015-964/)
[https://unity3d.com/es/learn/tutorials/topics/animation/animation-view?playlist=17099](https://unity3d.com/es/learn/tutorials/topics/animation/animation-view?playlist=17099)
[https://en.wikipedia.org/wiki/Cutscene](https://en.wikipedia.org/wiki/Cutscene)
