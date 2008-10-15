# norootforbuild
%define fedora_version 8

%if 0%{?fedora_version} != 0
%define breq qt4-devel
%define qmake /usr/bin/qmake-qt4
%define lrelease /usr/bin/lrelease-qt4
%endif  
%if 0%{?suse_version} != 0
%define breq libqt4-devel
%define qmake /usr/bin/qmake
%define lrelease /usr/bin/lrelease
%endif  
%if 0%{?mandriva_version} != 0
%define breq libqt4-devel, qt4-linguist
%define qmake /usr/lib/qt4/bin/qmake
%define lrelease /usr/lib/qt4/bin/lrelease
%endif

Name:           juffed
Version:        0.5
Release:        svn
License:        GPL
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Source:		%{name}_%{version}-%{release}.tar.bz2
Group:          Utility
Summary:        Simple tabbed text editor
BuildRequires:  gcc-c++, %{breq}, qscintilla2-devel


%description
Simple tabbed text editor with syntax highlighting for C++, Python, HTML, PHP, XML, TeX, Makefiles, ini-files and patch-files


%prep
%setup -q -n %{name}_%{version}-%{release}


%build
./configure --qmake=%{qmake} --lrelease=%{lrelease}
#%{make} QMAKE=%{qmake} LRELEASE=%{lrelease}
%{__make}


%install
%{__rm} -rf %{buildroot}
%{makeinstall} FAKE_ROOT=%{buildroot}


%clean
%{__rm} -rf %{buildroot}


%files
%doc COPYING ChangeLog
%defattr(-,root,root)
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%{_datadir}/%{name}


%changelog
* Thu Apr 24 2008 TI_Eugene <ti.eugene@gmail.com> 0.2.1-1
- Initial build on OBS

* Thu Apr 24 2008 TI_Eugene <ti.eugene@gmail.com> 0.2.1-1
- Initial build on OBS
