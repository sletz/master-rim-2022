 
all: gen_square gen_sinus gen_rampe signaux

gen_square : gen_square.c
	gcc -Wall gen_square.c -o gen_square

gen_sinus : gen_sinus.c
	gcc -Wall gen_sinus.c -o gen_sinus

gen_rampe : gen_rampe.c
	gcc -Wall gen_rampe.c -o gen_rampe

signaux : signaux.c
	gcc -Wall signaux.c -o signaux

clean:
	rm -f gen_square gen_rampe gen_sinus signaux
