EAPI="7"

DESCRIPTION="Deleted to trash IMAP plugin for Dovecot"
HOMEPAGE="http://wiki.dovecot.org/Plugins/deleted-to-trash"
RESTRICT="nomirror"
if [[ ${PV} == "9999" ]] ; then
	EGIT_REPO_URI="https://github.com/lexbrugman/${PN}.git"
	inherit git-r3 autotools
else
	SRC_URI="https://github.com/lexbrugman/${PN}/archive/v${PV}.tar.gz -> ${P}.tar.gz"
fi

LICENSE=""
KEYWORDS="~amd64 ~x86"
SLOT="0"

IUSE=""
DEPEND="=net-mail/dovecot-2*
	!!>=net-mail/dovecot-2.3"

src_unpack() {
	default
	[[ ${PV} == "9999" ]] && git-2_src_unpack
}

src_prepare() {
	sed -i 's/DOVECOT_IMAP_PLUGIN_PATH = \/usr\/lib\/dovecot\/modules/DOVECOT_IMAP_PLUGIN_PATH = \/usr\/lib\/dovecot/' Makefile
	eapply_user
}

src_install() {
	emake DESTDIR="${D}" install || die
	emake DESTDIR="${D}" configure || die
}
