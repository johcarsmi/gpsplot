# gpsPlot

This is a small project which I use to plot data from a Garmin 
Oregon 450.

### Porting to Qt5
The Qt object references were make specifically qt5 as initial
attempts caused the headers for Qt4 to be installed. (Since removed).
##### QWT Issues
The qwt6 library in the openSUSE repositories is compiled against 
Qt4 libraries and does not work with Qt5. The program fails to load.
To overcome the problem I downloaded the qwt source from SourceForge 
and compiled it in QtCreator (Qt5 of course - release process) and 
issued a 'sudo make install' in the compile directory.
In order to get the widgets appearing in QtDesigner I created a link
from the installed library to the Qt5 plugins directory with:-
>ln -s /usr/local/qwt-6.1.3/plugins/designer/libqwt_designer_plugin.so /usr/lib64/qt5/plugins/designer/
##### Project file changes required
The INCLUDEPATH value needed to be changed to point to the new location 
and the LIBS value to the new libqwt.so .

### Description
It takes a .gpx file and imports it and then allows the choice
of graphs that can be obtained from the data.

### Dependencies
* It was written in __C++__ using the **Qt4** Framework, now ported
to **Qt5**.
* It uses the __QwtPlot__ object from **QWT6** to present the graphs.

### Status
It is a perpetual work in progress! Having said that I have no immediate
plans to add any functionality at present (February 2015).
One idea I may have a go at is using the mouse to drag the plot-on-satellite 
to pan it. This was added in February 2016 with mouse wheel zoom.

### Credits
The filedownloader code was taken straight from the Qt documentation site.
The relevant document may be found [here] (https://wiki.qt.io/Download_Data_from_URL).

### Licence
Lesser GPL. 

### Author
John Carrick Smith 
