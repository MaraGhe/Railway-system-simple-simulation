all: build

build: src/railway_sim.c
	gcc src/railway_sim.c -o src/railway_sim -g

clean:
	rm src/railway_sim
