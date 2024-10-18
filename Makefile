all: build

build: railway_sim.c
	gcc railway_sim.c -o railway_sim -g

clean:
	rm railway_sim
