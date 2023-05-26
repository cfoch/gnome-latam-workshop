builddir = builddir
container_tag = gnome-latam-workshop
# Using $(shell pwd) because $(PWD) is failing inside docker :/
prefix = $(shell pwd)/prefix

.PHONY: all configure compile install test

all: compile

test: install
	GST_DEBUG=cheese_filter:5 \
	GST_PLUGIN_PATH=$(prefix) \
	XDG_DATA_HOME=$(PWD) \
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

container_install:
container_compile:
container_%: .container_built
	docker run \
		--rm \
		--user $(shell id -u):$(shell id -g) \
		--volume "$(PWD)":"/tmp/ws" \
		--workdir "/tmp/ws" \
		$(container_tag) \
		make $*

.container_built: Dockerfile
	docker build ./ --tag "$(container_tag)" && touch "$@"

clean:
	-rm -fr "$(builddir)"
	-rm .container_built
	-rm "$(prefix)"

mrproper: clean
	-docker image rm "$(container_tag)"
