dovecot_deleted_to_trash
========================

Purpose
=======

The purpose of this deleted_to_trash-plugin is to fix unexepected behaviour that occurs when using IMAP clients such as outlook, it can not copy deleted email to Trash folder automatically. So, this plugin is designed to copy deleted items to the Trash folder. The plugin needs to tell the difference between "move" and "delete" actions on the client side, since both actions mark the original email as deleted. For a "move" case, we don't copy the email to the trash folder.

Installation
============

Download the sourcecode.

Edit the Makefile to match your environment:

	# Dovecot's header directory
	DOVECOT_INC_PATH = /usr/include/dovecot
	# Dovecot's IMAP plugin path
	DOVECOT_IMAP_PLUGIN_PATH = /usr/lib/dovecot/modules

Run:

	make
	sudo make install

Configuration
=============

Edit the src/95-deleted_to_trash_plugin.conf file and run:

	sudo make configure

Restart Dovecot, and that's all.
