EAPI="2"

DESCRIPTION="Deleted to trash IMAP plugin for Dovecot"
HOMEPAGE="http://wiki.dovecot.org/Plugins/deleted-to-trash"
RESTRICT="nomirror"
SRC_URI="https://github.com/lexbrugman/dovecot_deleted_to_trash/archive/v0.5.tar.gz -> ${P}.tar"

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
