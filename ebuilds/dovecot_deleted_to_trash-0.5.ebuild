EAPI="2"

DESCRIPTION="Deleted to trash IMAP plugin for Dovecot"
HOMEPAGE="http://wiki.dovecot.org/Plugins/deleted-to-trash"
RESTRICT="nomirror"
SRC_URI="http://wiki2.dovecot.org/Plugins/deleted-to-trash?action=AttachFile&do=get&target=deleted-to-trash-plugin_0.3_for_dovecot_2.1.tar -> ${P}.tar"

LICENSE=""
KEYWORDS="~amd64 ~x86"
SLOT="0"

IUSE=""
RDEPEND="=net-mail/dovecot-2.1*"

src_prepare() {
	sed -i 's/DOVECOT_IMAP_PLUGIN_PATH = \/usr\/lib\/dovecot\/imap/DOVECOT_IMAP_PLUGIN_PATH = \/usr\/lib\/dovecot/' Makefile
}

src_install() {
	emake DESTDIR="${D}" install || die
}
