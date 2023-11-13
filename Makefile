build:
	gcc serial.c -o serial
	gcc gen_inputs.c -lm -o gen_inputs
clean:
	rm serial gen_inputs && rm *.txt