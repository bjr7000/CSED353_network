Assignment 1 Writeup
=============

My name: Bae Jaeryun

My POVIS ID: bjr7000

My student ID (numeric): 20220196

This assignment took me about 8 hours to do (including the time on studying, designing, and writing the code).

Program Structure and Design of the StreamReassembler:
  Variable passedIndex indicates the length of the assembled string.
  To remove string that is already in assembled stream, the string with index
before passedIndex is cut off.
  To remove string that exceeds capacity, the string with index after 
passedIndex+capacity is cut off.
  StreamReassembler doesn't need to cut off string that is already in its buffer
, because I used string.replace function to put the string in, which 
automatically overlaps original string.
  To indicates which character in string is arrived or not, a string variable
named validStr has same length with the buffer string variable.
  If a character arrived, the position of the char in validStr becomes '1', 
else, it becomes '0'.
  To put correctly assembled string to output stream, check if validStr[len] is
'1' from the front, and put the buffer's character with corresponing position, 
len, to output stream.
  If validStr[len] is '0', the loop breaks.
  To prevent the program refering to beyond valid range of buffer string, if len
equals to size of buffer string, the loop breaks.
  To prevent the program writing output stream over its capacity, if len is 
bigger than remaining capacity, len is remaining capacity of output stream.
  For next data input, erase validStr and buffer string from 0 to len and 
passedIndex+=len.
  For eof notification, if eof is true, eofIndex is index of end of given data.
  If all buffer is used so it is empty and passedIndex equals to eofIndex, eof 
function of output stream is excuted.

Implementation Challenges:
  It was hard to find bug in test 'many' and 'win', random string tests. In 
other tests, there is no string with NULL character ('\0'), so it took some time
to find out NULL character makes the bug.
  Also, the tests are randomly generated so I had to several checks to sure my 
code operates well.

Remaining Bugs:
  In test address, timeout rarely happens.
  Its usual excution time is 1-2 seconds, but it sometimes becomes 5-6 seconds, 
and rarely, it exceeds 10 seconds, which leads to timeout failure.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: My code is perfect because there are some random tests.
