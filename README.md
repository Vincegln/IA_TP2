# IA_TP2

This repository is a bout the second assignment we had to complete in our "Artificial Intelligence for Video Games" course.

### Calculation methods

The provided calculation methods are not all functionnal.

**Weighted Sum** works fine for all parts of the project.

**Prioritized** doesn't work above a *maximum speed* of 40. Beyond that value, the leader moves too much for the followers to be able to cope with all its movement variations.

**Dithered** doesn't have the *OnPursuitOffset* function implemented, which is the basic function used in the early questions of this assignment.

### Report for AI TP2

**Question 1** : We create a FollowAgent Class and thanks to the steering behavior, we created the movement that was asking in the question.

**Question 2** : We implemented a LeaderAgent Class and add the steering behavior Wander.

**Question 3** : For this question, we imagine that if we introduce a new Leader to the window, the first FollowAgent will follow the closest Leader Agent at each time.
So, if we introduced a second Leader, the first FollowAgent will alternate between those 2 Leader.

**Question 4** : In this case, we add an Agent which is controlled by us. We can see that all FollowAgents follow our Agent without any problems.

**Question 5** : All the features implemented in this TP can be changed in the sub-menu. 