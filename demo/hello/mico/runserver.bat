call democonfigWin.cmd

server.exe -OpenbusHost %HostBus% -OpenbusPort %PortBus% -OpenbusDebug ALL -EntityName %EntityName% -PrivateKeyFilename %PrivateKeyFilename% -ACSCertificateFilename %ACSCertificateFilename%  -FacetName %FacetName%
