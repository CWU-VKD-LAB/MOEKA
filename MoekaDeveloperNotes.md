# Contacts:
## Austin Snyder:
- School email: austin.snyder@cwu.edu
- Personal email: austin.w.snyder@outlook.com
- LinkedIn: linkedin.com/in/austinsnyder411
- Discord: mxstic.

## Ryan Gallagher:
- School Email: ryan.gallagher@cwu.edu
- Personal Email: rygall75@gmail.com
- LinkedIn: linkedin.com/in/ryan-gallagher-0b2095285
- Discord: gally3338915

# MOEKA Future Work

We worked on mainly creating an ML interface to classify larger datasets.  
- A quick and "easy" feature to add would be to have a Subject Matter Expert be able to re-classify the same data which the ML just did.  
  - A comparison mode  
- Another few interesting features, run ML, then interactively, make it so that an SME can go back through and change some points.  
  - This of course means the points must then update their neighbors if they need.  
- It would be necessary to make some kind of side by side view interface for a user.
- When generating Hansel Chains, it may be much faster to simply make all points [0,0,0,0], [0,0,0,1] ... and put each into a map. Since we know exactly which points are going to be the expansions, we don't really need to mark them. Since we know +-1 in each attribute is an expansion each time. It will save a lot of time computing expansions, we just go mark them at interview time instead. We could just look up for example the points with each digit value +-1 for each attribute from the one which we just asked.
> **Note:** To run the ML, you need to have Python installed, and have your firewall off (or turn it off if asked). This is because the python communicates through a client server connection.

# Current Bottlenecks
- Right now, the program is able to run dimension 9, k value 3 reasonably quickly. The rendering is slow, and there is a lot of slow copying operations which are unnecessary. 
- Calculating the hansel chains is quite slow right now, and finding expansions can be faster. a simple fix would be throw some parallelism at this with a simple few well placed #pragma omp parallel. 
- Right now, we also eat memory a TON. Way more than we should be. There is some kind of memory leak or unnecessary copying going on in the computation of hansel chains, or calculateAllPossibleExpansions. 
- The ML server-client thing is pretty slow. It may be important to eventually port the Python to run in C++ somehow, or make it callable as a DLL in some way. Somehow making that run in parallel would be interesting as well. This is a very slow portion of the program at the moment.

- The rendering has some sort of bottleneck with really large datasets. For example rendering 9 dimensional 3 k values, it computed all chains after a while, but never renders. It just hangs.
- Right now there is a good deal of copying happening before we render, that is something we can investigate in Form.cpp. That is most likely not the entire issue.
- Find a more efficient way of determining whether to ask a question. Right now, we go to all those points, and just check if we need to ask a question, but maybe if we kept a queue somehow of questions to ask, we could skip all those checks.  


- When calculating hansel chains, it would be much faster to store a "point" struct, or "dvector" in some kind of big map. Since we know exactly which points expand to which at interview time, it would be way faster to look them up at interview time. then we don't need the calculate all possible expansions functions. We would just look up points at runtime. Just one entry in the map per point.
## Quick Fixes

- In rendered outputs, include class names, and attribute names. (`Window.cpp` hovering functionality is what you want to investigate).  
- In rendered outputs, set the `.color` attribute of each `Bar` object automatically, rather than the janky color picker thing we have to use now. A random color picker type thing for each class would be nice.  
- Implement a frame cap on the rendering. Right now it runs as hard as possible, which is kind of ridiculous since it is just rectangles on a screen.  
- Get moving the model with arrow keys to work. (Right now they can cause runtime exceptions. This is the `keyCallback`s in `Window.cpp` that you want to play with, some kind of translation moving it around and scaling.)  
- Get the rectangles to render properly once and for all. Right now you have to slightly play with the scaling to get them to stop overlapping each other. It can be done with the zooming buttons, but should be rendering properly the first time. Could be an issue with float vs ints for the actual pixel locations?  
- Look into why rendering is so heavy while not moving. Make it so that we don't re-render boxes if they aren't changing. Zooming into certain areas would be important as well.  

- Right now, as you can see in the demo video, there are some things which do not behave properly when you run an interview/ML model for a second time. This is most likely an issue with attributeNames / k values of the moeka object.
- Demo video link: https://youtu.be/AwgxQ8XcNI8
- If you want to use more datasets, they go into Moeka/Moeka/datasets/. That is the path which the drop down menu is looking for.
---

### To change K value for machine learning
- Right now you go into the `ML_Oracles`, and in `makeMLModels.py`, you can just change `MAX_K_VAL` to whatever.
