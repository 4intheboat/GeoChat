all: world

world:
	
	make ROOT_SRC=${current_dir} -C tests/src/
	rm -rf tests/src/BUILD
	@ python ./waf -v configure
	@ python ./waf -v
