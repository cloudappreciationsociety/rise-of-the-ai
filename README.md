# rise-of-the-ai

Requirement 1: Menu Screen 
Show the name of your game and the text "Press enter to start" (or any variation thereof). 

The keycode for enter is: KEY_ENTER 

This can be a solid color background with text on it. 

The menu must be a different Scene object. Do not just show/hide text.

Requirement 2: 3 Levels 
Your game needs to have 3 levels of some increasing difficulty. 

They do not need to be long or complicated. 

They must scroll! (no single screen games) 

This is a platformer game, which means it needs to have platforms, but again you can get creative here.

“Protagonist” should be fully animated.

Implement a system through which you can switch levels automatically using the number keys, think of this as a debugging feature. 

Requirement 3: Lives System
The player gets a limited amount of lives for the entire game (not per level).

If the player runs out of lives, show a “You Lose” text. This also must be a Scene object.

If the player gets to the end of your game, show text a “You Win” text. Likewise, this also must be a Scene object.

From either ending Scene, the game must be able to go back to the main menu scene and reset the lives. (i.e. like pressing a key)

Requirement 4: AI
At least 3 types of moving AI (i.e. wanderer, follower, flyer, lerper, etc.) 

If the player touches the AI, the player will lose 1 life.

Each of your levels must have some form of AI.

Requirement 5: Audio 
You must have at least 1 looping background music. 

There should be at least 3 sound effects (such as for jumping, when the player or AI dies, etc.)