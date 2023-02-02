# Endless Dungeon Project

## The Assignment

A project where the goal was to implement random generation to create a rougelike game where every playthrough would be different

## The Game

I created a game that was inspired by "The Oregon Trail" and "FTL: Faster Than Light" where the player needs to traverse through the random environment and strategically use the resources that the player has to reach the goal while random encounters and enemies tries to stop them.

### Gameplay

In the game you start out seeing a map of randomly generated areas. All of these areas are connected with eachother and the player starts at the one in the lower left corner of the map. The player can then enter that area and then you get transported to a new screen where you find a randomly generated city with roads and houses. The player can walk around in the city and also enter the houses. When the player enters a house, you get transported to a new screen with a randomly generated house, which contains walls and chests which contains resources. The player can exit the house which returns them to the city. If the player goes to the exit of the city, they get transported back to the map, which now has the neighboring areas unlocked because they have explored the connected city. Also everytime the player enters a city, it costs them resources so the player has to find enough resources in the cities and houses so they can travel.

### The Map

The map is generated using a couple of techniques. Firstly, a set of nodes are randomly but evenly distributed using Poisson Disc Sampling. The technique creates a uniform distribution and also makes the nodes have a minimum spacing. After that there are edges created between the nodes. These where originally planned out to be created using Delaunay Triangulation but I spent a lot of time without getting any results. I instead decided to create my own algorithm which starts off by connecting all the arbitrary "close" nodes together. Then it randomly removes edges that intersect eachother. This works ok but has a lot of flaws so it is not recommended to use. I also create a background for the map which maps the nodes into regions that looks like a map with countries. This is just a Voronoi Diagram created from the nodes. It is also created very inefficiently by looping over each pixel and finding the closest node to that pixel. This has horrendous time complexity but because it is not performance critical it works.

### The City

The city was the first feature I implemented. It is created using Wave Function Collapse as described in [this github repo.](https://github.com/mxgmn/WaveFunctionCollapse) Specifically the Tilemap generation function is what I am using. It creates a map of tiles based on the initial tilemap and each tile having a ruleset of how it can be connected to other tiles. The function then picks the tile with the least amount of options as to what tile can be placed there (this value is called entropy) and picks one of these available tiles randomly. It the calculates the new entropy for all tiles and finds the new tile with the lowest entropy. This continues until it cannot place any more tiles. I have modified the algorithm to suit my use case. First of all I skew the probability of which tile should be placed. I make sure that the chance of placing a house tile increases the more you place road tiles and vice versa. This makes it so I can control how big I want the city depending on how probable it is to place a house rather than a road. I also set it so that it only places tiles that are connecting to other tiles. That way it doesn't create any loops or neighbouring cities that the player cannot access. I only want to create one city.

## The house

The house generation I created myself using some inspiration from other techniques such as Binary Space Partitioning. It first of all places the door to the house on the edge which matches with the entrance in the city map. Then it creates a random split either vertically or horizontally and creates a wall there with a single opening. Then it goes through each floor tile and it has a 2% chance to place a chest per tile with a maximum of 3 chests.