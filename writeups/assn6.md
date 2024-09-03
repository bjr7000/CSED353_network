Assignment 6 Writeup
=============

My name: Bae Jaeryun

My POVIS ID: bjr7000

My student ID (numeric): 20220196

This assignment took me about 2 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the Router:
	Adding to routing table function just pushes data about routing to
routing rule vector. The data is implemented with struct type.
	One datagram function checks the remaining life of the recieved
datagram. If its life is 1 or below, the function drops it. If its life is okay,
the function matches the datagram with routing rule table. If no matching is
found, the function ignores the datagram. If there are two or more matchings,
choose longer matched one. Matching length is calculated with prefix length in
the routing rule.
	After matching, it makes sending datagram with next hop in the rule,
and pushes the sending datagram to interface with interface number in the rule.	

Implementation Challenges:
	The program successfully works in the first try. Just reading standard
documents and finding some words in English dictionary were the only challenges.

Remaining Bugs:
	No new bug.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
