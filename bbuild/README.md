## Dependências

Clique no link para baixar o respectivo pacote.

[scs](https://git.tecgraf.puc-rio.br/scs/scs-core-cpp/tree/master)

[logger](https://git.tecgraf.puc-rio.br/engdist/cpp-logger/tree/master)

[boost](http://webserver2.tecgraf.puc-rio.br/ftp_pub/openbus/repository/boost-1.51.0.tar.bz2)

[openssl](http://webserver2.tecgraf.puc-rio.br/ftp_pub/openbus/repository/openssl-1.0.0o.tar.gz)

[openssl.jam](https://git.tecgraf.puc-rio.br/boost-build/openssl/tree/master)

[ACE_TAO (i)](http://webserver2.tecgraf.puc-rio.br/ftp_pub/openbus/repository/ACE+TAO-6.4.0.tar.gz)

[ace_tao.jam](https://git.tecgraf.puc-rio.br/boost-build/ace-tao/tree/master)

[taoidl](https://git.tecgraf.puc-rio.br/boost-build/tao-idl/tree/master)

[boost-build (i)](http://webserver2.tecgraf.puc-rio.br/ftp_pub/openbus/repository/boost-build-2014-10_tecgraf_28112014snapshot.tgz)

(i) Versões com *patches* aplicados pela TecGraf.

## *Build*
0. É necessário ter a Boost Build, OpenSSL e ACE+TAO
instaladas. [Instalação da Boost
Build](https://jira.tecgraf.puc-rio.br/confluence/x/vYq_B),
[Instalação da OpenSSL 1.0.0]
(https://jira.tecgraf.puc-rio.br/confluence/x/wYq_B) e [Instalação da
ACE+TAO 6.3 ou 6.4]
(https://jira.tecgraf.puc-rio.br/confluence/x/7gTXB).
1. Escolher um diretório raiz para o *build* (`$BUILD`) e disponibilizar
cada uma das dependências como um subdiretório com o nome da
dependência conforme consta na seção [Dependências](#Dependências).Por exemplo:
`$BUILD/scs`,`$BUILD/logger`, `$BUILD/boost` e assim por diante.
2. Disparar a Boost Build no diretório `bbuild` informando o local 
da instalação da OpenSSL:

### Unix

```bash
cd bbuild
$INSTALL/boost-build/bin/b2 warnings=off \
  -sOPENSSL_INSTALL=$OPENSSL_INSTALL

```

### Windows

```
cd bbuild
%INSTALL%\boost-build\bin\b2 warnings=off ^
  -sOPENSSL_INSTALL=%OPENSSL_INSTALL%
```

O local de instalação da biblioteca OpenSSL pode ser informado através
da variável `OPENSSL_INSTALL`. Como alternativa, os diretórios `include` e `lib`
podem ser informados de forma separada através das variáveis
`OPENSSL_INC` e `OPENSSL_LIB`.

As outras dependências são buscadas automaticamente no diretório pai
do pacote OpenBus SDK C++. Para cada dependência apresentada na seção
[Dependências](#Dependências) , a Boost Build procura um diretório com
o nome da dependência conforme descrito. O diretório deve conter a
extração do pacote da dependência. É possível informar caminhos
customizados para cada uma das dependências através das seguintes
variáveis de ambiente:

`SCS`

`LOGGER`

`BOOST`

`OPENSSL_INSTALL`

`OPENSSL_JAM`

`ACE_TAO`

`ACE_TAO_JAM`

`TAOIDL`

As variáveis acima podem ser fornecidas para a Boost Build através do
argumento `-sVAR=value`, por exemplo, `-sACE_TAO=/path/to/ace_tao`.

Os produtos do *build* são disponibilizados em 
`bbuild\stage`. 