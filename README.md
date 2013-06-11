dovecot_deleted_to_trash
========================

Purpose
=======

The purpose of this deleted_to_trash-plugin is that IMAP client,such as outlook doesn't work well with dovecot, it can not copy deleted email to Trash folder automatically. So, this plugin is to copy deleted item to Trash folder. Also, need to tell the difference between "move" and "delete" action on Outlook side, since both action deleted_to_trash marks the original email as deleted. for a "move" case, we don't copy to the trash folder.

Installation
============

Download the sourcecode.

Edit the Makefile to match your environment:

  # Dovecot's header directory
  DOVECOT_INC_PATH = /usr/include/dovecot
  # Dovecot's IMAP plugin path
  DOVECOT_IMAP_PLUGIN_PATH = /usr/lib/dovecot/imap

Run:

  make
  make install

Configuration
=============

Add the plugin to your mail_plugins in the imap protocol section

  protocol imap {
          [...]
          mail_plugins = deleted_to_trash
  }

Set the name of the trash folder in the plugin section of your configuration file

  plugin {
          [...]
          deleted_to_trash_folder = Trash
  }

Restart Dovecot, and that's all.
