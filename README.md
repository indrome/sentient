Sentient
========

Voice recognition for performing detection and recognition

About
=====

This is still considered a non-official release.

Installation
============

In the top-level directory, type ``make``.


Overview
========

* feature_extractor - Includes a library that perform Mel-Frequency Cepstral Coefficient extraction from raw normalized speeech. 
* recognizer - Classifiers for signal processing. Currently, the only classifier is Dynamic Time Warping.
* server - Setup a socket server which can receive JSON-formatted speech.    
* bin - Symlinks to various 
* reference - Location of stored features.
* scripts - Location of bash-scripts which bares the same name as the their assoiated features in referece/ 

Example: Running the server
===========================

```$: ./bin/server 192.168.0.100 54321 
```

When receiving, the server store the speech signal intermittently in a file in the top-level directory called sample.wav. 
After processing sample.wav the feature extractor stores a file in the top-level directory called features.json which may 
be saved to reference/ and given a proper name. Upon a successful lookup into reference, the classifier will try to run a 
bash script that has the same name in the scripts directory.

Note
====

* Port is fixed in source to 54321 
* A high-pass filter is recommended for optimal training.
