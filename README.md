# The calibration of absolute judgments

This repository contains data from my unpublished Masters thesis. The experiments were designed and run in 2002-2003.

All files in this repository that can be licensed are licensed CC-BY 2.0 (https://creativecommons.org/licenses/by/2.0/).

Directory  |  Contents
-----------|--------------
analysis   | Description of the methods and some preliminary analyses that I redid (in 2017, 14 years after the experiments)
data       | Data files and description
experimental_code | The C code that ran the experiments. You likely won't be able to compile these, but they contain critical information about the methods. If for some reason you'd like to compile the code, contact Jeff Rouder (rouderj@missouri.edu) for the libraries
info       | A copy of the thesis (sorry about the terrible writing; it was my first scientific effort) and other information, including figures I extracted from the text for convenience


### The basic idea

In an absolute judgment experiment, participants are presented with a number of stimuli which each have specific "names". Often --- as in these experiments --- the stimuli only vary on a single dimension (e.g., pitch, loudness, weight, length). The stimuli in this experiment are all line lengths displayed horizontally on the screen, randomly, one at a time. The participant's task is to identify which line has just been presented with its name. The names given to these lines are the ordinal lengths (1-12) of the lines in the twelve-line set. The shortest line is "1", the longest line is "12".

Although such a task would be trivial to perform by a computer, which can measure the line exactly, it is quite difficult for humans to perform to a high level of performance. [Miller (1956)](http://psychclassics.yorku.ca/Miller/) famously discussed the "magical number" seven items, which seems to be the limit of performance in absolute judgment tasks. With more stimuli, accuracy drops precipitously, keeping the average performance in terms of mutual information between stimulus and response at about 2.5 bits, on average (although [some](http://link.springer.com/article/10.3758/BF03196725) have extraordinary abilities in this task, reaching above 4.3 bits).

There are two related questions about unidimensional absolute judgment:
1. How do humans perform the task, and
2. Why are they so bad at it?

The two experiments in my Masters thesis, whose data are presented here, are meant to shed some light on the question. They seemed to rule out several classes of model: simple "calibration" models, in people perform the task by estimating the magnitude of the stimulus based on the the current (and perhaps the previous) one, and simple memory models, where participants have "fuzzy" representations of the stimuli.

The key to the experiments was testing performance on several key, constant stimuli while changing (or not changing) other stimuli after the they have been learned. Some changes --- those that bring new stimuli "closer" to the constant ones --- seem to disrupt performance identifying the constant stimuli, *in spite of the fact* that they are rarely confused with the new stimuli.

Why this occurs is not known (at least, I don't believe it is). Although the question is interesting, I abandoned the line of research due to a growing interest in more quantitative psychology.


