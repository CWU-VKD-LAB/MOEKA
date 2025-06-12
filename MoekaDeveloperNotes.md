# MOEKA Future Work

We worked on mainly creating an ML interface to classify larger datasets.  
- A quick and "easy" feature to add would be to have a Subject Matter Expert be able to re-classify the same data which the ML just did.  
  - A comparison mode  
- Another few interesting features, run ML, then interactively, make it so that an SME can go back through and change some points.  
  - This of course means the points must then update their neighbors if they need.  
- It would be necessary to make some kind of side by side view interface for a user.

> **Note:** To run the ML, you need to have Python installed, and have your firewall off (or turn it off if asked). This is because the python communicates through a client server connection.

# Current Bottlenecks
- Right now, the program is able to run dimension 9, k value 3 reasonably quickly. The rendering is slow, and there is a lot of slow copying operations which are unnecessary. 
- Calculating the hansel chains is quite slow right now, and finding expansions can be faster. a simple fix would be throw some parallelism at this with a simple few well placed #pragma omp parallel. 
- Right now, we also eat memory a TON. Way more than we should be. There is some kind of memory leak or unnecessary copying going on in the computation of hansel chains, or calculateAllPossibleExpansions. 
- The ML server-client thing is pretty slow. It may be important to eventually port the Python to run in C++ somehow, or make it callable as a DLL in some way. Somehow making that run in parallel would be interesting as well. This is a very slow portion of the program at the moment.

## Quick Fixes

- In rendered outputs, include class names, and attribute names. (`Window.cpp` hovering functionality is what you want to investigate).  
- In rendered outputs, set the `.color` attribute of each `Bar` object automatically. A random color picker type thing for each class would be nice.  
- Implement a frame cap on the rendering. Right now it runs as hard as possible, which is kind of ridiculous since it is just rectangles on a screen.  
- Get moving the model with arrow keys to work. (Right now they can cause runtime exceptions. This is the `keyCallback`s in `Window.cpp` that you want to play with, some kind of translation moving it around and scaling.)  
- Get the rectangles to render properly once and for all. Right now you have to slightly play with the scaling to get them to stop overlapping each other. It can be done with the zooming buttons, but should be rendering properly the first time. Could be an issue with float vs ints for the actual pixel locations?  
- Find a more efficient way of determining whether to ask a question. Right now, we go to all those points, and just check if we need to ask a question, but maybe if we kept a queue somehow of questions to ask, we could skip all those checks.  
- Look into why rendering is so heavy while not moving. Make it so that we don't re-render boxes if they aren't changing. Zooming into certain areas would be important as well.  

---

### To change K value for machine learning
- Right now you go into the `ML_Oracles`, and in `makeMLModels.py`, you can just change `MAX_K_VAL` to whatever.
