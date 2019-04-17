OBJS   = ./obj/documentmap.o ./obj/minisearch.o ./obj/msmisc.o ./obj/myindex.o ./obj/postinglist.o ./obj/trie.o ./obj/termsearch.o
SOURCE = ./source/documentmap.c ./source/minisearch.c ./source/msmisc.c ./source/myindex.c ./source/postinglist.c ./source/trie.c ./source/termsearch.c
OUT = minisearch
CC  = gcc


$(OUT): $(SOURCE)
	$(CC) -g -Wextra -Wall $(SOURCE) -o $@ -lm

clean:
	rm -f $(OBJS) $(OUT)
