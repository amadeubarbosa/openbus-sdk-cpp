PROJNAME= runtests
APPNAME= runtests

USE_NODEPEND=Yes

run:
	./connect_host_port_connect_to_right_host_and_port
	./connect_host_port_connect_to_wrong_host_and_port
	./connect_orb_connect_getting_orb_with_createORB
	./fail_user_created_orb
	./get_offer
	./register_service

