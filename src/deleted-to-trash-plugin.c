/* Copyright (C) 2008 steven.xu@lba.ca, 2010 Lex Brugman <lex dot brugman at gmail dot com> */
/* Adapted to dovecot 2.0.9 by Emerson Pinter 2011-01-28 19:24 -0200 */
/* Adapted to dovecot 2.1.9 by Emerson Pinter 2012-09-27 17:10 -0300 */
#include "deleted-to-trash-plugin.h"
#include "array.h"
#include "str.h"
#include "str-sanitize.h"
#include "mail-storage-hooks.h"

#include <stdlib.h>

struct mail_namespace *get_users_inbox_namespace(struct mail_user *user)
{
	struct mail_namespace *ns = NULL;
	struct mail_namespace *curns = NULL;

	/* find the inbox namespace */
	for (curns = user->namespaces; curns != NULL; curns = curns->next)
	{
		if (curns->flags & NAMESPACE_FLAG_INBOX_USER)
		{
			ns = curns;
			break;
		}
	}

	return ns;
}

static int search_deleted_id(struct mail *_mail)
{
	unsigned int deleted_id = _mail->uid;
	int ret = 0;

	/**
	 * only if the copy and delete box are the same, otherwise, we need to reset the last copy data
	 * since copy always follows a delete in IMAP (when moving an email from one folder to another folder).
	 */
	if (last_copy.src_mailbox_name != NULL && strcmp(_mail->box->name, last_copy.src_mailbox_name) == 0)
	{
		unsigned int count = 0;
		unsigned int i = 0;
		unsigned int const *mail_ids = NULL;

		mail_ids = array_get(&last_copy.mail_id, &count);
		for (i = 0; i < count; i++)
		{
			if (mail_ids[i] == deleted_id)
			{
				ret = 1;
				break;
			}
		}
	}
	else
	{
		if (array_count(&last_copy.mail_id) > 0)
		{
			array_clear(&last_copy.mail_id);
		}
		if (last_copy.src_mailbox_name != NULL)
		{
			i_free_and_null(last_copy.src_mailbox_name);
		}
	}

	return ret;
}

static struct mailbox *mailbox_open_or_create(struct mailbox_list *list, const char *name, const char **error_r)
{
	struct mailbox *box;
	enum mail_error error;

	box = mailbox_alloc(list, name, MAILBOX_FLAG_NO_INDEX_FILES);
	if (mailbox_open(box) == 0)
	{
		*error_r = NULL;
		return box;
	}

	*error_r = mail_storage_get_last_error(mailbox_get_storage(box), &error);
	if (error != MAIL_ERROR_NOTFOUND)
	{
		mailbox_free(&box);
		return NULL;
	}

	/* try creating and re-opening it. */
	if (mailbox_create(box, NULL, FALSE) < 0 || mailbox_open(box) < 0)
	{
		*error_r = mail_storage_get_last_error(mailbox_get_storage(box), NULL);
		mailbox_free(&box);
		return NULL;
	}
	return box;
}

static int copy_deleted_mail_to_trash(struct mail *_mail)
{
	int ret;
	struct mailbox *trash_box = NULL;
	struct mail_namespace *ns = NULL;
	const char *error;

	ns = get_users_inbox_namespace(_mail->box->list->ns->user);
	trash_box = mailbox_open_or_create(ns->list, trashfolder_name, &error);

	if (trash_box != NULL)
	{
		i_debug("opening %s succeeded", trashfolder_name);
		struct mailbox_transaction_context *dest_trans;
		struct mail_save_context *save_ctx;
		struct mail_keywords *keywords;
		const char *const *keywords_list;

#if DOVECOT_VERSION_MAJOR == 2 && DOVECOT_VERSION_MINOR >= 3
		dest_trans = mailbox_transaction_begin(trash_box, MAILBOX_TRANSACTION_FLAG_EXTERNAL, __func__);
#else
		dest_trans = mailbox_transaction_begin(trash_box, MAILBOX_TRANSACTION_FLAG_EXTERNAL);
#endif

		keywords_list = mail_get_keywords(_mail);
		keywords = str_array_length(keywords_list) == 0 ? NULL : mailbox_keywords_create_valid(trash_box, keywords_list);
		/* dovecot won't set the delete flag at this moment */
		save_ctx = mailbox_save_alloc(dest_trans);
		/* remove the deleted flag for the new copy */
		enum mail_flags flags = mail_get_flags(_mail);
		flags &= ~MAIL_DELETED;
		/* copy the message to the Trash folder */
		mailbox_save_set_flags(save_ctx, flags, keywords);
		ret = mailbox_copy(&save_ctx, _mail);
		if (keywords != NULL) mailbox_keywords_unref(&keywords);

		if (ret < 0)
		{
			mailbox_transaction_rollback(&dest_trans);
			i_debug("copying to %s failed", trashfolder_name);
		}
		else
		{
			ret = mailbox_transaction_commit(&dest_trans);
			i_debug("copying to %s succeeded", trashfolder_name);
		}

		mailbox_free(&trash_box);
	}
	else
	{
		i_debug("opening %s failed", trashfolder_name);
		ret = -1;
	}

	return ret;
}

static void deleted_to_trash_mail_update_flags(struct mail *_mail, enum modify_type modify_type, enum mail_flags flags)
{
	struct mail_private *mail = (struct mail_private *) _mail;
	union mail_module_context *lmail = DELETED_TO_TRASH_MAIL_CONTEXT(mail);
	enum mail_flags old_flags, new_flags;

	old_flags = mail_get_flags(_mail);
	lmail->super.update_flags(_mail, modify_type, flags);

	new_flags = old_flags;
	switch (modify_type)
	{
	case MODIFY_ADD:
		new_flags |= flags;
		break;
	case MODIFY_REMOVE:
		new_flags &= ~flags;
		break;
	case MODIFY_REPLACE:
		new_flags = flags;
		break;
	}

	if (((old_flags ^ new_flags) & MAIL_DELETED) != 0)
	{
		struct mail_namespace *ns = NULL;
		ns = get_users_inbox_namespace(_mail->box->list->ns->user);

		/* marked as deleted and not deleted from the Trash folder */
		if (new_flags & MAIL_DELETED && !(strcmp(_mail->box->name, trashfolder_name) == 0 && strcmp(_mail->box->list->ns->prefix, ns->prefix) == 0))
		{
			if (search_deleted_id(_mail))
			{
				i_debug("email uid=%d was already copied to %s", _mail->uid, trashfolder_name);
			}
			else
			{
				if (copy_deleted_mail_to_trash(_mail) < 0)
				{
					i_fatal("failed to copy %d to %s", _mail->uid, trashfolder_name);
				}
			}
		}
	}
}

void set_trashfolder_name(struct mail_user *user)
{
	trashfolder_name = mail_user_plugin_getenv(user, "deleted_to_trash_folder");
	if (trashfolder_name == NULL) trashfolder_name = DEFAULT_TRASH_FOLDER;
}

static int deleted_to_trash_copy(struct mail_save_context *save_ctx, struct mail *mail)
{
	int ret = 0;

	/* one IMAP copy command can contain many mails ID's, so, this function will be called multiple times */
	union mailbox_module_context *lbox = DELETED_TO_TRASH_CONTEXT(save_ctx->transaction->box);
	ret = lbox->super.copy(save_ctx, mail);
	if (ret >= 0)
	{
		/* if the copy transaction context changes, a new copy could have been started, so add additional conditions to check the folder name */
		i_debug("from %s to %s, previous action from %s", mail->box->name, save_ctx->transaction->box->name, last_copy.src_mailbox_name);
		if (last_copy.transaction_context == save_ctx->transaction && last_copy.src_mailbox_name != NULL && strcmp(last_copy.src_mailbox_name, mail->box->name) == 0)
		{
			array_append(&last_copy.mail_id, &mail->uid, 1);
			i_debug("nr %i", array_count(&last_copy.mail_id));
		}
		else
		{ /* if copying from Trash to some other folder, we don't mark it */
			last_copy.transaction_context = save_ctx->transaction;
			if (array_count(&last_copy.mail_id) > 0)
			{
				array_clear(&last_copy.mail_id);
			}

			if (strcmp(mail->box->name, trashfolder_name) != 0)
			{
				array_append(&last_copy.mail_id, &mail->uid, 1);
				last_copy.src_mailbox_name = i_strdup(mail->box->name);
			}
			else
			{
				i_debug("from %s!", trashfolder_name);
			}
		}
	}

	return ret;
}

static int deleted_to_trash_transaction_commit(struct mailbox_transaction_context *t, struct mail_transaction_commit_changes *changes_r)
{
	union mailbox_module_context *lbox = DELETED_TO_TRASH_CONTEXT(t->box);
	if (last_copy.transaction_context == t)
	{
		last_copy.transaction_context = NULL;
	}
	int ret = lbox->super.transaction_commit(t, changes_r);

	return ret;
}

static void deleted_to_trash_transaction_rollback(struct mailbox_transaction_context *t)
{
	union mailbox_module_context *lbox = DELETED_TO_TRASH_CONTEXT(t->box);
	if (last_copy.transaction_context == t)
	{
		last_copy.transaction_context = NULL;
	}
	lbox->super.transaction_rollback(t);
}

static void deleted_to_trash_mailbox_allocated(struct mailbox *box)
{
	struct mailbox_vfuncs *v = box->vlast;
	union mailbox_module_context *lbox;
	set_trashfolder_name(box->list->ns->user);

	lbox = p_new(box->pool, union mailbox_module_context, 1);
	lbox->super = *v;
	box->vlast = &lbox->super;

	v->copy = deleted_to_trash_copy;
	v->transaction_commit = deleted_to_trash_transaction_commit;
	v->transaction_rollback = deleted_to_trash_transaction_rollback;
	MODULE_CONTEXT_SET_SELF(box, deleted_to_trash_storage_module, lbox);
}

static void deleted_to_trash_mail_allocated(struct mail *_mail)
{
	struct mail_private *mail = (struct mail_private *) _mail;
	struct mail_vfuncs *v = mail->vlast;
	union mail_module_context *lmail;
	set_trashfolder_name(_mail->box->list->ns->user);

	lmail = p_new(mail->pool, union mail_module_context, 1);
	lmail->super = *v;
	mail->vlast = &lmail->super;

	v->update_flags = deleted_to_trash_mail_update_flags;
	MODULE_CONTEXT_SET_SELF(mail, deleted_to_trash_mail_module, lmail);
}

static struct mail_storage_hooks deleted_to_trash_mail_storage_hooks = {
	.mailbox_allocated = deleted_to_trash_mailbox_allocated,
	.mail_allocated = deleted_to_trash_mail_allocated
};

void deleted_to_trash_plugin_init(struct module *module)
{
	last_copy.transaction_context = NULL;
	i_array_init(&last_copy.mail_id, TRASH_LIST_INITSIZE);
	last_copy.src_mailbox_name = NULL;
	mail_storage_hooks_add(module, &deleted_to_trash_mail_storage_hooks);
}

void deleted_to_trash_plugin_deinit(void)
{
	if (last_copy.src_mailbox_name != NULL)
	{
		i_free(last_copy.src_mailbox_name);
	}
	array_free(&last_copy.mail_id);
	mail_storage_hooks_remove(&deleted_to_trash_mail_storage_hooks);
}
