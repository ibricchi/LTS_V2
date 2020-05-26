# Some useful links regarding conductance matrices 

## “SPICE algorithms and internals” by Paul D. Mitcheson
Basic/easy to understand introduction how SPICE works through conduction matrices and Newton-Raphson method.  
<http://www3.imperial.ac.uk/pls/portallive/docs/1/56133736.PDF>

---

## “In a Nutshell: How SPICE Works”
Very good reference that tells you quickly how SPICE does what. This can then be used to look it up in more detail somewhere else.  
<http://www.emcs.org/acstrial/newsletters/summer09/HowSpiceWorks.pdf>

---

## An Algorithm for Modified Nodal Analysis (MNA)
A usable (little chaotic) explanation of how MNA works and how to obtain it algorithmically. MNA is used by SPICE as it can deal with voltage sources. Voltage sources cause a problem with conventional nodal analysis due to infinite conductance. 
Also includes reactive components (explanation even more chaotic…)  
<https://lpsa.swarthmore.edu/Systems/Electrical/mna/MNA1.html>

---

## HELM Math by Nucinkis (From page 39 – actual pdf page = 41)
Easy to understand mathematical explanation of Newton-Raphson method
(For quick circuit analysis application example, see “SPICE algorithms and internals” above)  
<http://nucinkis-lab.cc.ic.ac.uk/HELM/HELM_Workbooks_11-15/WB12-all.pdf>

---

## Spice source File Breakdown
Explains how source files work for spice files  
<https://www.cpp.edu/~prnelson/courses/ece220/220-spice-notes.pdf>

---

## Techniques for circuit simulation
Tons of information about circuit simulation in lecture slide format. Start reading from pdf page 37 (Or skip further as MNA is explained better in link above). Also ignore STA analysis as it is less efficient than MNA.
Goes through topics such as Newton-Raphson method for non-linear components and transient analysis with capacitors.  
<https://www.ee.iitb.ac.in/~sequel/circuit_sim_1.pdf>

---

## Transient Analysis (eCircuit)
Very basic and simply to understand explanation how to use backward-Euler and MNA to perform transient analysis. This avoids the having to solve differential equations and is similar to how SPICE does it (probably a little simplified).
The results look very promising (comparison with LTSpice transient simulation). A screenshot of the comparison is provided in the research folder.  
<http://www.ecircuitcenter.com/SpiceTopics/Transient%20Analysis/Transient%20Analysis.htm>

---

## A Study of Numerical Integration Techniques for use in the Companion Circuit Method of Transient Circuit Analysis
Harder to understand but more detailed than eCircuit page above. Also seems like a good source to use.
Includes companion circuit models produced by different integration techniques. Also gives a detailed comparison of integration techniques (Will need to justify why we choose a particular one).
Page 27 (pdf page 37) gives the backward-Euler companion circuit for an inductor (eCircuit only gave the one for a capacitor). On this page, there are also all kinds of other companion circuits.  
<https://docs.lib.purdue.edu/cgi/viewcontent.cgi?article=1301&context=ecetr>

---

## Inside Spice
A good book that explains methods used in SPICE. Especially useful for Newton-Raphson with non-linear components and for dynamic time step control (could be added in end if want greater accuracy while being fast).  
<https://idoc.pub/download/kielkowski-inside-spice-d2nv6ogoxy4k>

---

## Nonlinear Device Stamping 
Lecture slides that explore companion models for nonlinear devices, how to express stem in matrix form and how to use Newton-Raphson to solve them.   
<http://202.120.46.7/msda/data/courseware/SPICE/lect08_nonlinear_stamp.pdf>

---

## Companion Models for Basic Non-Linear and Transient Devices
Very short and easy to understand document that gives different companion models. Included nonlinear companion models = Diode, MOSFET (n-channel and p-channel), BJT (NPN and PNP).
Also includes a good image outlining the general algorithm for nonlinear transient analysis.  
<http://dev.hypertriton.com/edacious/trunk/doc/lec.pdf>

---

## The Newton-Raphson method *(applied to diodes)*
A good description of the Newton-Raphson method in detail. Applied to simple polynomials. Applied to diodes (includes derivation of companion model). Used in matrix form for multiple nonlinear components. And more.  
<https://matel.p.lodz.pl/wee/i12zet/The%20Newton-Raphson%20method.pdf>

---

## Appendix A - The Newton-Raphson algorithm *(for computer implementation)*
A less good description than the link just above. However, it gives a suggested pseudo code algorithm *(Figure A.2)* that can be implemented on a computer and is specifically written to show how Newton-Raphson can be used in a computer algorithm for. Probably best to start with the link above and the use this link as a guidance of how to write the actual algorithm.  
Only Appendix A is interesting => Ignore the rest.  
<https://link.springer.com/content/pdf/bbm%3A978-1-4612-3486-9%2F1.pdf>

---

## Ebers-moore BJT model links
Honestly wikipedia had a lot of good resources, but you know, it's wikipedia so here are a few links, including the wikipedia ones.
<https://en.wikipedia.org/wiki/Bipolar_junction_transistor>
This presentation has a good break down of what each part of the ebers moore model does
<https://kpandiaraj.files.wordpress.com/2017/05/ebers-moll-model-ppt-compatibility-mode.pdf>  

---

## Gummel-Poon BJT model model links
This model would be nicer to implement since it is more complete, however it's qutie a bit more complex and requires more time spent uderstanding. I don't think the benefits would outweigh the time spent
This link has a basic breakdown of the equations
<https://uk.mathworks.com/help/physmod/sps/ref/npnbipolartransistor.html>  
This link is the best resource I could find, it has a break down of everything to do with this model, and explains the basic and augmented versions of it, aswell as how to obtain everything necessary to model it. It is very complex however.
<http://www.idea2ic.com/PlayWithSpice/pdf/G%20U%20M%20M%20E%20L%20-%20P%20O%20O%20N.pdf>  

---

## SPICE2: A computer program to simulate semiconductor circuits
Written by the original developer of SPICE. It contains lots of detail but is a little outdated.  
<https://www2.eecs.berkeley.edu/Pubs/TechRpts/1975/ERL-m-520.pdf>

---

## Newton Raphson for DC circuits
Break down of how Newton raphson is used to derrive equivalent circuits and, then how to use those circuits in calculations
<http://qucs.sourceforge.net/tech/node16.html>

---

## Stamping "this is basically the method we're using"
Break down of how to use stamping to make the matrixes we're using to solve non-linear circuits
<https://courses.grainger.illinois.edu/ece552/fa2017/ECE552-F17-Chapter6.pdf>