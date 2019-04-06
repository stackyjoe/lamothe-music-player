# lamothe-music-player
LaMothe is a music player that strives for simplicity, modularity, and clean code.

#### LaMothe is built with QtCreator, and currently uses the QMake build system.

Once the UI is stabilized I might look into using a CMake or a POSIX Make build system, to minimize build dependencies. That's low priority though, and I'm not certain how much the dependency on Qt's moc and uic can be reduced.

#### LaMothe organizes music into library files, which location and metadata.

This is currently implemented in a hacky manner, but I plan to change the code to store it as plain JSON soon.

#### The actual code handling the audio and audio metadata is handled through a [pimpl](https://en.cppreference.com/w/cpp/language/pimpl) interface, making it easy to add alternative backends without changing the front end.

Currently the audio interface supports [SFML](https://github.com/SFML/SFML) and [Audiere](http://audiere.sourceforge.net/). I plan to add SDL2 as an alternative soon.

Currently, the metadata interface only supports [taglib](https://github.com/taglib/taglib). I plan to add id3lib as an alternative soon.

#### LaMothe does not handle compact disks, nor does it handle RSS feeds.

I will probably not spend time trying to implement those things in LaMothe.

   1. Part of it is my belief in the **UNIX philosophy**, which says in part that tools should focus on doing one thing well;
   2. part of it is that I think adding a tab feature makes the program look a lot worse (and perhaps there are smarter ways to add those features);
   3. part of it is that I am making this for me to use on my FreeBSD machine, and cdcontrol is perfectly adequate for playing compact disks;
   4. part of it is that I am working on another standalone tool for podcast subscription and listening.
   
So it's possible that I will add those features down the road, but I won't working on it for the time being.
