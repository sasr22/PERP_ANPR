# PLPD ANPR

## Description

Using [OBS rtspserver](https://obsproject.com/forum/resources/obs-rtspserver.1037/) you can capture text and hopefully license plates in [perpheads.com](https://perpheads.com/).

Using OpenCV2, [this pseudo code](https://stackoverflow.com/a/37523538), and this [code to count the white pixels](https://stackoverflow.com/a/53584482) I created a program that detects text 
in video feed.

The code is meant to work with any RTSP feed running on ```rtsp://localhost:9768/live```.

The program will start a window to show what it sees and all hits will be in the ```img``` folder and under the name(s) ```hit_HITNUMBER.png```, all in png format.

## How to build

To build the code use [c++ complier](https://jmeubank.github.io/tdm-gcc/download/) and run the following command:

```g++.exe -Iinclude libopencv_core453.dll libopencv_highgui453.dll libopencv_imgcodecs453.dll libopencv_imgproc453.dll libopencv_videoio453.dll opencv_videoio_ffmpeg453_64.dll -g ANPR.cpp -o ANPR.exe```

To build ```ANPR.exe```, make sure to have all DLLs provided in the project in the same folder as the ```ANPR.exe``` file.