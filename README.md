# IA_TP2

This repository is a bout the second assignment we had to complete in our "Artificial Intelligence for Video Games" course.

### Calculation methods

The provided calculation methods are not all functionnal.

**Weighted Sum** works fine for all parts of the project.

**Prioritized** doesn't work above a *maximum speed* of 40. Beyond that value, the leader moves too much for the followers to be able to cope with all its movement variations.

**Dithered** doesn't have the *OnPursuitOffset* function implemented, which is the basic function used in the early questions of this assignment.

**Additional info** : Sometimes, one agent might pursue the leader over the edge, instead of stopping before it. We determined that it happens because of the speed setting, inducing the fact that the agent doesn't have enough time to slow down. However, we decided not to decrease the speed of the agents in order to keep some dynamism.

### Report for AI TP2

**Question 1** : We created a FollowAgent Class and thanks to the steering behavior, we obtained the movement that was required.

**Question 2** : We implemented a LeaderAgent Class and added the steering behavior Wander.

**Question 3** : For this question, we imagined that if we introduce a new Leader to the window, the first FollowAgent will follow the closest Leader Agent at each time.
So, if we introduce a second Leader, the first FollowAgent will alternate between those 2 Leader.

**Question 4** : In this case, we added an Agent which is controlled by us. We can see that all FollowAgents follow our Agent without any problems.

**Question 5** : All the features implemented in this TP can be changed in the sub-menus. 