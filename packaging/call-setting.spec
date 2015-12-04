Name:          call-setting
Summary:       Call Settings
Version:       0.8.8
Release:       1
Group:         Applications/Core Applications
License:       Apache-2.0
Source0:       %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "wearable" || "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(tapi)
BuildRequires: pkgconfig(vconf)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(ecore-imf)
BuildRequires: pkgconfig(ecore-file)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(db-util)
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(contacts-service2)
BuildRequires: pkgconfig(msg-service)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-appfw-app-manager)
BuildRequires: pkgconfig(capi-telephony)
BuildRequires: pkgconfig(notification)
BuildRequires: cmake
BuildRequires: gettext
BuildRequires: pkgconfig(efl-extension)
BuildRequires: edje-bin, embryo-bin
BuildRequires: pkgconfig(capi-content-media-content)
BuildRequires: pkgconfig(capi-media-metadata-extractor)
BuildRequires: pkgconfig(capi-system-system-settings)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Call Settings.

%prep
%setup -q

%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif
export CFLAGS="${CFLAGS} -fvisibility=hidden"
export CXXFLAGS+="${CXXFLAGS} -fvisibility-inlines-hidden -fvisibility=hidden"
export FFLAGS+="${FFLAGS} -fvisibility-inlines-hidden -fvisibility=hidden"
export LDFLAGS+="-Wl,--hash-style=both -Wl,--rpath=%{_prefix}/lib -Wl,--as-needed,--unresolved-symbols=ignore-in-shared-libs"

%define PREFIX /usr
%define APPDIR %{PREFIX}/apps
%define PKGDIR %{APPDIR}/setting-call-efl
%define BINDIR %{PKGDIR}/bin
%define LIBDIR %{PKGDIR}/lib/ug
%define RESDIR %{PKGDIR}/res
%define DATADIR %{PKGDIR}/shared/trusted

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} \
-DSYSLIBDIR=%{_libdir} \
%if 0%{?sec_product_feature_call_operator_docomo}
-D_ENABLE_CALL_SET_OPERATOR_DOCOMO:BOOL=OFF \
%else
-D_ENABLE_CALL_SET_OPERATOR_DOCOMO:BOOL=OFF \
%endif
%if 0%{?sec_product_feature_app_lite}
-D_ENABLE_TIZEN_LITE_CODE:BOOL=OFF
%else
-D_ENABLE_TIZEN_LITE_CODE:BOOL=ON
%endif
make %{?jobs:-j%jobs}

%install
%make_install

mkdir -p %{buildroot}/opt/usr/ug/data/ug-setting-call-efl

#install license file
mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

%post
/sbin/ldconfig
chown -R 5000:5000 /opt/usr/ug/data/ug-setting-call-efl
mkdir -p %{BINDIR}

%files
%manifest call-setting.manifest
%defattr(-,root,root,-)
%attr(-,inhouse,inhouse) %{DATADIR}
%{LIBDIR}/libsetting-call-efl.so
%{RESDIR}/edje/call-setting-theme.edj
%{RESDIR}/edje/call-setting.edj
%if 0%{?sec_product_feature_call_operator_docomo}
%{RESDIR}/edje/call-setting-docomo.edj
%endif
%{RESDIR}/edje/ug_effect.edj
%{RESDIR}/images/*
%{RESDIR}/locale/*/LC_MESSAGES/setting-call-efl.mo
/etc/smack/accesses2.d/ug.setting-call-efl.include
%dir %{DATADIR}
%dir /opt/usr/ug/data/ug-setting-call-efl
/usr/share/packages/setting-call-efl.xml
/usr/share/license/%{name}

%postun -p /sbin/ldconfig
