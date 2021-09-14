Name:           inertiablast
Version:        0.92
Release:        2%{?dist}
Summary:        Steal energy pods to defeat the empire
# Almost all is GPLv2+ with some graphics using the other licenses
License:        GPLv2+ and CC0 and CC-BY and (CC-BY or GPLv3)
URL:            http://identicalsoftware.com/inertiablast/

Source0:        %{url}/%{name}-%{version}.tgz

BuildRequires: cmake
BuildRequires: desktop-file-utils
BuildRequires: gcc
BuildRequires: libgamerzilla-devel
BuildRequires: libappstream-glib
BuildRequires: make
BuildRequires: SDL2-devel
BuildRequires: SDL2_mixer-devel
Requires:      hicolor-icon-theme


%description
The rebellion captured several warships but lack the energy pod to
power the ships. You are part of a risky expedition to steal the energy
pods. Defense systems will attempt to stop you. The energy pods are
often stored in tunnels making them hard to retrieve. The massive weight
of the pod increases the difficultly in getting out.

Inertia Blast is a remake of an C64 game called Thrust.


%prep
%setup -q


%build
%cmake
%cmake_build


%install
%cmake_install
\

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/%{name}.desktop
appstream-util validate-relax --nonet %{buildroot}%{_metainfodir}/%{name}.metainfo.xml


%files
%doc README
%license COPYING
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_metainfodir}/%{name}.metainfo.xml
%{_datadir}/applications/%{name}.desktop
%{_datadir}/man/man6/%{name}.6*


%changelog
* Mon Sep 13 2021 Dennis Payne <dulsi@identicalsoftware.com> - 0.92-2
- Fix buildrequires to use SDL2 not SDL.

* Mon Sep 06 2021 Dennis Payne <dulsi@identicalsoftware.com> - 0.92-1
- Upgrade to new release.

* Mon Sep 06 2021 Dennis Payne <dulsi@identicalsoftware.com> - 0.91-2
- Add man page, use name macro everywhere and update license.

* Sun Sep 05 2021 Dennis Payne <dulsi@identicalsoftware.com> - 0.91-1
- Initial build
