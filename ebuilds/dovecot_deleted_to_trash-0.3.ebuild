EAPI="2"

DESCRIPTION="Deleted to trash IMAP plugin for Dovecot"
HOMEPAGE="http://wiki.dovecot.org/Plugins/deleted-to-trash"
RESTRICT="nomirror"
SRC_URI="http://wiki.dovecot.org/Plugins/deleted-to-trash?action=AttachFile&do=get&target=${P}.tar.gz -> ${P}.tar.gz"

LICENSE=""
KEYWORDS="~amd64 ~x86"
SLOT="0"

IUSE=""
RDEPEND="=net-mail/dovecot-1.2*"

src_install() {
	emake DESTDIR="${D}" install || die
}
