#!/bin/bash

docker build -t gnome_latam_workshop_cheese .
docker run --rm -it \
    -v $PWD:/opt/gnome-latam-workshop \
    -w /opt/gnome-latam-workshop \
    gnome_latam_workshop_cheese bash -c "meson builddir; ninja -C builddir"
