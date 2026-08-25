Repository for code to play with DSP functions on LGT8F328P using the DSC module or not.

All pass now in the DSC tests with new patched assembly file. C Calls are in the header and axample calls are in 
the ino file.
Remember to reset before any cascading if the preceding entries are to be discarded. The test examples show how and 
the manual explains why (DA is preserved in many functions).
