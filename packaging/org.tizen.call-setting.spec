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
BuildRequires: pkgconfig(capi-appfw-app-manager)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-media-metadata-extractor)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-telephony)
BuildRequires: pkgconfig(contacts-service2)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(ecore-imf)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(msg-service)
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(tapi)
BuildRequires: pkgconfig(feedback)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
The application provides functionality to change specific call settings, like
set call-reject messages, turn on/off Home/Power keys support and call
forwarding/waiting options.

%prep
%setup -q

%build
export CFLAGS="${CFLAGS} -fvisibility=hidden"
export CXXFLAGS="${CXXFLAGS} -fvisibility-inlines-hidden -fvisibility=hidden"
export FFLAGS="${FFLAGS} -fvisibility-inlines-hidden -fvisibility=hidden"

%define APPDIR      %{TZ_SYS_RO_APP}/%{name}
%define BINDIR      %{APPDIR}/bin
%define RESDIR      %{APPDIR}/res
%define PACKAGESDIR %{TZ_SYS_RO_PACKAGES}

%define _tmp_buld_dir TEMP_BUILD_DIR/%{_project}-%{_arch}

mkdir -p %{_tmp_buld_dir}
cd %{_tmp_buld_dir}

cmake ../../ -DAPP_PKG_NAME=%{name} \
        -DAPP_BIN_DIR=%{BINDIR} \
        -DAPP_RES_DIR=%{RESDIR} \
        -DAPP_SHARE_PACKAGES_DIR=%{PACKAGESDIR}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
cd %{_tmp_buld_dir}

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

