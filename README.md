# c3spit
C3S Playlist Inspection Tool

This software demonstrates how we envision the playlist monitoring in clubs using a blackbox. It records audio sgnals from line-in, stores it in a wav file, creates a fingerprint from the wav, uses the fingerprint to query our Echoprint server to see if there is a match in the database (which holds a selection of over 100.000 tracks for testing purposes currently).

First, clone this git repo:

git clone https://github.com/C3S/c3spit.git

## Prerequisites

sudo apt-get install gcc
sudo apt-get install libasound2-dev 
sudo apt-get install libboost1.55-dev libtag1-dev zlib1g-dev
sudo apt-get install gnustep-gui-runtime

create a subdirectory for the echoprint-codegen:

## Build 

gcc -o c3spit c3spit.c -lasound

## Prepare audio input

First, got to your sound settings and make sure the signal level is not to high because distorted music won't be recognized. Sometimes the level is just too high to be normalized undistorted by the audio input mixer. In this case, try a device with an earphone socket, that won't switch off your main audio out when a cable is plugged in (otherwise you would need a Y-cable). Often the output level of the headphone can be adjusted separately.

If you have more than one sound device in your system, you might need to adapt the soundcard parameter (hw) in the shell script c3spit.sh, line "./c3spit.sh hw:0". 

## Run the demo

Finally, just start the script:

./c3spit.sh
