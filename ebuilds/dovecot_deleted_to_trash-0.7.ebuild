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
DEPEND="=net-mail/dovecot-2*"

src_unpack() {
	default
	[[ ${PV} == "9999" ]] && git-2_src_unpack
}

src_install() {
	emake DESTDIR="${D}" DOVECOT_IMAP_PLUGIN_PATH="/usr/$(get_libdir)/dovecot" install || die
	emake DESTDIR="${D}" configure || die
}
