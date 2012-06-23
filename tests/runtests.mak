PROJNAME= runtests
APPNAME= runtests

USE_NODEPEND=Yes

run:
	./fail_user_created_orb
	./connect_host_port_connect_to_wrong_host_and_port
	./get_offer
	./single_signon
	./register_service
	./connect_host_port_connect_to_right_host_and_port
	./connect_host_port_connect_to_wrong_host_and_port
	./connect_host_port_connect_with_empty_host
	./connect_orb_connect_getting_orb_with_createORB
	./connection_login_login_info_after_login
#	./connection_loginByCertificate_entity_id_and_file_private_key_ok ./admin/TestCppHello.key
	./connection_loginByPassword_after_logout
	./connection_loginByPassword_entity_id_and_password_ok
	./connection_loginByPassword_login_when_already_logged
	./connection_loginByPassword_wrong_password
	./connection_manager_create_connection_to_invalid_host
	./connection_manager_create_connection_to_accessible_bus
	./connection_manager_create_connection_to_inaccessible_bus
	./connection_manager_get_manager
