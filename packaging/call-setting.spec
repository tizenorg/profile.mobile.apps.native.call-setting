Name:          org.tizen.call-setting
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
BuildRequires: pkgconfig(libtzplatform-config)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Call Settings

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

%define _app_lib_name             setting-call
%define _app_home_dir             %{TZ_SYS_RO_APP}/%{name}
%define _app_bin_dir              %{_app_home_dir}/bin
%define _app_lib_dir              %{_app_home_dir}/lib/ug
%define _app_res_dir              %{_app_home_dir}/res
%define _app_data_dir             %{_app_home_dir}/shared/trusted
%define _app_license_dir          %{TZ_SYS_SHARE}/license
%define _share_packages_dir       %{TZ_SYS_RO_PACKAGES}

cmake . \
        -DPKG_NAME=%{name} \
        -DLIB_NAME=%{_app_lib_name} \
        -DAPP_HOME_DIR=%{_app_home_dir} \
        -DAPP_BIN_DIR=%{_app_bin_dir} \
        -DAPP_LIB_DIR=%{_app_lib_dir} \
        -DAPP_RES_DIR=%{_app_res_dir} \
        -DAPP_DATA_DIR=%{_app_data_dir} \
        -DSHARE_PACKAGES_DIR=%{_share_packages_dir} \
%if 0%{?sec_product_feature_app_lite}
        -D_ENABLE_TIZEN_LITE_CODE:BOOL=OFF
%else
        -D_ENABLE_TIZEN_LITE_CODE:BOOL=ON
%endif

make %{?jobs:-j%jobs}

%install
%make_install

#install license file
mkdir -p %{buildroot}%{_app_license_dir}
cp LICENSE %{buildroot}%{_app_license_dir}/%{name}

%post
/sbin/ldconfig
mkdir -p %{_app_bin_dir}

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%dir %{_app_data_dir}
%{_app_lib_dir}/lib%{_app_lib_name}.so
%{_app_res_dir}/edje/call-setting-theme.edj
%{_app_res_dir}/edje/call-setting.edj
%if 0%{?sec_product_feature_call_operator_docomo}
%{_app_res_dir}/edje/call-setting-docomo.edj
%endif
%{_app_res_dir}/edje/ug_effect.edj
%{_app_res_dir}/images/*
%{_app_res_dir}/locale/*/LC_MESSAGES/%{name}.mo
%{_share_packages_dir}/%{name}.xml
%{_app_license_dir}/%{name}

%postun -p /sbin/ldconfig
