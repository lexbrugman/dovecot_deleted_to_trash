#ifndef __DELETED_TO_TRASH_PLUGIN_H
#define __DELETED_TO_TRASH_PLUGIN_H

#include "lib.h"
#include "mail-storage-private.h"
#include "mail-namespace.h"
#include "mailbox-list-private.h"

#define DEFAULT_TRASH_FOLDER "Trash"
#define TRASH_LIST_INITSIZE 128

#define DELETED_TO_TRASH_CONTEXT(obj) MODULE_CONTEXT(obj, deleted_to_trash_storage_module)
#define DELETED_TO_TRASH_MAIL_CONTEXT(obj) MODULE_CONTEXT(obj, deleted_to_trash_mail_module)


ARRAY_DEFINE_TYPE(mail_ids, unsigned int);

struct last_copy_info
{
	void *transaction_context;
	ARRAY_TYPE(mail_ids) mail_id;
	char *src_mailbox_name;
};

/* defined by imap, pop3, lda */
const char *deleted_to_trash_plugin_version = DOVECOT_ABI_VERSION;

static MODULE_CONTEXT_DEFINE_INIT(deleted_to_trash_storage_module, &mail_storage_module_register);
static MODULE_CONTEXT_DEFINE_INIT(deleted_to_trash_mail_module, &mail_module_register);

static struct last_copy_info last_copy;

const char *trashfolder_name = NULL;

void deleted_to_trash_plugin_init(struct module *module);
void deleted_to_trash_plugin_deinit(void);
void set_trashfolder_name(struct mail_user *user);

#endif
