# GNOME Latam: Creando efectos para Cheese con GStreamer

Este repositorio es parte de los requisitos para el taller del GNOME Latam 2023
dictado por @cfoch y @FarK.

## Abstracto del taller
Una guía para la creación desde cero de un plugin para GStreamer, con el
objetivo de agregar efectos a la webcam en la aplicación Cheese. Se discutirán
los pasos necesarios para escribir el código del plugin, compilar, instalarlo
y finalmente integrarlo con Cheese. Esta charla puede ser útil tanto para
aquellos que quieran agregar efectos personalizados a Cheese como para quienes
desean aprender más acerca del desarrollo de plugins para GStreamer.

## Tutorial


### Generar boilerplate con gst-template

El repositorio gst-template contiene un conjunto de templates sobre las clases
bases más usadas, que serán útiles para generar el scaffolding del proyecto.

Ejecutar los siguientes comandos:
```
git clone https://gitlab.freedesktop.org/gstreamer/gst-template
cd gst-template/gst-plugin/src
../tools/make_element cheese_filter gsttransform
```

Se generarán los siguientes archivos
```
$ ls  | grep cheesefilter
gstcheesefilter.c
gstcheesefilter.h
```

Estos archivos serán útiles para compilar el primer binario que pueda ser leído
por GStreamer.

