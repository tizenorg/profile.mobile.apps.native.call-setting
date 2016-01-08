Name:          org.tizen.call-setting
Summary:       Call Settings
Version:       0.1.0
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
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(vconf)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
The application provides functionality to change specific call settings, like
set call-reject messages, turn on/off Home/Power keys support and call
forwarding/waiting options.

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

%define _app_name                 call-setting
%define _app_lib_name             ug-%{_app_name}
%define _app_bin_dir              %{TZ_SYS_RO_UG}/bin
%define _app_lib_dir              %{TZ_SYS_RO_UG}/lib
%define _app_res_dir              %{TZ_SYS_RO_UG}/res
%define _app_res_edje_dir         %{TZ_SYS_RO_UG}/res/edje/%{_app_lib_name}
%define _app_res_image_dir        %{TZ_SYS_RO_UG}/res/images/%{_app_lib_name}
%define _app_res_locale_dir       %{TZ_SYS_RO_UG}/res/locale
%define _share_packages_dir       %{TZ_SYS_RO_PACKAGES}

cmake . \
        -DPKG_NAME=%{name} \
        -DLIB_NAME=%{_app_lib_name} \
        -DAPP_LIB_DIR=%{_app_lib_dir} \
        -DAPP_RES_DIR=%{_app_res_dir} \
        -DAPP_RES_EDJE_DIR=%{_app_res_edje_dir} \
        -DAPP_RES_IMAGE_DIR=%{_app_res_image_dir} \
        -DAPP_RES_LOCALE_DIR=%{_app_res_locale_dir} \
        -DSHARE_PACKAGES_DIR=%{_share_packages_dir} \
%if 0%{?sec_product_feature_app_lite}
        -D_ENABLE_TIZEN_LITE_CODE:BOOL=OFF
%else
        -D_ENABLE_TIZEN_LITE_CODE:BOOL=ON
%endif

make %{?_smp_mflags}

%install
%make_install
%find_lang %{_app_lib_name}

%post
/sbin/ldconfig

mkdir -p  %{_app_bin_dir}
ln -sf %{TZ_SYS_BIN}/ug-client %{TZ_SYS_RO_UG}/bin/%{_app_name}

%files -f %{_app_lib_name}.lang
%license LICENSE

%manifest %{name}.manifest
%{_app_lib_dir}/lib%{_app_lib_name}.so
%{_app_res_edje_dir}
%{_app_res_image_dir}
%{_share_packages_dir}/%{name}.xml

%postun -p /sbin/ldconfig
