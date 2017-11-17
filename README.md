# Rasberry PI Picture Frame

## Premise
The idea is that lots of low cost digital picture frames exist in the world today. Some are
even connected. But they lack in a lot of places specific to what I want. So, I built my own.
This is based on the Pi 3 or Pi Zero W, uses Qt5, and must be network connected. I have
some PHP server code that sits on my little web server which allows me to upload pictures
to the server, and then the Pi will go ask the server every hour for those pictures. It
allows for people to share pictures across different displays by simply logging in and
uploading images you want the other person to see. Granted, there are a lot of abuses
that can be done here, but honestly, it's just for me and my wife.

## Dependencies

```bash
sudo apt update
sudo apt install qt5-default qt5-qmake
sudo apt install qtmultimedia5-dev libqt5multimediawidgets5 libqt5multimedia5-plugins libqt5multimedia5
git clone git@github.com:buelowp/pictureframe.git
cd pictureframe
qmake
make
mkdir -p ~/.config/Home
cp PictureViewer.ini ~/.config/Home
```

Copy the binary pictureframe to /usr/bin, then edit your startup script to run at boot.

Finally, edit PictureViewer.ini to point to the right Images folder on your pi, update the URL to
download the contentlist.xml file from the right server location, and change the Network parameter
to point to the correct network interface you use for getting onto the internet. Mostly this will
be wlan0, but may be eth0 if you are wired.

# Future plans
I'd like it to play back videos as well, but so far, that hasn't worked very well.

