Name:           inertiablast
Version:        0.91
Release:        1%{?dist}
Summary:        Steal energy pods to defeat the empire
License:        GPLv2+
URL:            http://identicalsoftware.com/inertiablast/

Source0:        %{url}/%{name}-%{version}.tgz

BuildRequires: cmake
BuildRequires: desktop-file-utils
BuildRequires: gcc-c++
BuildRequires: libgamerzilla-devel
BuildRequires: libappstream-glib
BuildRequires: make
BuildRequires: SDL-devel
BuildRequires: SDL_mixer-devel
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


%check
desktop-file-validate %{buildroot}%{_datadir}/applications/inertiablast.desktop
appstream-util validate-relax --nonet %{buildroot}%{_metainfodir}/inertiablast.metainfo.xml


%files
%doc README
%license COPYING
%{_bindir}/inertiablast
%{_datadir}/inertiablast
%{_datadir}/icons/hicolor/*/apps/inertiablast.png
%{_metainfodir}/%{name}.metainfo.xml
%{_datadir}/applications/inertiablast.desktop


%changelog
* Sun Sep 05 2021 Dennis Payne <dulsi@identicalsoftware.com> - 0.91-1
- Initial build
