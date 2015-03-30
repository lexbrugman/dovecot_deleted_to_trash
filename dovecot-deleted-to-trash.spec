# do not build debuginfo
#%global debug_package %{nil}

Name:           dovecot-deleted-to-trash
Version:        0.6
Release:        2%{?dist}
Summary:        Dovecot plugin: copy deleted item to Trash folder

Group: Networking/Daemons
Source: %{name}-%{version}.tar.gz

License: GPL

BuildRequires: dovecot-devel, postgresql-devel
Requires: dovecot >= 2.0.15


%description
The purpose of this deleted_to_trash-plugin is that IMAP client,such as outlook doesn't work well with dovecot, it can not copy deleted email to Trash folder automatically. So, this plugin is to copy deleted item to Trash folder. Also, need to tell the difference between "move" and "delete" action on Outlook side, since both action deleted_to_trash marks the original email as deleted. for a "move" case, we don't copy to the trash folder.

%prep
%setup -q

%build
make

%install
make install DESTDIR=%{buildroot} DOVECOT_IMAP_PLUGIN_PATH=%{_libdir}/dovecot
mv %{buildroot}/%{_libdir}/dovecot/lib_deleted_to_trash_plugin.so %{buildroot}/%{_libdir}/dovecot/lib89_deleted_to_trash_plugin.so 

%files
%{_libdir}/dovecot/lib89_deleted_to_trash_plugin.so
%doc 95-deleted_to_trash_plugin.conf

%changelog
* Mon Mar 30 2015 Davide Principi <davide.principi@nethesis.it> - 0.6-2
- Load deleted_to_trash before antispam plugin. Bug #3095 [NethServer]

* Tue Jan 27 2014 Edoardo Spadoni <edoardo.spadoni@nethesis.it> - 0.6
- Update to version 0.6

* Wed Aug 01 2012 Giacomo Sanchietti <giacomo.sanchietti@nethesis.it> - 0.1-1
- First release
