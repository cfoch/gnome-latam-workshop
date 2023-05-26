builddir = builddir
container_tag = gnome-latam-workshop
# Using $(shell pwd) because $(PWD) is failing inside docker :/
prefix = $(shell pwd)/prefix

.PHONY: all configure compile install test

define GST_LAUNCH_PIPELINE_VIDEOTESTSRC
	gst-launch-1.0 \
		videotestsrc \
			pattern=ball \
		! video/x-raw,format=I420,width=1280,height=720 \
		! v4l2sink \
			device=/dev/video6
endef
define GST_LAUNCH_PIPELINE_V4L2SRC
	gst-launch-1.0 \
		v4l2src \
			device=/dev/video4 \
		! 'image/jpeg' \
		! jpegdec \
		! videoconvert \
		! cheesefilter \
		! v4l2sink \
			device=/dev/video6
endef

all: compile

test: install
	GST_DEBUG=cheese_filter:5 \
	GST_PLUGIN_PATH=$(prefix) \
	XDG_DATA_HOME=$(shell pwd) \
	cheese

install: configure
	cd "$(builddir)" && meson install

compile: configure
	cd "$(builddir)" && meson compile

configure: $(builddir)/build.ninja
$(builddir)/build.ninja:
	rm -fr "$(builddir)"
	meson setup \
		--prefix "$(prefix)" \
		"$(builddir)"

container_test:
container_install:
container_compile:
container_%: .container_built
	./x11docker \
		--share=$(PWD) \
		--webcam \
		$(container_tag) \
		-- \
		make -C "$(PWD)" $*

.container_built: Dockerfile
	docker build ./ --tag "$(container_tag)" && touch "$@"

v4l2_start: .v4l2_pid
v4l2_stop:
	kill $$(cat .v4l2_pid)

.v4l2_pid: v4l2_modprobe install
	GST_PLUGIN_PATH=$(prefix) \
	$(GST_LAUNCH_PIPELINE_V4L2SRC) > /dev/null \
	& echo $$! > $@

v4l2_modprobe:
	lsmod | grep v4l2loopback || \
	sudo modprobe v4l2loopback \
		devices=1 \
		max_buffers=2 \
		exclusive_caps=1 \
		card_label="VirtualCam"

clean:
	-rm -fr "$(builddir)"
	-rm -f .container_built
	-rm -fr "$(prefix)"

mrproper: clean
	-docker image rm "$(container_tag)"
