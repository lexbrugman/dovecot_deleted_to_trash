# Makefile for deleted_to_trash

#### configuration begin ####

# Dovecot's header directory
DOVECOT_INC_PATH = /usr/include/dovecot
# Dovecot's IMAP plugin path
DOVECOT_IMAP_PLUGIN_PATH ?= /usr/lib/dovecot/modules
# Dovecot's config path
DOVECOT_CONFIG_PATH = /etc/dovecot/conf.d 

## usually no need to configure anything below this line ##

# plugin source & target name #
PLUGIN_NAME = lib_deleted_to_trash_plugin.so

# config file
PLUGIN_CONFIG_FILE = 95-deleted_to_trash_plugin.conf

#### configuration end ####

SRCDIR = src
SOURCES := $(wildcard $(SRCDIR)/*.c)

.PHONY: all build install configure clean

all: build

build: ${PLUGIN_NAME}

${PLUGIN_NAME}: ${SOURCES}
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) \
	      -fPIC -shared -Wall \
	      -I${DOVECOT_INC_PATH} \
	      -I${DOVECOT_INC_PATH}/src \
	      -I${DOVECOT_INC_PATH}/src/lib \
	      -I${DOVECOT_INC_PATH}/src/lib-storage \
	      -I${DOVECOT_INC_PATH}/src/lib-mail \
	      -I${DOVECOT_INC_PATH}/src/lib-imap \
	      -I${DOVECOT_INC_PATH}/src/lib-index \
	      -DHAVE_CONFIG_H \
	      $< -o $@

install: install_plugin

install_plugin: ${PLUGIN_NAME}
	install -d ${DESTDIR}/${DOVECOT_IMAP_PLUGIN_PATH}
	install $< ${DESTDIR}/${DOVECOT_IMAP_PLUGIN_PATH}

configure:
	install -d ${DESTDIR}/${DOVECOT_CONFIG_PATH}
	install -m 644 ${PLUGIN_CONFIG_FILE} ${DESTDIR}/${DOVECOT_CONFIG_PATH}

clean:
	$(RM) ${PLUGIN_NAME}

# EOF
