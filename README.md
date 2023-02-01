# Endless Dungeon Project

## The Assignment

A project where the goal was to implement random generation to create a rougelike game where every playthrough would be different

## The Game

I created a game that was inspired by "The Oregon Trail" and "FTL: Faster Than Light" where the player needs to traverse through the random environment and strategically use the resources that the player has to reach the goal while random encounters and enemies tries to stop them.

### Gameplay

In the game you start out seeing a map of randomly generated areas. All of these areas are connected with eachother and the player starts at the one in the lower left corner of the map. The player can then enter that area and then you get transported to a new screen where you find a randomly generated city with roads and houses. The player can walk around in the city and also enter the houses. When the player enters a house, you get transported to a new screen with a randomly generated house, which contains walls and chests which contains resources. The player can exit the house which returns them to the city. If the player goes to the exit of the city, they get transported back to the map, which now has the neighboring areas unlocked because they have explored the connected city. Also everytime the player enters a city, it costs them resources so the player has to find enough resources in the cities and houses so they can travel.

### The Map

The map is generated using a couple of techniques. Firstly, a set of nodes are evenly distributed randomly using Poisson Disc Sampling. The technique createas a more uniform distribution and also 