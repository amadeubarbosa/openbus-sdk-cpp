${OPENBUS_HOME}/bin/openssl-generate.ksh -n TesteBarramento${TEC_UNAME}

${OPENBUS_HOME}/bin/run_management.sh --login=tester --password=tester \
  --del-deployment=TesteBarramento${TEC_UNAME}
${OPENBUS_HOME}/bin/run_management.sh --login=tester --password=tester \
  --del-system=TesteBarramento${TEC_UNAME}

${OPENBUS_HOME}/bin/run_management.sh --login=tester --password=tester \
  --add-system=TesteBarramento${TEC_UNAME} --description=desc
${OPENBUS_HOME}/bin/run_management.sh --login=tester --password=tester \
  --add-deployment=TesteBarramento${TEC_UNAME} --system=TesteBarramento${TEC_UNAME} --description=desc --certificate=TesteBarramento${TEC_UNAME}.crt
${OPENBUS_HOME}/bin/run_management.sh --login=tester --password=tester \
  --set-authorization=TesteBarramento${TEC_UNAME} --grant="IDL:IRGSTest:1.0" --no-strict
  