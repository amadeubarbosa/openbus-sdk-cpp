
::Máquina em que está o barramento.
set HostBus=localhost

::Porta da máquina em que está o barramento.
set PortBus=2089

::Nome do usuário Openbus a ser utilizado pelo processo cliente.
set BusUser=tester

::Senha do usuário Openbus a ser utilizada pelo processo cliente.
set BusPassword=tester

::Nome da entidade a ser autenticada através de um certificado digital.
set EntityName=HelloService

::Nome do arquivo referente a chave privada da entidade.
set PrivateKeyFilename=HelloService.key

::Nome do arquivo referente ao certificado do ACS.
set ACSCertificateFilename=AccessControlService.crt

::Nome da faceta referente ao serviço Hello.
set FacetName=IHello_II
