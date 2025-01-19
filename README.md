# Airplane-sim
Used Semaphores to simulate air trafic control


Author: Tyler Ward<br/>
Date: November 14th, 2024<br/>
Operating System: Mac OS<br/>
Special steps to compile: make sure to compile proj2.cpp and AirportAnimator.cpp with -lcurses<br/>
Special details on how to run: 1st argument is the number of passengers 2nd the number of tours<br/>
Bugs: None that I know of<br/>
Overview of project: I started just by messing around with the animations to get familiar with the AirportAnimator 
class. I then created 8 threads, 1 for each airplane. Then I started working on a while loop that would run until the desired amount of tours were reached.
I accomplished this but all the airplanes were running into each other. Then I implemented semaphores. However,
they didn't work as desired. I accidentally created 4 semaphore sets with 4 in each set (kinda). I got help 
from lab help hours and got the semaphores sorted out. <br/>



Run with 9 passengers: deadlock this isn't even enough passengers for 1 plane to take off even if they all
managed to somehow get on the same plane.  <br/>
Run with 10 passengers: deadlock because even if you got fortunate the plane would take off once and Then
the passengers would distribute to the other planes and cause deadlock. However, the chances of it even taking off
in the first place is nearly impossible because all the planes are trying to grab passengers. 
Run with 42 passengers and 100 tours: deadlock not a single plane takes off because none of the planes are 
able to get to 10 passengers before the passengers run out. <br/>
Run with 79 passengers and 20 tours: Took on average 5 mins and was always 28 total tours
Minimum number of passengers to prevent deadlock: 73 technically is the bare minimum to avoid
all deadlock completely. This is because if you only had 72 technically all 8 planes could get 9 passengers and cause 
deadlock. <br/>
