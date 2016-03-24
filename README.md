# Picture Frame

## Premise
So, with the rise of low cost ARM boards which have good graphical capability, specifically to decode video playback, my goal was to create my own Qt5 application which is able to show me pictures, playback videos, and provide an easy network based upload mechanism. I know you can buy these, but the resolution is terrible, and generally they are expensive or look/work terrible.

## Design
So, you start with Qt5, and make it show images. It needs to be compatible with Qt5 to make the best use of gstreamer and QtMultimedia. Then, you add video playback, and finally, add in a web upload capacity. Make sure you include instructions for how to get it running on an RPI, and away you go.

The design breaks down into a few differen classes to handle the Qt work, and a configuration file for defining some runtime values. To make sure you don't need to do anything special, make it possible to recognize changes to the filesystem so that new images or videos can be added to the rotation as soon as they are uploaded.

## Install
TODO

## Usage
TODO

