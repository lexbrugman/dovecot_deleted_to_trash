# Makefile for deleted_to_trash

#### configuration begin ####

# Dovecot's header directory
DOVECOT_INC_PATH = /usr/include/dovecot
# Dovecot's IMAP plugin path
DOVECOT_IMAP_PLUGIN_PATH = /usr/lib/dovecot/modules

## usually no need to configure anything below this line ##

# plugin source & target name #
PLUGIN_SOURCES = deleted-to-trash-plugin.c
PLUGIN_NAME = lib_deleted_to_trash_plugin.so

#### configuration end ####


.PHONY: all build install clean

all: build

build: ${PLUGIN_NAME}

${PLUGIN_NAME}: ${PLUGIN_SOURCES}
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

clean:
	$(RM) ${PLUGIN_NAME}

# EOF
