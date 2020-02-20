Name:     wdisplays
Version:  0.1
Release:  0.1.20200219git.ba331ca%{?dist}.wef
Summary:  GUI display configurator for wlroots compositors
License:  MIT
URL:      https://github.com/cyclopsian/wdisplays

# use this to fetch the source: spectool -g wdisplays.spec
Source0:  %{url}/archive/v%{version}/%{name}-%{version}.tar.gz

BuildRequires: gcc
BuildRequires: gtk3-devel
BuildRequires: meson
BuildRequires: wayland-devel

%description

wdisplays is a graphical application for configuring displays in
Wayland compositors. It borrows some code from kanshi. It should work
in any compositor that implements the
wlr-output-management-unstable-v1 protocol, including sway. The goal
of this project is to allow precise adjustment of display settings in
kiosks, digital signage, and other elaborate multi-monitor setups.

%prep
%autosetup

%build
%meson
%meson_build

%install
%meson_install

%files
%{_bindir}/%{name}
%{_datadir}/applications/*
%{_datadir}/icons/*

%doc README.md

%license LICENSE

%changelog
* Wed Feb 19 2020 Bob Hepple <bob.hepple@gmail.com> - 0.1-0.1.20200219git.ba331ca.fc31.wef
- Initial version of the package
