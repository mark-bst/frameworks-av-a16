libwvdrmengine source code project need to depend on libwvlevel3.a (third-party static library), this library contains Intel IPP extension related implementation.
So that libwvdrmengine.so can not be compiled on Android Pie/Rvc, can only be generated on Android Nougat by reducing NDK version build(such as NDK 14), and then ported to use it.

PS: libwvdrmengine source code is only submitted on Android Nougat.
