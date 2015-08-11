# NSTU - Natural Scene Text Understanding
# Copyright (C) 2014 Fabio M. Banfi (fbanfi90@gmail.com)

NAME = nstu
CC = g++
#CFLAGSL = -std=c++11 -l:libtesseract.so -O2 `pkg-config --libs opencv`
CFLAGSL = -std=c++11 -l:libtesseract.so -O3 -lopencv_core -lopencv_imgproc -lopencv_highgui
CFLAGSC = -c -std=c++11 -l:libtesseract.so -O3
OBJS = \
	\
	obj/NSTU.o \
	\
	obj/MultipartRequest.o \
	\
	obj/ROIPreProcessor.o \
	\
	obj/DateTime.o \
	obj/Header.o \
	obj/Location.o \
	obj/Tag.o \
	obj/Tagger.o \
	\
	obj/AndrewPerraultAndSauravKumar.o \
	obj/FabioBanfi.o \
	obj/TextDetector.o \
	\
	obj/Utils.o \

all: NSTU

NSTU: obj/main.o $(OBJS)
	$(CC) obj/*.o $(CFLAGSL) -o bin/$(NAME)

# Main

obj/main.o: test/main.cpp
	$(CC) $(CFLAGSC) -Iinclude test/main.cpp -o$@

obj/NSTU.o: src/NSTU.cpp
	$(CC) $(CFLAGSC) -Iinclude src/NSTU.cpp -o$@

# CGI

obj/MultipartRequest.o: src/CGI/MultipartRequest.cpp
	$(CC) $(CFLAGSC) -Iinclude src/CGI/MultipartRequest.cpp -o$@

# ImageProcessing

obj/ROIPreProcessor.o: src/ImageProcessing/ROIPreProcessor.cpp
	$(CC) $(CFLAGSC) -Iinclude src/ImageProcessing/ROIPreProcessor.cpp -o$@

# Tagging

obj/DateTime.o: src/Tagging/DateTime.cpp
	$(CC) $(CFLAGSC) -Iinclude src/Tagging/DateTime.cpp -o$@

obj/Header.o: src/Tagging/Header.cpp
	$(CC) $(CFLAGSC) -Iinclude src/Tagging/Header.cpp -o$@

obj/Location.o: src/Tagging/Location.cpp
	$(CC) $(CFLAGSC) -Iinclude src/Tagging/Location.cpp -o$@

obj/Tag.o: src/Tagging/Tag.cpp
	$(CC) $(CFLAGSC) -Iinclude src/Tagging/Tag.cpp -o$@

obj/Tagger.o: src/Tagging/Tagger.cpp
	$(CC) $(CFLAGSC) -Iinclude src/Tagging/Tagger.cpp -o$@

# TextDetection

obj/AndrewPerraultAndSauravKumar.o: src/TextDetection/AndrewPerraultAndSauravKumar.cpp
	$(CC) $(CFLAGSC) -Iinclude src/TextDetection/AndrewPerraultAndSauravKumar.cpp -o$@

obj/FabioBanfi.o: src/TextDetection/FabioBanfi.cpp
	$(CC) $(CFLAGSC) -Iinclude src/TextDetection/FabioBanfi.cpp -o$@

obj/TextDetector.o: src/TextDetection/TextDetector.cpp
	$(CC) $(CFLAGSC) -Iinclude src/TextDetection/TextDetector.cpp -o$@

# Utils

obj/Utils.o: src/Utils/Utils.cpp
	$(CC) $(CFLAGSC) -Iinclude src/Utils/Utils.cpp -o$@

clean:
	rm -rf *o $(NAME)
