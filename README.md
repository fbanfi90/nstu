# nstu
NSTU (Natural Scene Text Understanding) is a software library which aims at analyzing streams of natural images in real time, look for text in them, and subsequently understand (recognize) it. NSTU integrates the Tesseract OCR (code.google.com/p/tesseract-ocr) with the open-source computer vision library OpenCV (opencv.org). In order to locate text in images, it uses some free implementations of the recently developed theory of Stroke Width Transform (research.microsoft.com/apps/pubs/default.aspx?id=149305), such as TiRG (sourceforge.net/projects/tirg) and the one by Andrew Perrault and Saurav Kumar (github.com/aperrau/DetectText).
