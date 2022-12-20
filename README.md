# Snake Game :joystick:
Snake game developed within the "Introduction to Robotics" course, 3rd year, 1st semester

## Introduction 
&ensp; This project consists of an extended snake game along with an intuitive menu designed to customize the playing session after user's preferences and, at the same time, to facilitate the user's experience. I have chosen to develop this game because I wanted to make the classic snake game more fun and interesting. With that in mind, I have created 2 more ways to play the game, one more difficult than the other, based on the classic game.   
&ensp; The whole project was a challenge, not only because I wanted to improve the existing snake game, but also since it was my first time developing something this big that combines hardware and software. 

## Game description
&ensp; The game itself is displayed on the 8x8 matrix and the game status can be seen on the LCD display. The main idea of the game is that the player has to eat as many foods as they can, in order to increase score. The game is focused on the score, and not on the time the player takes to play. The food is generated random at the beginning of the game and each time the snake eats one. Additionally, the food is distinguished from the snake and other obstacles by the fact that it is blinking. At the beginning of each game the snake has a length of 2 and is moving in a horizontal direction, starting from the second column of the row #3.   
There are 3 difficulty levels the player can choose from the menu:  

- **EASY**:  
&ensp; The snake is moving as the player moves the joystick in one of the 4 directions -up, down, left, right- and increases in size after each food eaten. Moreover, the speed of the snake also increases gradually after each food eaten. Although the speed might not seem to differ at first, it gets really obvious when the snake gets longer. It is possibile to teleport through the walls, meaning that if the snake exits the matrix it is immediately reappearing in the opposite side of matrix, as if the matrix is circular.

- **MEDIUM**:   
&ensp; This level is built starting from the previous one's logic and has one addition. Three random generated walls are added at the very beginning of the game. Even though they are random, they respect a few rules:
1. They have the length of 2 on the matrix
2. Each wall is generated in a specific area and has a predefined orientation, as described in the photo:
<img src="./Resources/Areas for walls.png" width=30% height=30%>

- **HARD**:   
&ensp; The last difficulty level takes the elements from the previous ones and comes with a fun challenge! The snake can now teleport only if the corresponding matrix  walls are 'activated'. They are marked using LEDs of 2 different colors (blue for left and right and yellow for up and down). This time the player has to use both hands to play, one for the joystick control and the second for pressing the button when needed.   

:pause_button: No matter the difficulty selected, the player can PAUSE the game if the joystick button is pressed for a few moments. The matrix freezes completely and to resume the game the player has to press again the joystick button. 



