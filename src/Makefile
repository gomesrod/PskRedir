##############################################
# Makefile for building the project on Linux #
##############################################

temp_obj_dir=PskRedir/Release
release_dir=Release
src_dir=PskRedir

# compiler=g++ -g
compiler=g++

all: compile
	$(compiler) -o $(release_dir)/PskRedir $(temp_obj_dir)/*.o	

compile: create_dirs
	$(compiler) -c -o $(temp_obj_dir)/utils.o $(src_dir)/utils.cpp
	$(compiler) -c -o $(temp_obj_dir)/AppConfig.o $(src_dir)/AppConfig.cpp
	$(compiler) -c -o $(temp_obj_dir)/SimpleSocket.o $(src_dir)/SimpleSocket.cpp
	$(compiler) -c -o $(temp_obj_dir)/BaseRedirEngine.o $(src_dir)/BaseRedirEngine.cpp
	$(compiler) -c -o $(temp_obj_dir)/ListenConnRedirEngine.o $(src_dir)/ListenConnRedirEngine.cpp
	$(compiler) -c -o $(temp_obj_dir)/ListenListenRedirEngine.o $(src_dir)/ListenListenRedirEngine.cpp
	$(compiler) -c -o $(temp_obj_dir)/ConnConnRedirEngine.o $(src_dir)/ConnConnRedirEngine.cpp
	$(compiler) -c -o $(temp_obj_dir)/Main.o $(src_dir)/Main.cpp

create_dirs:
	if [ ! -d $(release_dir) ]; then mkdir $(release_dir); fi
	if [ ! -d $(temp_obj_dir) ]; then mkdir $(temp_obj_dir); fi

clean:
	rm -f $(temp_obj_dir)/* 
	rm -f $(release_dir)/*
