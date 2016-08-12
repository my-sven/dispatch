################.spec##############
Name: data_filter
Version: 1.0
Release: 1
Summary: data_filter
License: GPL
Group: root
%description
%define userpath /home/bin

%prep

%build

%install

install -d $RPM_BUILD_ROOT%{_bindir}
cp -a /home/sven/data_filter/data_filter $RPM_BUILD_ROOT%{_bindir}
cp -a /home/sven/data_filter/filter_config.xml $RPM_BUILD_ROOT%{_bindir}

%files
%defattr(755,root,root)
%{_bindir}/data_filter
%{_bindir}/filter_config.xml

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
* Wed Jul 20 2016 data_filter
--change data_filter log

%per
%post
%perun
%postun

###################################
