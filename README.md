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

### Cambiar la clase base a GstOpencvVideoFilter

Hasta este momento, el plugin hereda de GstBaseTransform, como se puede ver
ejecutando `gst-inspect-1.0 --gst-plugin-load ./builddir/gst/cheese/libgstcheese.so cheesefilter`.

```
GObject
 +----GInitiallyUnowned
       +----GstObject
             +----GstElement
                   +----GstBaseTransform
                         +----Gstcheesefilter
```

GstBaseTranform es una clase base para filtros, que provee dos funciones
principalmente:

- [transform](https://gstreamer.freedesktop.org/documentation/base/gstbasetransform.html?gi-language=c#GstBaseTransformClass::transform):
  recibe un buffer de entrada (*inbuf*) y uno de salida (*outbuf*) ya previamente reservados en memoria. El desarrollador deberá colocar
  el resultado en *outbuf*.
- [transform_ip](https://gstreamer.freedesktop.org/documentation/base/gstbasetransform.html?gi-language=c#GstBaseTransformClass::transform_ip):
  recibe un buffer de entrada (*inbuf*) que será reusado como buffer de salida.

GstOpencvVideoFilter hereda de GstBaseTransform, e implementa ambas funciones
virtuales, envolviendo los buffers en [cv::Mat](https://docs.opencv.org/4.x/d3/d63/classcv_1_1Mat.html),
y volviéndolas a exportar en las funciones virtuales análogas:

- [cv_trans_func](https://gitlab.freedesktop.org/gstreamer/gstreamer/-/blob/main/subprojects/gst-plugins-bad/gst-libs/gst/opencv/gstopencvvideofilter.cpp#L129):
- [cv_trans_ip_func]((https://gitlab.freedesktop.org/gstreamer/gstreamer/-/blob/main/subprojects/gst-plugins-bad/gst-libs/gst/opencv/gstopencvvideofilter.cpp#L152)

Ya que es más sencillo trabajar con [cv::Mat](https://docs.opencv.org/4.x/d3/d63/classcv_1_1Mat.html), heredaremos de esta clase base. Para
ello, podemos reemplazar en nuestro código todas las referencias a GstBaseTransform por GstOpencvVideoFilter. Además será necesario en meson,
agregar dependencias a `opencv` y enlazar con `libgstopencv-1.0.so`.

Una vez hecho ello, se puede ver con gst-inspect-1.0, el siguiente árbol de
herencias:

```
GObject
 +----GInitiallyUnowned
       +----GstObject
             +----GstElement
                   +----GstBaseTransform
                         +----GstVideoFilter
                               +----GstOpencvVideoFilter
                                     +----Gstcheesefilter
```

