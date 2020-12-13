

elfasm : process.o assemble.o mnemonics.o symbol.o evaluate.o
	$(CC) $^ -o $@

%.o : %.c
	$(CC) $< -c


clean : 
	rm *.o
	rm elfasm


install : elfasm
	chmod 755 elfasm
	cp ./elfasm /usr/local/bin
