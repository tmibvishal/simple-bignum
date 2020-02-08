CC=gcc
objPath=./obj
exePath=./

default: pre_build $(objPath)/main.o
	$(CC) $(wildcard ${objPath}/*) -o $(exePath)/mainfile;

$(objPath)/%.o: %.c
	$(CC) -c $^ -o $@;

pre_build: 
	[ -d $(exePath) ] || mkdir -p $(exePath); \
	[ -d $(objPath) ] || mkdir -p $(objPath);

run:
	$(exePath)/mainfile

# avoid confusion for makefile when there is a clean and run file in this directory
.PHONY : clean run

clean:
	# removing all objects, executable files
	rm -rf $(objPath)
	rm -rf $(exePath)/mainfile
