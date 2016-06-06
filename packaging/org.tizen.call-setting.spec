Name:          org.tizen.call-setting
Summary:       Call Settings
Version:       1.0.0
Release:       0
Group:         Applications/Core Applications
License:       Apache-2.0
Source:        %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "wearable" || "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires: cmake
BuildRequires: gettext
BuildRequires: edje-bin
BuildRequires: embryo-bin

BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(contacts-service2)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(ecore-imf)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(msg-service)
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(tapi)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Call Settings application provides functionality to change specific call settings, like
set call-reject messages, configure Answering/Ending calls via Home/Power keys, setup call
forwarding and call waiting options, etc.

%prep
%setup -q

%build
export CFLAGS="${CFLAGS} -fvisibility=hidden -flto"
export CXXFLAGS="${CXXFLAGS} -fvisibility-inlines-hidden -fvisibility=hidden -flto"
export FFLAGS="${FFLAGS} -fvisibility-inlines-hidden -fvisibility=hidden -flto"

%define APPDIR      %{TZ_SYS_RO_APP}/%{name}
%define BINDIR      %{APPDIR}/bin
%define RESDIR      %{APPDIR}/res
%define PACKAGESDIR %{TZ_SYS_RO_PACKAGES}

%define _gbs_buld_dir GBS_BUILD_DIR/%{_project}-%{_arch}

mkdir -p %{_gbs_buld_dir}
cd %{_gbs_buld_dir}

cmake ../../CMake -DAPP_PKG_NAME=%{name} \
        -DAPP_BIN_DIR=%{BINDIR} \
        -DAPP_RES_DIR=%{RESDIR} \
        -DAPP_SHARE_PACKAGES_DIR=%{PACKAGESDIR}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
cd %{_gbs_buld_dir}

%make_install

%clean
rm -f debugfiles.list debuglinks.list debugsources.list

%files
%defattr(-,root,root,-)
%manifest %{name}.manifest
%license LICENSE
%{BINDIR}/*
%{RESDIR}/*
%{PACKAGESDIR}/%{name}.xml
