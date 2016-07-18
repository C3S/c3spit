# c3spit
C3S Playlist Inspection Tool

This software demonstrates how we envision the playlist monitoring in clubs using a blackbox. It records audio sgnals from line-in, stores it in a wav file, creates a fingerprint from the wav, uses the fingerprint to query our Echoprint server to see if there is a match in the database (which holds a selection of over 100.000 tracks for testing purposes currently).

If you have an Ubuntu 14.04 installed, you might want to take a shortcut and install just the binaries, so you can skip the Build step:

https://files.c3s.cc/c3spit-ubuntu14.04-64bin.tgz (64 bit build)
https://files.c3s.cc/c3spit-ubuntu14.04-32bin.tgz (32 bit build)

Otherwise, clone this git repo and enter it:

git clone https://github.com/C3S/c3spit.git
cd c3spit

## Prerequisites

sudo apt-get install gcc
sudo apt-get install libasound2-dev 
sudo apt-get install libboost1.55-dev libtag1-dev zlib1g-dev
sudo apt-get install gnustep-gui-runtime jq

clone the echoprint-codegen in a subdirectory of c3spit:
git clone https://github.com/spotify/echoprint-codegen.git

## Build 

gcc -o c3spit c3spit.c -lasound

also build the codegen:

cd echoprint-codegen/src
make
cd ../..

Check the recorder with a test run:

./c3spit hw:0

## Prepare audio input

First, got to your sound settings and make sure the signal level is not to high because distorted music won't be recognized. Sometimes the level is just too high to be normalized undistorted by the audio input mixer. In this case, try a device with an earphone socket, that won't switch off your main audio out when a cable is plugged in (otherwise you would need a Y-cable). Often the output level of the headphone can be adjusted separately. Try low values first 10% or -20dB.

If you have more than one sound device in your system, you might need to adapt the soundcard parameter (hw) in the shell script c3spit.sh, line "./c3spit.sh hw:0". 

## Run the demo

Finally, just start the script:

./c3spit.sh

Try some older music first for more hits. If you don't get music recognized after playing some tracks and to ultimately be certain about the audio quality, playback the recorded samples.wav after a test start of ./c3spit.sh. 

Have fun!
