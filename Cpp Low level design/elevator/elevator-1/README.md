
https://github.com/hannahvoelker/Elevator

# Elevator
Basic C++ Elevator Implementation (four hour time limit)

# Approach
I created two classes-- the elevator class, which provides all of the functions for a single elevator, as well as a "lobby" class, which acts as sort of a wrapper that handles the delegations to all of the elevators. 

### Elevator Class:
Aside from the typical construction/destructor as well as getting and setting functions, the  elevator class contains a few important variables. Namely, I used an enum to specify the direction of the elevator, and a set to contain all of the floors the elevator needs to stop at. I chose a set since it fit the bill for what I needed - it is ordered, and floors can only be in the set once, so if a floor is chosen more than once, it does not matter. 

### Lobby Class
The lobby class utilizes a few data structures. The elevators for that building are stored in a vector, the requests to be processed are stored in a queue. The more interesting functions come down to how a request is processed. When a request is processed, it is removed from the queue, and by looping through the elevators vector, the request is sent to a specific elevator. The request is delegated based on the location of the elevators and their direction. If none of the elevators are able to take the passenger (eg, if they are all going up, and the user wants to go down), the request is re-enqueued. This allows for the invariant of real elevators -- if it is going up, it will keep going up until no more buttons on higher floors have been pushed, and vice versa.  
There are some flaws in this approach -- this algorithim runs in O(n^2), due to the two while loops, however, since we max at 16 elevators, this isn't as important. Another flaw I noticed is that storing the elevators in a vector and looping through it is a bit problematic, as the later elevators may not be used as often, thus making our approach potentially inefficient. If I had a bit more time, I would use a heap instead of a vector, sorting the elevators by their current floor, in order to better process a request. I also made an elevator "Idle" once it was done with a request, which might not be the most energy-efficient approach-- eg, an elevator that was once going up, could start taking passengers down from that floor. The way the requests are processed doesn't account for this, and if I had more time, I would delegate idle elevators based on which one was closest to the request.

# Things to Consider / What I would expand on
Right now, my implementation is somewhat restricted. Many elevators have floors that are not numeric, like "G", "L", or "1A". I might process this by aligning an input with an index in an array of buttons, instead of directly using an integer to be put into a set. This would also involve a more involved constructor, since you would have to input an array of buttons when creating an elevator. Another thing I would like to expand on is the fact that every elevator is limited to the same number of floors.  Again, I would have to process and add each elevator one by one, and ensure that the corresponding number to a button in the set that each elevator uses aligns between every elevator. For example, if one elevator has a "G" button that is lower than the main floor, the lobby class would have to ensure that every elevator's lobby button corresponded to floor 1, if G was floor 0. Finally, I would like to improve how my program runs. Requests are handled, processed, and delegated one by one with the way main.cpp is written, so I would like to rewrite this in a way that multiple requests are being enqueued and handled at once, since this is how real elevators work. 

Overall, this was a fun challenge since I hadn't written a large C++ program since the summer, and got me thinking about many different things. 
